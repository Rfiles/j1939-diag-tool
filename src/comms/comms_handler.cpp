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
#include "lwip/inet.h"
#include "lwip/ip_addr.h" 
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"

#include "../core/config.h"
#include "../core/error_handler.h"
#include "../core/time_handler.h"

// --- Module State & Clients ---
static CommsState current_state = COMMS_STATE_STARTING;
static WiFiStatus wifi_status = WIFI_STATUS_DISCONNECTED;
WebServer http_server(80);
File fs_upload_file;
// ... other clients

// --- Ping Implementation ---

#define PING_TIMEOUT_MS 1000
#define PING_ID 0xABCD

static bool ping_success = false;
static bool ping_in_progress = false;
static ip_addr_t ping_target;

static void ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr) {
    struct icmp_echo_hdr *iecho;
    if (p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) {
        iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + PBUF_IP_HLEN);
        if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(1))) {
            ping_success = true;
        }
    }
    pbuf_free(p);
    ping_in_progress = false;
}

static void ping_send(struct raw_pcb *pcb, ip_addr_t *addr) {
    struct pbuf *p = pbuf_alloc(PBUF_IP, sizeof(struct icmp_echo_hdr), PBUF_RAM);
    if (!p) return;

    struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)p->payload;
    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = lwip_htons(1);

    iecho->chksum = inet_chksum(iecho, sizeof(struct icmp_echo_hdr));
    raw_sendto(pcb, p, addr);
    pbuf_free(p);
}

static void dns_found(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr) {
        ping_target = *ipaddr;
    }
}

bool start_ping(const char* host) {
    if (ping_in_progress) return false;

    err_t err = dns_gethostbyname(host, &ping_target, dns_found, NULL);
    if (err == ERR_INPROGRESS) return true; // DNS request sent
    if (err == ERR_OK) { // Already in cache
        struct raw_pcb *pcb = raw_new(IP_PROTO_ICMP);
        if (!pcb) return false;

        raw_recv(pcb, ping_recv, NULL);
        raw_bind(pcb, IP_ADDR_ANY);
        ping_in_progress = true;
        ping_success = false;
        ping_send(pcb, &ping_target);
        raw_remove(pcb);
        return true;
    }
    return false;
}

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
                    current_state = COMMS_STATE_INTERNET_CHECK;
                } 
                break;

            case COMMS_STATE_INTERNET_CHECK:
                if (start_ping("google.com")) {
                    vTaskDelay(pdMS_TO_TICKS(PING_TIMEOUT_MS)); // Wait for ping response
                    if (ping_success) {
                        wifi_status = WIFI_STATUS_INTERNET;
                    } else {
                        wifi_status = WIFI_STATUS_CONNECTED; // Connected but no internet
                    }
                }
                current_state = COMMS_STATE_MQTT_CONNECTING;
                break;

            case COMMS_STATE_MQTT_CONNECTING:
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
