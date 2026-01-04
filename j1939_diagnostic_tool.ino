/**
 * J1939 Diagnostic Tool - FreeRTOS Entry Point
 * 
 * Versão: 4.3.0
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
#include "modules/ota/src/ota_handler.h"
#include "modules/hardware/src/hardware_handler.h"

// Force include of CLI command definitions
#include "modules/cli/src/commands/cli_commands_include.h"

#include "modules/ui/src/screens/error_screen.h"
#include "modules/ui/src/screens/main_menu.h"

void init_core_services() {
    // Start serial for debugging
    Serial.begin(115200);
    Serial.println("\n--- J1939 Diagnostic Tool v4.3.0 ---");

    // Initialize shared resources (mutexes)
    shared_resources_init();

    // Initialize the UI as early as possible to show the splash screen
    UIManager::getInstance().init();

    // Initialize J1939 definitions
    init_spn_to_pgn_map();
}

void init_databases() {
    // Load configuration from LittleFS
    if (!filesystem_init()) {
        error_report(ErrorLevel::CRITICAL, "Main", "Filesystem error, attempting to recover.");
        
        // Attempt to format and create a default filesystem
        if (filesystem_format_and_create_defaults()) {
            // Show a message to the user and wait for confirmation
            UIManager::getInstance().push_screen(std::make_shared<ErrorScreen>("FS Error", "FS created. Press SELECT"));
        } else {
            Serial.println("CRITICAL: Filesystem recovery failed. Halting.");
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
}

void init_comms() {
    // Initialize handlers based on feature flags
    if (config.features.wifi_enabled) {
        comms_handler_init();
        ota_handler_init();
    } else {
        error_report(ErrorLevel::INFO, "Main", "WiFi/Comms handler disabled by config.");
    }

    if (config.features.gps_enabled) {
        gps_handler_init();
    } else {
        error_report(ErrorLevel::INFO, "Main", "GPS handler disabled by config.");
    }
}

void setup() {
    init_core_services();
    init_databases();

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

    // Initialize communication handlers
    init_comms();

    // Synchronize time after handlers are initialized
    time_sync();

    // Initialize the core handlers
    cli_handler_init();
    hardware_handler_init();

    error_report(ErrorLevel::INFO, "Main", "All tasks created. Scheduler is running.");
}

void loop() {
    // This loop is now idle. All work is done in the FreeRTOS tasks.
    vTaskDelay(portMAX_DELAY);
}
