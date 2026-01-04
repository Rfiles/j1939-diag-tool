#include "../cli_command.h"
#include "../../core/vehicle_db_handler.h"
#include <Arduino.h>

// Execute function for the 'select_model' command
void select_model_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Serial.println("Usage: select_model <model_name>");
        return;
    }
    if (vehicle_db_set_active_model(args[1])) {
        Serial.printf("Active vehicle model set to: %s\n", args[1].c_str());
    } else {
        Serial.println("Error: Model not found in database.");
    }
}

// Command definition
extern const CliCommand select_model_command = {
    "select_model",
    "Set the active vehicle model for context-specific commands",
    select_model_execute
};

