/**
 * J1939 Diagnostic Tool - SPN Database Handler Implementation
 * 
 * Versão: 3.14.0
 */

#include "spn_db_handler.h"
#include "error_handler.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <map>

// In-memory representation of the SPN database
static std::map<uint32_t, String> spn_database;

bool spn_db_init() {
    File db_file = LittleFS.open("/spn_descriptions.json", "r");
    if (!db_file) {
        error_report(ErrorLevel::WARN, "SPN_DB", "spn_descriptions.json not found.");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, db_file);
    db_file.close();

    if (error) {
        error_report(ErrorLevel::ERROR, "SPN_DB", "Failed to parse spn_descriptions.json.");
        return false;
    }

    spn_database.clear();
    JsonObject descriptions = doc["spn_descriptions"].as<JsonObject>();
    for (JsonPair pair : descriptions) {
        uint32_t spn = atoi(pair.key().c_str());
        spn_database[spn] = pair.value().as<String>();
    }

    error_report(ErrorLevel::INFO, "SPN_DB", "SPN description database loaded.");
    return true;
}

const char* spn_get_description(uint32_t spn) {
    auto it = spn_database.find(spn);
    if (it != spn_database.end()) {
        return it->second.c_str();
    } else {
        return "Unknown SPN";
    }
}
