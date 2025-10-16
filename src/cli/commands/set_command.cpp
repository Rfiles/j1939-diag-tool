#include "../cli_command.h"
#include "../../core/config.h"
#include "../../core/shared_resources.h"
#include "../cli_output.h"
#include <Arduino.h>
#include <stdexcept>

// Helper to parse boolean values
bool string_to_bool(const std::string& s) {
    return s == "true" || s == "1";
}

// Execute function for the 'set' command
void set_execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        cli_printf("Usage: set <key> <value>\n");
        return;
    }

    const std::string& key = args[1];
    const std::string& value = args[2];

    if (xSemaphoreTake(config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock config to apply settings.\n");
        return;
    }

    try {
        if (key == "wifi_ssid") config.wifi.ssid = value.c_str();
        else if (key == "wifi_pass") config.wifi.password = value.c_str();
        else if (key == "mqtt_broker") config.mqtt.broker = value.c_str();
        else if (key == "mqtt_port") config.mqtt.port = std::stoi(value);
        else if (key == "mqtt_topic") config.mqtt.topic = value.c_str();
        else if (key == "ota_host") config.ota.hostname = value.c_str();
        else if (key == "ota_pass") config.ota.password = value.c_str();
        else if (key == "f_wifi") config.features.wifi_enabled = string_to_bool(value);
        else if (key == "f_mqtt") config.features.mqtt_enabled = string_to_bool(value);
        else if (key == "f_gps") config.features.gps_enabled = string_to_bool(value);
        else {
            cli_printf("Unknown setting key.\n");
            xSemaphoreGive(config_mutex);
            return;
        }
    } catch (const std::invalid_argument& ia) {
        cli_printf("ERROR: Invalid number format for port.\n");
        xSemaphoreGive(config_mutex);
        return;
    }

    xSemaphoreGive(config_mutex);
    cli_printf("Set %s to: %s\n", key.c_str(), value.c_str());
    cli_printf("NOTE: Run 'save' to make the change persistent.\n");
}

// Command definition
extern const CliCommand set_command = {
    "set",
    "Set a configuration value (e.g., set wifi_ssid MyNetwork)",
    set_execute
};
