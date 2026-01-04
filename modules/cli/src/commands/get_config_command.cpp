#include "../cli_command.h"
#include "../cli_output.h"
#include "../../settings/src/config.h"
#include "../../core/shared_resources.h"
#include <ArduinoJson.h>

// Execute function for the 'get_config' command
void get_config_execute(const std::vector<std::string>& args) {
    if (xSemaphoreTake(config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock config.\n");
        return;
    }

    JsonDocument doc;
    doc["wifi.ssid"] = config.wifi.ssid;
    doc["wifi.password"] = config.wifi.password;
    doc["mqtt.broker"] = config.mqtt.broker;
    doc["mqtt.port"] = config.mqtt.port;
    doc["mqtt.topic"] = config.mqtt.topic;
    doc["ota.hostname"] = config.ota.hostname;
    doc["ota.password"] = config.ota.password;
    doc["features.wifi_enabled"] = config.features.wifi_enabled;
    doc["features.mqtt_enabled"] = config.features.mqtt_enabled;
    doc["features.gps_enabled"] = config.features.gps_enabled;

    String output;
    serializeJson(doc, output);
    cli_printf("%s\n", output.c_str());

    xSemaphoreGive(config_mutex);
}

// Command definition
extern const CliCommand get_config_command = {
    "get_config",
    "Print the current device configuration in JSON format",
    get_config_execute
};