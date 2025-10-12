#include "../cli_command.h"
#include "../../core/vehicle_db_handler.h"
#include <Arduino.h>

// --- add_model ---
void add_model_execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        Serial.println("Usage: add_model <model_name> <type>");
        Serial.println("  type: diesel or electric");
        return;
    }
    VehicleModel new_model;
    new_model.model_name = args[1].c_str();
    new_model.type = args[2].c_str();

    if (vehicle_db_add_model(new_model)) {
        Serial.println("Model added successfully. Run 'save_db' to make it persistent.");
    } else {
        Serial.println("Error: Model with that name already exists.");
    }
}
const CliCommand add_model_command = {
    "add_model",
    "Add a new vehicle model to the database",
    add_model_execute
};

// --- add_ecu ---
void add_ecu_execute(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        Serial.println("Usage: add_ecu <model_name> <ecu_name> <source_address>");
        return;
    }
    VehicleEcu new_ecu;
    new_ecu.name = args[2].c_str();
    new_ecu.source_address = std::stoi(args[3]);

    if (vehicle_db_add_ecu_to_model(args[1], new_ecu)) {
        Serial.println("ECU added successfully. Run 'save_db' to make it persistent.");
    } else {
        Serial.println("Error: Model not found or ECU already exists in model.");
    }
}
const CliCommand add_ecu_command = {
    "add_ecu",
    "Add an ECU to a vehicle model",
    add_ecu_execute
};

// --- add_pgn ---
void add_pgn_execute(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        Serial.println("Usage: add_pgn <model_name> <ecu_name> <pgn>");
        return;
    }
    uint32_t pgn = std::stoul(args[3]);

    if (vehicle_db_add_pgn_to_ecu(args[1], args[2], pgn)) {
        Serial.println("PGN added successfully. Run 'save_db' to make it persistent.");
    } else {
        Serial.println("Error: Model or ECU not found.");
    }
}
const CliCommand add_pgn_command = {
    "add_pgn",
    "Add a PGN of interest to an ECU",
    add_pgn_execute
};
