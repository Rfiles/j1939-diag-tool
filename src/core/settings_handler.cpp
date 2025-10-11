/**
 * J1939 Diagnostic Tool - NVS Settings Handler Implementation
 * 
 * Versão: 2.4.0
 */

#include "settings_handler.h"
#include "config.h"
#include "error_handler.h"
#include <Preferences.h>

// NVS namespace
static const char* NVS_NAMESPACE = "j1939_diag";

// Create an instance of the Preferences library
Preferences preferences;

void settings_init() {
    // Start preferences with our namespace. The second argument is false for read-write mode.
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        error_report(ErrorLevel::ERROR, "Settings", "Failed to initialize NVS");
    }
}

void settings_load() {
    error_report(ErrorLevel::INFO, "Settings", "Loading settings from NVS...");

    // Read from NVS and update the global config struct.
    // If the key doesn't exist, getString/getInt will return the default value (the second argument).
    // We use the existing value from the config struct (loaded from config.json) as the default.
    config.wifi.ssid = preferences.getString("wifi_ssid", config.wifi.ssid);
    config.wifi.password = preferences.getString("wifi_pass", config.wifi.password);

    config.mqtt.broker = preferences.getString("mqtt_broker", config.mqtt.broker);
    config.mqtt.port = preferences.getInt("mqtt_port", config.mqtt.port);
    config.mqtt.topic = preferences.getString("mqtt_topic", config.mqtt.topic);

    config.ota.hostname = preferences.getString("ota_host", config.ota.hostname);
    config.ota.password = preferences.getString("ota_pass", config.ota.password);

    config.features.wifi_enabled = preferences.getBool("f_wifi", config.features.wifi_enabled);
    config.features.mqtt_enabled = preferences.getBool("f_mqtt", config.features.mqtt_enabled);
    config.features.gps_enabled = preferences.getBool("f_gps", config.features.gps_enabled);
}

void settings_save() {
    error_report(ErrorLevel::INFO, "Settings", "Saving settings to NVS...");

    // Store the current config values into NVS
    preferences.putString("wifi_ssid", config.wifi.ssid);
    preferences.putString("wifi_pass", config.wifi.password);

    preferences.putString("mqtt_broker", config.mqtt.broker);
    preferences.putInt("mqtt_port", config.mqtt.port);
    preferences.putString("mqtt_topic", config.mqtt.topic);

    preferences.putString("ota_host", config.ota.hostname);
    preferences.putString("ota_pass", config.ota.password);

    preferences.putBool("f_wifi", config.features.wifi_enabled);
    preferences.putBool("f_mqtt", config.features.mqtt_enabled);
    preferences.putBool("f_gps", config.features.gps_enabled);

    error_report(ErrorLevel::INFO, "Settings", "Settings saved successfully.");
}

void settings_check_version_update() {
    String last_version = preferences.getString("fw_version", "");
    if (last_version != FIRMWARE_VERSION) {
        error_report(ErrorLevel::INFO, "Settings", "New firmware version detected. Resetting one-time flags.");
        // Clear the flag that disables the dev command
        preferences.putBool("dev_cmd_used", false);
        // Update the stored version
        preferences.putString("fw_version", FIRMWARE_VERSION);
    }
}

bool settings_is_dev_cmd_disabled() {
    return preferences.getBool("dev_cmd_used", false);
}

void settings_disable_dev_cmd() {
    preferences.putBool("dev_cmd_used", true);
}
