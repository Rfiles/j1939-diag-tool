#include "../cli_command.h"
#include "../../core/config.h"
#include <Arduino.h>

// Helper to parse boolean values
bool string_to_bool(const std::string& s) {
    return s == "true" || s == "1";
}

// Execute function for the 'set' command
void set_execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        Serial.println("Usage: set <key> <value>");
        return;
    }

    const std::string& key = args[1];
    const std::string& value = args[2];

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
        Serial.println("Unknown setting key.");
        return;
    }
    Serial.printf("Set %s to: %s\n", key.c_str(), value.c_str());
    Serial.println("NOTE: Run 'save' to make the change persistent.");
}

// Command definition
const CliCommand set_command = {
    "set",
    "Set a configuration value (e.g., set wifi_ssid MyNetwork)",
    set_execute
};
