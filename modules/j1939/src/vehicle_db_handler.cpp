/**
 * J1939 Diagnostic Tool - Vehicle Database Handler Implementation
 * 
 * Versão: 3.3.0
 */

#include "vehicle_db_handler.h"
#include "error_handler.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "shared_resources.h"

// In-memory representation of the vehicle database
static std::vector<VehicleModel> vehicle_database;
// Pointer to the currently selected vehicle model
static const VehicleModel* active_vehicle_model = nullptr;

// The capacity of the JsonDocument. This needs to be large enough for the entire file.
// Adjust this value if you add more models or data.
const int JSON_CAPACITY = 4096;

bool vehicle_db_init() {
    if (xSemaphoreTake(vehicle_db_mutex, portMAX_DELAY) != pdTRUE) return false;

    File db_file = LittleFS.open("/vehicles.json", "r");
    if (!db_file) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "vehicles.json not found.");
        xSemaphoreGive(vehicle_db_mutex);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, db_file);
    db_file.close();

    if (error) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to parse vehicles.json.");
        xSemaphoreGive(vehicle_db_mutex);
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
    xSemaphoreGive(vehicle_db_mutex);
    return true;
}

bool vehicle_db_save() {
    if (xSemaphoreTake(vehicle_db_mutex, portMAX_DELAY) != pdTRUE) return false;

    JsonDocument doc;
    // Build the JSON array for vehicles
    JsonArray vehicles = doc["vehicles"].to<JsonArray>();
    for (const auto& model : vehicle_database) {
        JsonObject vehicle_json = vehicles.add<JsonObject>();
        vehicle_json["model_name"] = model.model_name;
        vehicle_json["type"] = model.type;
        JsonArray ecus_json = vehicle_json["ecus"].to<JsonArray>();
        for (const auto& ecu : model.ecus) {
            JsonObject ecu_json = ecus_json.add<JsonObject>();
            ecu_json["name"] = ecu.name;
            ecu_json["sa"] = ecu.source_address;
            JsonArray pgns_json = ecu_json["pgns_of_interest"].to<JsonArray>();
            for (uint32_t pgn : ecu.pgns_of_interest) {
                pgns_json.add(pgn);
            }
        }
    }

    File db_file = LittleFS.open("/vehicles.json", "w");
    if (!db_file) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to open vehicles.json for writing.");
        xSemaphoreGive(vehicle_db_mutex);
        return false;
    }

    if (serializeJson(doc, db_file) == 0) {
        error_report(ErrorLevel::ERROR, "VehicleDB", "Failed to write to vehicles.json.");
        db_file.close();
        xSemaphoreGive(vehicle_db_mutex);
        return false;
    }

    db_file.close();
    error_report(ErrorLevel::INFO, "VehicleDB", "Vehicle database saved successfully.");
    xSemaphoreGive(vehicle_db_mutex);
    return true;
}

const std::vector<VehicleModel>& vehicle_db_get_all_models() {
    return vehicle_database;
}

bool vehicle_db_set_active_model(const std::string& model_name) {
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) return false;
    bool found = false;
    for (const auto& model : vehicle_database) {
        if (model.model_name == model_name.c_str()) {
            active_vehicle_model = &model;
            found = true;
            break;
        }
    }
    xSemaphoreGive(vehicle_db_mutex);
    return found;
}

const VehicleModel* vehicle_db_get_active_model() {
    // This function doesn't need a mutex because it just returns a pointer.
    // The caller is responsible for locking the mutex if they dereference the pointer.
    return active_vehicle_model;
}

bool vehicle_db_add_model(const VehicleModel& new_model) {
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) return false;
    // Check if model already exists
    for (const auto& model : vehicle_database) {
        if (model.model_name == new_model.model_name) {
            xSemaphoreGive(vehicle_db_mutex);
            return false; // Already exists
        }
    }
    vehicle_database.push_back(new_model);
    xSemaphoreGive(vehicle_db_mutex);
    return true;
}

bool vehicle_db_add_ecu_to_model(const std::string& model_name, const VehicleEcu& new_ecu) {
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) return false;
    bool success = false;
    for (auto& model : vehicle_database) {
        if (model.model_name == model_name.c_str()) {
            // Check if ECU already exists
            bool ecu_exists = false;
            for (const auto& ecu : model.ecus) {
                if (ecu.name == new_ecu.name) {
                    ecu_exists = true;
                    break;
                }
            }
            if (!ecu_exists) {
                model.ecus.push_back(new_ecu);
                success = true;
            }
            break;
        }
    }
    xSemaphoreGive(vehicle_db_mutex);
    return success;
}

bool vehicle_db_add_pgn_to_ecu(const std::string& model_name, const std::string& ecu_name, uint32_t pgn) {
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) return false;
    bool success = false;
    for (auto& model : vehicle_database) {
        if (model.model_name == model_name.c_str()) {
            for (auto& ecu : model.ecus) {
                if (ecu.name == ecu_name.c_str()) {
                    ecu.pgns_of_interest.push_back(pgn);
                    success = true;
                    break;
                }
            }
            break;
        }
    }
    xSemaphoreGive(vehicle_db_mutex);
    return success;
}
