#include "../cli_command.h"
#include "../cli_output.h"
#include "../../core/config.h"
#include "../../core/shared_resources.h"

// Execute function for the 'get_config' command
void get_config_execute(const std::vector<std::string>& args) {
    if (xSemaphoreTake(config_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        cli_printf("ERROR: Could not lock config.\n");
        return;
    }

    cli_printf("--- Current Configuration ---\n");
    cli_printf("wifi_ssid=%s\n", config.wifi.ssid.c_str());
    cli_printf("mqtt_broker=%s\n", config.mqtt.broker.c_str());
    cli_printf("mqtt_port=%d\n", config.mqtt.port);
    cli_printf("mqtt_topic=%s\n", config.mqtt.topic.c_str());
    cli_printf("ota_host=%s\n", config.ota.hostname.c_str());
    cli_printf("f_wifi=%s\n", config.features.wifi_enabled ? "true" : "false");
    cli_printf("f_mqtt=%s\n", config.features.mqtt_enabled ? "true" : "false");
    cli_printf("f_gps=%s\n", config.features.gps_enabled ? "true" : "false");
    cli_printf("-----------------------------\n");

    xSemaphoreGive(config_mutex);
}

// Command definition
extern const CliCommand get_config_command = {
    "get_config",
    "Print the current device configuration",
    get_config_execute
};

