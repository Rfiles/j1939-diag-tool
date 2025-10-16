#include "../cli_command.h"
#include "../../core/vehicle_db_handler.h"
#include "../../core/shared_resources.h"
#include "../cli_output.h"
#include <Arduino.h>

// --- list_models ---
void list_models_execute(const std::vector<std::string>& args) {
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock vehicle database.\n");
        return;
    }
    const auto& models = vehicle_db_get_all_models();
    if (models.empty()) {
        cli_printf("No vehicle models found in the database.\n");
    } else {
        cli_printf("Available Vehicle Models:\n");
        for (const auto& model : models) {
            cli_printf("  - %s (Type: %s)\n", model.model_name.c_str(), model.type.c_str());
        }
    }
    xSemaphoreGive(vehicle_db_mutex);
}
extern const CliCommand list_models_command = {
    "list_models",
    "List all vehicle models in the database",
    list_models_execute
};

// --- show_model ---
void show_model_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: show_model <model_name>\n");
        return;
    }
    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock vehicle database.\n");
        return;
    }
    const auto& models = vehicle_db_get_all_models();
    bool found = false;
    for (const auto& model : models) {
        if (model.model_name == args[1].c_str()) {
            cli_printf("Model: %s\n", model.model_name.c_str());
            cli_printf("Type: %s\n", model.type.c_str());
            cli_printf("ECUs:\n");
            for (const auto& ecu : model.ecus) {
                cli_printf("  - %s (SA: %d)\n", ecu.name.c_str(), ecu.source_address);
                cli_printf("    PGNs: ");
                for (uint32_t pgn : ecu.pgns_of_interest) {
                    cli_printf("%lu, ", pgn);
                }
                cli_printf("\n");
            }
            found = true;
            break;
        }
    }
    if (!found) {
        cli_printf("Model not found.\n");
    }
    xSemaphoreGive(vehicle_db_mutex);
}
extern const CliCommand show_model_command = {
    "show_model",
    "Show details of a specific vehicle model",
    show_model_execute
};

// --- save_db ---
void save_db_execute(const std::vector<std::string>& args) {
    vehicle_db_save();
}
extern const CliCommand save_db_command = {
    "save_db",
    "Save any changes to the vehicle database file",
    save_db_execute
};
