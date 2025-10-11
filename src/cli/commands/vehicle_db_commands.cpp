#include "../cli_command.h"
#include "../../core/vehicle_db_handler.h"
#include <Arduino.h>

// --- list_models ---
void list_models_execute(const std::vector<std::string>& args) {
    const auto& models = vehicle_db_get_all_models();
    if (models.empty()) {
        Serial.println("No vehicle models found in the database.");
        return;
    }
    Serial.println("Available Vehicle Models:");
    for (const auto& model : models) {
        Serial.printf("  - %s (Type: %s)\n", model.model_name.c_str(), model.type.c_str());
    }
}
const CliCommand list_models_command = {
    "list_models",
    "List all vehicle models in the database",
    list_models_execute
};

// --- show_model ---
void show_model_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Serial.println("Usage: show_model <model_name>");
        return;
    }
    const auto& models = vehicle_db_get_all_models();
    for (const auto& model : models) {
        if (model.model_name == args[1].c_str()) {
            Serial.printf("Model: %s\n", model.model_name.c_str());
            Serial.printf("Type: %s\n", model.type.c_str());
            Serial.println("ECUs:");
            for (const auto& ecu : model.ecus) {
                Serial.printf("  - %s (SA: %d)\n", ecu.name.c_str(), ecu.source_address);
                Serial.print("    PGNs: ");
                for (uint32_t pgn : ecu.pgns_of_interest) {
                    Serial.printf("%lu, ", pgn);
                }
                Serial.println();
            }
            return;
        }
    }
    Serial.println("Model not found.");
}
const CliCommand show_model_command = {
    "show_model",
    "Show details of a specific vehicle model",
    show_model_execute
};

// --- save_db ---
void save_db_execute(const std::vector<std::string>& args) {
    vehicle_db_save();
}
const CliCommand save_db_command = {
    "save_db",
    "Save any changes to the vehicle database file",
    save_db_execute
};
