/**
 * J1939 Diagnostic Tool - FMI Database Handler Implementation
 * 
 * Versão: 3.17.0
 */

#include "fmi_db_handler.h"
#include "error_handler.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <map>

// In-memory representation of the FMI database
static std::map<uint8_t, String> fmi_database;

bool fmi_db_init() {
    File db_file = LittleFS.open("/fmi_descriptions.json", "r");
    if (!db_file) {
        error_report(ErrorLevel::WARN, "FMI_DB", "fmi_descriptions.json not found.");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, db_file);
    db_file.close();

    if (error) {
        error_report(ErrorLevel::ERROR, "FMI_DB", "Failed to parse fmi_descriptions.json.");
        return false;
    }

    fmi_database.clear();
    JsonObject descriptions = doc["fmi_descriptions"].as<JsonObject>();
    for (JsonPair pair : descriptions) {
        uint8_t fmi = atoi(pair.key().c_str());
        fmi_database[fmi] = pair.value().as<String>();
    }

    error_report(ErrorLevel::INFO, "FMI_DB", "FMI description database loaded.");
    return true;
}

const char* fmi_get_description(uint8_t fmi) {
    auto it = fmi_database.find(fmi);
    if (it != fmi_database.end()) {
        return it->second.c_str();
    } else {
        return "Unknown FMI";
    }
}
