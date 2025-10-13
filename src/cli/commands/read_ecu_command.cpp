/**
 * J1939 Diagnostic Tool - "read_ecu" Command
 * 
 * Versão: 3.8.0
 */

#include "../cli_command.h"
#include "../core/vehicle_db_handler.h"
#include "../ui/screens/live_data_screen.h"
#include "../core/shared_resources.h"
#include "../cli_output.h"
#include <Arduino.h>

// Execute function for the 'read_ecu' command
void read_ecu_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: read_ecu <ecu_name>\n");
        return;
    }

    const VehicleModel* model = vehicle_db_get_active_model();
    if (model == nullptr) {
        cli_printf("Error: No vehicle model selected. Use 'select_model' first.\n");
        return;
    }

    const std::string ecu_name_to_find = args[1];
    bool ecu_found = false;

    if (xSemaphoreTake(vehicle_db_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock vehicle database.\n");
        return;
    }

    for (const auto& ecu : model->ecus) {
        if (ecu.name == ecu_name_to_find.c_str()) {
            ecu_found = true;
            cli_printf("Switching to live data screen for ECU: %s\n", ecu.name.c_str());
            ui_show_ecu_data(&ecu);
            break;
        }
    }

    xSemaphoreGive(vehicle_db_mutex);

    if (!ecu_found) {
        cli_printf("Error: ECU not found in the active vehicle model.\n");
    }
}

// Command definition
const CliCommand read_ecu_command = {
    "read_ecu",
    "Request all interesting PGNs from a specific ECU of the active model",
    read_ecu_execute
};
