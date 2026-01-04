/**
 * J1939 Diagnostic Tool - FreeRTOS Entry Point
 * 
 * Versão: 4.2.1
 */

#include "src/core/config.h"
#include "src/core/filesystem_handler.h"
#include "src/j1939/j1939_service.h"
#include "src/ui/ui_manager.h"
#include "src/cli/cli_handler.h"
#include "src/driver/gps/gps_handler.h"
#include "src/comms/comms_handler.h"
#include "src/core/error_handler.h"
#include "src/core/settings_handler.h"
#include "src/core/license_handler.h"
#include "src/core/time_handler.h"
#include "src/core/vehicle_db_handler.h"
#include "src/core/spn_db_handler.h"
#include "src/core/fmi_db_handler.h"
#include "src/core/shared_resources.h"
#include "src/j1939/j1939_pgn_definitions.h"

// Force include of CLI command definitions
#include "src/cli/commands/cli_commands.cpp"
#include "src/cli/commands/db_edit_commands.cpp"
#include "src/cli/commands/dev_license_generator.cpp"
#include "src/cli/commands/dm_clear_commands.cpp"
#include "src/cli/commands/dm_commands.cpp"
#include "src/cli/commands/file_transfer_commands.cpp"
#include "src/cli/commands/get_config_command.cpp"
#include "src/cli/commands/help_command.cpp"
#include "src/cli/commands/license_commands.cpp"
#include "src/cli/commands/ls_command.cpp"
#include "src/cli/commands/read_ecu_command.cpp"
#include "src/cli/commands/request_command.cpp"
#include "src/cli/commands/request_vin_command.cpp"
#include "src/cli/commands/save_command.cpp"
#include "src/cli/commands/select_model_command.cpp"
#include "src/cli/commands/set_command.cpp"
#include "src/cli/commands/stream_command.cpp"
#include "src/cli/commands/vehicle_db_commands.cpp"

#include "src/ui/screens/error_screen.h"
#include "src/ui/screens/main_menu.h"


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