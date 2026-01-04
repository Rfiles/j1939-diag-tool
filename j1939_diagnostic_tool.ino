/**
 * J1939 Diagnostic Tool - FreeRTOS Entry Point
 * 
 * Versão: 4.2.1
 */

#include "modules/settings/src/config.h"
#include "modules/core/src/filesystem_handler.h"
#include "modules/j1939/src/j1939_service.h"
#include "modules/ui/src/ui_manager.h"
#include "modules/cli/src/cli_handler.h"
#include "modules/gps/src/gps_handler.h"
#include "modules/wifi/src/comms_handler.h"
#include "modules/core/src/error_handler.h"
#include "modules/settings/src/settings_handler.h"
#include "modules/license/src/license_handler.h"
#include "modules/time/src/time_handler.h"
#include "modules/j1939/src/vehicle_db_handler.h"
#include "modules/j1939/src/spn_db_handler.h"
#include "modules/j1939/src/fmi_db_handler.h"
#include "modules/core/src/shared_resources.h"
#include "modules/j1939/src/j1939_pgn_definitions.h"

// Force include of CLI command definitions
#include "modules/cli/src/commands/cli_commands.cpp"
#include "modules/cli/src/commands/db_edit_commands.cpp"
#include "modules/license/src/dev_license_generator.cpp"
#include "modules/cli/src/commands/dm_clear_commands.cpp"
#include "modules/cli/src/commands/dm_commands.cpp"
#include "modules/cli/src/commands/file_transfer_commands.cpp"
#include "modules/cli/src/commands/get_config_command.cpp"
#include "modules/cli/src/commands/help_command.cpp"
#include "modules/cli/src/commands/license_commands.cpp"
#include "modules/cli/src/commands/ls_command.cpp"
#include "modules/cli/src/commands/read_ecu_command.cpp"
#include "modules/cli/src/commands/request_command.cpp"
#include "modules/cli/src/commands/request_vin_command.cpp"
#include "modules/cli/src/commands/save_command.cpp"
#include "modules/cli/src/commands/select_model_command.cpp"
#include "modules/cli/src/commands/set_command.cpp"
#include "modules/cli/src/commands/stream_command.cpp"
#include "modules/cli/src/commands/vehicle_db_commands.cpp"

#include "modules/ui/src/screens/error_screen.h"
#include "modules/ui/src/screens/main_menu.h"


void setup() {
    // Start serial for debugging
    Serial.begin(115200);
    Serial.println("\n--- J1939 Diagnostic Tool v4.2.1 ---");

    // Initialize shared resources (mutexes)
    shared_resources_init();

    // Initialize the UI as early as possible to show the splash screen
    UIManager::getInstance().init();

    // Initialize J1939 definitions
    init_spn_to_pgn_map();

    // Load configuration from LittleFS
    if (!filesystem_init()) {
        error_report(ErrorLevel::CRITICAL, "Main", "Filesystem error, attempting to recover.");
        
        // Attempt to format and create a default filesystem
        if (filesystem_format_and_create_defaults()) {
            // Show a message to the user and wait for confirmation
            UIManager::getInstance().push_screen(std::make_shared<ErrorScreen>("FS Error", "FS created. Press SELECT"));
            // The UI task will handle the rest. We just need to wait here.
            // The ErrorScreen will pop itself when the user presses SELECT.
        } else {
            // If we can't even create a new filesystem, we're stuck.
            // A more robust solution might try to display a persistent error on the screen.
            Serial.println("CRITICAL: Filesystem recovery failed. Halting.");
            // You could draw a critical error message directly to the screen here
            // and then halt.
            while(1);
        }
    }

    // Load databases
    if (!vehicle_db_init()) {
        error_report(ErrorLevel::WARN, "Main", "Could not load vehicle database.");
    }
    if (!spn_db_init()) {
        error_report(ErrorLevel::WARN, "Main", "Could not load SPN database.");
    }
    if (!fmi_db_init()) {
        error_report(ErrorLevel::WARN, "Main", "Could not load FMI database.");
    }

    // Initialize NVS and load persistent settings, overriding defaults
    settings_init();
    settings_load();

    // Check for firmware updates and handle accordingly
    settings_check_version_update();

    // Initialize the licensing system
    license_handler_init();

    // Initialize time handler before network services
    time_handler_init(config.features.wifi_enabled ? TIME_SOURCE_NTP : TIME_SOURCE_GPS);

    // Initialize J1939 protocol handlers
    J1939Service::getInstance().init();

    // Initialize handlers based on feature flags
    if (config.features.wifi_enabled) {
        comms_handler_init();
    } else {
        error_report(ErrorLevel::INFO, "Main", "WiFi/Comms handler disabled by config.");
    }

    if (config.features.gps_enabled) {
        gps_handler_init();
    } else {
        error_report(ErrorLevel::INFO, "Main", "GPS handler disabled by config.");
    }

    // Synchronize time after handlers are initialized
    time_sync();

    // Initialize the core handlers
    cli_handler_init();

    error_report(ErrorLevel::INFO, "Main", "All tasks created. Scheduler is running.");

    // After splash screen, normal operation proceeds to the main menu.
    // The splash screen itself transitions to the main menu after a delay.
}

void loop() {
    // This loop is now idle. All work is done in the FreeRTOS tasks.
    vTaskDelay(portMAX_DELAY);
}