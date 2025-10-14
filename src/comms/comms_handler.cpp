/**
 * J1939 Diagnostic Tool - Comms Handler with HTTP File Server
 * 
 * Versão: 3.8.0
 */

#include "comms_handler.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet_chksum.h"
#include "lwip/raw.h"
#include "lwip/prot/ip.h"
#include "lwip/icmp.h"
#include "../core/config.h"
#include "../core/error_handler.h"
#include "../core/time_handler.h"

// --- Module State & Clients ---
static CommsState current_state = COMMS_STATE_STARTING;
static WiFiStatus wifi_status = WIFI_STATUS_DISCONNECTED;
WebServer http_server(80);
File fs_upload_file;
// ... other clients

// --- HTTP Server Handlers ---
void handle_file_list() {
    String json = "{\"files\":[";
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    bool first = true;
    while(file){
        if (!first) {
            json += ",";
        }
        json += "{\"name\":\"" + String(file.name()) + "\",\"size\":" + String(file.size()) + "}";
        first = false;
        file = root.openNextFile();
    }
    json += "]}";
    http_server.send(200, "application/json", json);
}

void handle_file_download() {
    String path = http_server.arg("path");
    if (!path.startsWith("/")) path = "/" + path;
    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        http_server.streamFile(file, "application/octet-stream");
        file.close();
    } else {
        http_server.send(404, "text/plain", "File Not Found");
    }
}

void handle_file_upload() {
    HTTPUpload& upload = http_server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        fs_upload_file = LittleFS.open(filename, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fs_upload_file) fs_upload_file.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fs_upload_file) fs_upload_file.close();
        http_server.send(200, "text/plain", "File Uploaded Successfully");
    }
}

void setup_http_server() {
    http_server.on("/files", HTTP_GET, handle_file_list);
    http_server.on("/download", HTTP_GET, handle_file_download);
    http_server.on("/upload", HTTP_POST, []() { http_server.send(200); }, handle_file_upload);
    http_server.begin();
    error_report(ErrorLevel::INFO, "HTTP", "HTTP File Server started.");
}

// --- Custom Ping Function using LwIP ---
bool ping_host(const char* host) {
    ip_addr_t target_addr;
    struct hostent *he;
    if (!ipaddr_aton(host, &target_addr)) {
        // If DNS resolution is needed
        he = lwip_gethostbyname(host);
        if (he == NULL) {
            error_report(ErrorLevel::WARN, "PING", "Failed to resolve host");
            return false;
        }
        struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
        if (addr_list[0] == NULL) {
            error_report(ErrorLevel::WARN, "PING", "No IP address found for host");
            return false;
        }
        target_addr.u_addr.ip4.addr = addr_list[0]->s_addr;
    }

    int sock = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
    if (sock < 0) {
        error_report(ErrorLevel::WARN, "PING", "Failed to create raw socket");
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct icmp_echo_hdr *iecho;
    char packet[sizeof(struct icmp_echo_hdr) + 32];
    iecho = (struct icmp_echo_hdr *)packet;
    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = 0xABCD;
    iecho->seqno = htons(1);

    // Fill with some data
    for (int i = sizeof(struct icmp_echo_hdr); i < sizeof(packet); i++) {
        packet[i] = i;
    }

    iecho->chksum = inet_chksum(packet, sizeof(packet));

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = target_addr.u_addr.ip4.addr;
    dest_addr.sin_len = sizeof(dest_addr);

    if (lwip_sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        error_report(ErrorLevel::WARN, "PING", "Failed to send ICMP packet");
        lwip_close(sock);
        return false;
    }

    char rx_buffer[64];
    struct sockaddr_in from_addr;
    int from_len = sizeof(from_addr);

    int len = lwip_recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&from_addr, (socklen_t*)&from_len);
    lwip_close(sock);

    if (len > 0) {
        struct ip_hdr *iphdr = (struct ip_hdr *)rx_buffer;
        struct icmp_echo_hdr *icmp_reply = (struct icmp_echo_hdr *)(rx_buffer + (IPH_HL(iphdr) * 4));
        if (ICMPH_TYPE(icmp_reply) == ICMP_ER) {
            error_report(ErrorLevel::INFO, "PING", "Ping successful");
            return true;
        }
    }
    
    error_report(ErrorLevel::WARN, "PING", "Ping failed");
    return false;
}

// --- State Machine Task ---
void comms_task_fn(void* pv) {
    for (;;) {
        switch (current_state) {
            case COMMS_STATE_STARTING:
                wifi_status = WIFI_STATUS_CONNECTING;
                current_state = COMMS_STATE_WIFI_CONNECTING;
                break;

            case COMMS_STATE_WIFI_CONNECTING:
                if (WiFi.status() == WL_CONNECTED) {
                    wifi_status = WIFI_STATUS_CONNECTED;
                    setup_http_server();
                    time_sync(); 
                    current_state = COMMS_STATE_MQTT_CONNECTING;
                } 
                break;

            case COMMS_STATE_MQTT_CONNECTING:
                if(ping_host("google.com")) {
                    wifi_status = WIFI_STATUS_INTERNET;
                } else {
                    wifi_status = WIFI_STATUS_CONNECTED; // Connected but no internet
                }
                // ... MQTT connection logic
                current_state = COMMS_STATE_OPERATIONAL;
                break;

            case COMMS_STATE_OPERATIONAL:
                http_server.handleClient();
                break;

            case COMMS_STATE_WIFI_DISCONNECTED:
                wifi_status = WIFI_STATUS_DISCONNECTED;
                http_server.stop();
                current_state = COMMS_STATE_STARTING;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// --- Public API ---
void comms_handler_init() {
    xTaskCreate(comms_task_fn, "Comms Task", 8192, NULL, 5, NULL);
}

WiFiStatus comms_get_wifi_status() {
    return wifi_status;
}

bool comms_publish_telemetry(const char* payload) {
    // ... (implementation)
    return false;
}
