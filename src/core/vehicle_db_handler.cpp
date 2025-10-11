/**
 * J1939 Diagnostic Tool - Vehicle Database Handler Implementation
 * 
 * Versão: 3.3.0
 */

#include "vehicle_db_handler.h"
#include "error_handler.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

// In-memory representation of the vehicle database
static std::vector<VehicleModel> vehicle_database;

// The capacity of the JsonDocument. This needs to be large enough for the entire file.
// Adjust this value if you add more models or data.
const int JSON_CAPACITY = 4096;

bool vehicle_db_init() {
    File db_file = LittleFS.open("/vehicles.json", "r");
    if (!db_file) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "vehicles.json not found.");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, db_file);
    db_file.close();

    if (error) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to parse vehicles.json.");
        return false;
    }

    vehicle_database.clear();
    JsonArray vehicles = doc["vehicles"].as<JsonArray>();
    for (JsonObject vehicle_json : vehicles) {
        VehicleModel model;
        model.model_name = vehicle_json["model_name"].as<String>();
        model.type = vehicle_json["type"].as<String>();

        JsonArray ecus = vehicle_json["ecus"].as<JsonArray>();
        for (JsonObject ecu_json : ecus) {
            VehicleEcu ecu;
            ecu.name = ecu_json["name"].as<String>();
            ecu.source_address = ecu_json["sa"].as<uint8_t>();
            JsonArray pgns = ecu_json["pgns_of_interest"].as<JsonArray>();
            for (JsonVariant pgn : pgns) {
                ecu.pgns_of_interest.push_back(pgn.as<uint32_t>());
            }
            model.ecus.push_back(ecu);
        }
        vehicle_database.push_back(model);
    }

    error_report(ErrorLevel::INFO, "VehicleDB", "Vehicle database loaded successfully.");
    return true;
}

bool vehicle_db_save() {
    JsonDocument doc;
    JsonArray vehicles = doc.createNestedArray("vehicles");

    for (const auto& model : vehicle_database) {
        JsonObject vehicle_json = vehicles.createNestedObject();
        vehicle_json["model_name"] = model.model_name;
        vehicle_json["type"] = model.type;
        JsonArray ecus_json = vehicle_json.createNestedArray("ecus");
        for (const auto& ecu : model.ecus) {
            JsonObject ecu_json = ecus_json.createNestedObject();
            ecu_json["name"] = ecu.name;
            ecu_json["sa"] = ecu.source_address;
            JsonArray pgns_json = ecu_json.createNestedArray("pgns_of_interest");
            for (uint32_t pgn : ecu.pgns_of_interest) {
                pgns_json.add(pgn);
            }
        }
    }

    File db_file = LittleFS.open("/vehicles.json", "w");
    if (!db_file) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to open vehicles.json for writing.");
        return false;
    }

    if (serializeJson(doc, db_file) == 0) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to write to vehicles.json.");
        db_file.close();
        return false;
    }

    db_file.close();
    error_report(ErrorLevel::INFO, "VehicleDB", "Vehicle database saved successfully.");
    return true;
}

const std::vector<VehicleModel>& vehicle_db_get_all_models() {
    return vehicle_database;
}
