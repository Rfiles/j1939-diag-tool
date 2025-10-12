/**
 * J1939 Diagnostic Tool - Comms Handler with HTTP File Server
 * 
 * Versão: 3.13.0
 */

#include "comms_handler.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <LittleFS.h>

#include "../core/config.h"
#include "../core/error_handler.h"
// ... other includes

// --- Module State & Clients ---
static CommsState current_state = COMMS_STATE_STARTING;
// ... other state variables
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

// --- State Machine Task ---
void comms_task_fn(void* pv) {
    // ...
    for (;;) {
        switch (current_state) {
            // ... other states
            case COMMS_STATE_WIFI_CONNECTING:
                if (WiFi.status() == WL_CONNECTED) {
                    // ... other setup
                    setup_http_server();
                    current_state = COMMS_STATE_NTP_SYNCING;
                } // ...
                break;

            case COMMS_STATE_OPERATIONAL:
                // ... other operational logic
                http_server.handleClient();
                break;

            case COMMS_STATE_WIFI_DISCONNECTED:
                http_server.stop();
                // ... other disconnection logic
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ... (init and other functions) ...
