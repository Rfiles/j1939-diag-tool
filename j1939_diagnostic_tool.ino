/**
 * J1939 Diagnostic Tool - FreeRTOS Entry Point
 * 
 * Versão: 2.2.0
 */

#include "src/core/filesystem_handler.h"
#include "src/j1939/j1939_handler.h"
#include "src/ui/ui_handler.h"
#include "src/cli/cli_handler.h"
#include "src/comms/gps/gps_handler.h"
#include "src/comms/comms_handler.h"
#include "src/core/error_handler.h"
#include "src/core/settings_handler.h"
#include "src/core/license_handler.h"
#include "src/j1939/tp_handler.h"
#include "src/j1939/pdu_processor.h"
#include "src/core/vehicle_db_handler.h"
#include "src/core/spn_db_handler.h"
#include "src/core/fmi_db_handler.h"
#include "src/core/shared_resources.h"
#include "src/j1939/j1939_pgn_definitions.h"

void setup() {
    // Start serial for debugging
    Serial.begin(115200);
    Serial.println("\n--- J1939 Diagnostic Tool v3.18.0 ---");

    // Initialize shared resources (mutexes)
    shared_resources_init();

    // Initialize J1939 definitions
    init_spn_to_pgn_map();

    // Load configuration from LittleFS (defaults)
    if (!filesystem_init()) {
        // We can't use the full error_report here as comms aren't up yet.
        Serial.println("CRITICAL: Filesystem or config error. Halting.");
        while(1);
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

    // Initialize J1939 protocol handlers
    tp_handler_init();
    pdu_processor_init();

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

    // Initialize the core handlers
    j1939_handler_init();
    ui_handler_init();
    cli_handler_init();

    error_report(ErrorLevel::INFO, "Main", "All tasks created. Scheduler is running.");
}

void loop() {
    // This loop is now idle. All work is done in the FreeRTOS tasks.
    vTaskDelay(portMAX_DELAY);
}
