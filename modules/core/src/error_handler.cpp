/**
 * J1939 Diagnostic Tool - Centralized Error Handler Implementation
 * 
 * Versão: 3.8.0
 */

#include "error_handler.h"
#include "time_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "../comms/comms_handler.h"
#include "../driver/gps/gps_handler.h"
#include "../cli/cli_output.h"

const char* error_level_to_string(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::INFO: return "INFO";
        case ErrorLevel::WARN: return "WARN";
        case ErrorLevel::ERROR: return "ERROR";
        case ErrorLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void error_report(ErrorLevel level, const char* module, const char* message) {
    String timestamp = time_handler_get_timestamp();

    // 1. Print to the active CLI stream for local debugging
    cli_printf("%s [%s] [%s] %s\n", timestamp.c_str(), error_level_to_string(level), module, message);

    // 2. Create JSON payload for MQTT
    JsonDocument doc;
    doc["type"] = "event_report";
    doc["timestamp"] = timestamp;
    doc["level"] = error_level_to_string(level);
    doc["module"] = module;
    doc["message"] = message;

    // 3. Get GPS data and add to payload
    GpsData gps_data;
    if (gps_get_last_data(&gps_data)) {
        // Use recommended API: obtain a JsonObject reference via to<>()
        JsonObject gps = doc["gps"].to<JsonObject>();
        gps["lat"] = gps_data.latitude;
        gps["lon"] = gps_data.longitude;
    }

    // 4. Serialize and send to comms task
    String output;
    serializeJson(doc, output);
    comms_publish_telemetry(output.c_str());
}
