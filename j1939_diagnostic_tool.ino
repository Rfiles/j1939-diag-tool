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

void setup() {
    // Start serial for debugging
    Serial.begin(115200);
    Serial.println("\n--- J1939 Diagnostic Tool v2.6.0 ---");

    // Load configuration from LittleFS (defaults)
    if (!filesystem_init()) {
        // We can't use the full error_report here as comms aren't up yet.
        Serial.println("CRITICAL: Filesystem or config error. Halting.");
        while(1);
    }

    // Initialize NVS and load persistent settings, overriding defaults
    settings_init();
    settings_load();

    // Check for firmware updates and handle accordingly
    settings_check_version_update();

    // Initialize the licensing system
    license_handler_init();

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
