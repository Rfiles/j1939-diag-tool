/**
 * J1939 Diagnostic Tool - Comms Handler Task Implementation
 * 
 * Versão: 2.6.0
 */

#include "comms_handler.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../core/config.h"
#include "../j1939/j1939_handler.h"
#include "gps/gps_handler.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "../core/error_handler.h"
#include "../core/time_handler.h"
#include "../core/license_handler.h"

// --- RTOS Queues ---
QueueHandle_t telemetry_queue;

// --- WiFi and MQTT Clients ---
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// --- Last known data ---
GpsData last_gps_data;
std::map<uint32_t, can_frame> last_j1939_data;

void setup_ota() {
    ArduinoOTA.setHostname(config.ota.hostname.c_str());
    ArduinoOTA.setPassword(config.ota.password.c_str());

    ArduinoOTA.onStart([]() { error_report(ErrorLevel::INFO, "OTA", "Update started"); });
    ArduinoOTA.onEnd([]() { error_report(ErrorLevel::INFO, "OTA", "Update finished"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        char message[30];
        sprintf(message, "Progress: %u%%", (progress / (total / 100)));
        error_report(ErrorLevel::INFO, "OTA", message);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        char message[30];
        sprintf(message, "Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) strcat(message, "Auth Failed");
        else if (error == OTA_BEGIN_ERROR) strcat(message, "Begin Failed");
        else if (error == OTA_CONNECT_ERROR) strcat(message, "Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) strcat(message, "Receive Failed");
        else if (error == OTA_END_ERROR) strcat(message, "End Failed");
        error_report(ErrorLevel::CRITICAL, "OTA", message);
    });

    ArduinoOTA.begin();
    error_report(ErrorLevel::INFO, "OTA", "OTA service initialized");
}

void setup_wifi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(config.wifi.ssid);
    WiFi.begin(config.wifi.ssid.c_str(), config.wifi.password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    setup_ota();
    time_handler_init();
}

void reconnect_mqtt() {
    while (!mqtt_client.connected()) {
        error_report(ErrorLevel::WARN, "MQTT", "Attempting connection...");
        if (mqtt_client.connect(config.ota.hostname.c_str())) {
            error_report(ErrorLevel::INFO, "MQTT", "Connected to broker.");
        } else {
            char message[40];
            sprintf(message, "Connection failed, rc=%d. Retrying in 5s", mqtt_client.state());
            error_report(ErrorLevel::ERROR, "MQTT", message);
            delay(5000);
        }
    }
}

void comms_task_fn(void* pv) {
    setup_wifi();

    if (config.features.mqtt_enabled) {
        mqtt_client.setServer(config.mqtt.broker.c_str(), config.mqtt.port);
    }

    can_frame j1939_frame;
    GpsData gps_data;
    char* telemetry_payload = NULL;
    unsigned long last_publish_time = 0;

    for (;;) {
        ArduinoOTA.handle();

        if (config.features.mqtt_enabled) {
            if (!mqtt_client.connected()) {
                reconnect_mqtt();
            }
            mqtt_client.loop();

            if (license_is_feature_active(LicensedFeature::TELEMETRY)) {
                // 1. Check for high-priority telemetry messages
                if (xQueueReceive(telemetry_queue, &telemetry_payload, 0) == pdPASS) {
                    if (telemetry_payload != NULL) {
                        mqtt_client.publish(config.mqtt.topic.c_str(), telemetry_payload);
                        free(telemetry_payload);
                    }
                }
            }
        }

        if (j1939_rx_queue != NULL && xQueueReceive(j1939_rx_queue, &j1939_frame, 0) == pdPASS) {
            uint32_t pgn = (j1939_frame.can_id >> 8) & 0x1FFFF;
            last_j1939_data[pgn] = j1939_frame;
        }

        if (gps_data_queue != NULL && xQueueReceive(gps_data_queue, &gps_data, 0) == pdPASS) {
            if (gps_data.is_valid) {
                last_gps_data = gps_data;
            }
        }

        if (config.features.mqtt_enabled && license_is_feature_active(LicensedFeature::TELEMETRY) && (millis() - last_publish_time > 5000)) {
            last_publish_time = millis();

            JsonDocument doc;
            doc["timestamp"] = time_handler_get_timestamp();
            doc["type"] = "periodic_data";

            if (last_gps_data.is_valid) {
                JsonObject gps = doc.createNestedObject("gps");
                gps["lat"] = last_gps_data.latitude;
                gps["lon"] = last_gps_data.longitude;
                gps["spd"] = last_gps_data.speed_kmh;
            }

            JsonObject j1939 = doc.createNestedObject("j1939");
            for (auto const& [pgn, frame] : last_j1939_data) {
                JsonArray data_array = j1939.createNestedArray(String(pgn));
                for(int i=0; i<frame.can_dlc; i++) {
                    data_array.add(frame.data[i]);
                }
            }

            String output;
            serializeJson(doc, output);
            mqtt_client.publish(config.mqtt.topic.c_str(), output.c_str());

            // Consume a license use
            license_consume_use();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void comms_handler_init() {
    telemetry_queue = xQueueCreate(10, sizeof(char*));
    xTaskCreate(comms_task_fn, "Comms Task", 10240, NULL, 3, NULL);
}

bool comms_publish_telemetry(const char* payload) {
    // Do not queue telemetry if the license is not active
    if (payload == NULL || !license_is_feature_active(LicensedFeature::TELEMETRY)) {
        return false;
    }
    char* payload_copy = strdup(payload);
    if (payload_copy == NULL) {
        return false;
    }
    if (xQueueSend(telemetry_queue, &payload_copy, 0) != pdPASS) {
        free(payload_copy);
        return false;
    }
    return true;
}