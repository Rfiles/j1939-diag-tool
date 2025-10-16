/**
 * J1939 Diagnostic Tool - Filesystem Handler Implementation
 * 
 * Versão: 3.8.0
 */

#include "filesystem_handler.h"
#include "config.h"
#include "error_handler.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "shared_resources.h"

// Define the global config instance
AppConfig config;

void load_default_config() {
    error_report(ErrorLevel::WARN, "FileSystem", "config.json not found. Loading default values.");

    config.j1939_node_address = 128;
    config.j1939_name = 0x8012345678ABCD00;

    config.wifi.ssid = "";
    config.wifi.password = "";

    config.mqtt.broker = "";
    config.mqtt.port = 1883;
    config.mqtt.topic = "j1939/diag_tool/data";
    
    config.buttons.select_pin = 0;
    config.buttons.next_pin = 14;

    config.display.spi_host = 2;
    config.display.sclk_pin = 7;
    config.display.mosi_pin = 6;
    config.display.cs_pin = 38;
    config.display.dc_pin = 39;
        config.display.rst_pin = 45;
        config.display.bl_pin = 46;
        config.display.display_brightness = 255;
        
        config.mcp2515.spi_host = 3;    config.mcp2515.sclk_pin = 12;
    config.mcp2515.mosi_pin = 11;
    config.mcp2515.miso_pin = 13;
    config.mcp2515.cs_pin = 10;

    config.ota.hostname = "j1939-diag-tool";
    config.ota.password = "";

    config.features.wifi_enabled = false;
    config.features.mqtt_enabled = false;
    config.features.gps_enabled = false;
}

bool filesystem_init() {
    if (xSemaphoreTake(config_mutex, portMAX_DELAY) != pdTRUE) return false;

    if (!LittleFS.begin()) {
        xSemaphoreGive(config_mutex);
        // This error is critical and will be caught in main, which will report it.
        return false;
    }

    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile) {
        load_default_config();
        xSemaphoreGive(config_mutex);
        return true; // Continue with default config
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
        String message = "Failed to parse config.json: ";
        message += error.c_str();
        error_report(ErrorLevel::CRITICAL, "FileSystem", message.c_str());
        xSemaphoreGive(config_mutex);
        return false;
    }

    // Load all values into the global config struct
    config.j1939_node_address = doc["j1939_node_address"] | 128;
    const char* name_str = doc["j1939_name"] | "0x8012345678ABCD00";
    config.j1939_name = strtoull(name_str, NULL, 16);

    config.wifi.ssid = doc["wifi"]["ssid"].as<String>();
    config.wifi.password = doc["wifi"]["password"].as<String>();

    config.mqtt.broker = doc["mqtt"]["broker"].as<String>();
    config.mqtt.port = doc["mqtt"]["port"] | 1883;
    config.mqtt.topic = doc["mqtt"]["topic"].as<String>();
    
    config.buttons.select_pin = doc["buttons"]["select_pin"] | 0;
    config.buttons.next_pin = doc["buttons"]["next_pin"] | 14;

    config.display.spi_host = doc["display"]["spi_host"] | 2;
    config.display.sclk_pin = doc["display"]["sclk_pin"] | 7;
    config.display.mosi_pin = doc["display"]["mosi_pin"] | 6;
    config.display.cs_pin = doc["display"]["cs_pin"] | 38;
    config.display.dc_pin = doc["display"]["dc_pin"] | 39;
        config.display.rst_pin = doc["display"]["rst_pin"] | 45;
        config.display.bl_pin = doc["display"]["bl_pin"] | 46;
        config.display.display_brightness = doc["display"]["display_brightness"] | 255;
    
        config.mcp2515.spi_host = doc["mcp2515"]["spi_host"] | 3;
    config.mcp2515.sclk_pin = doc["mcp2515"]["sclk_pin"] | 12;
    config.mcp2515.mosi_pin = doc["mcp2515"]["mosi_pin"] | 11;
    config.mcp2515.miso_pin = doc["mcp2515"]["miso_pin"] | 13;
    config.mcp2515.cs_pin = doc["mcp2515"]["cs_pin"] | 10;

    config.ota.hostname = doc["ota"]["hostname"].as<String>();
    config.ota.password = doc["ota"]["password"].as<String>();

    config.features.wifi_enabled = doc["features"]["wifi_enabled"] | true;
    config.features.mqtt_enabled = doc["features"]["mqtt_enabled"] | true;
    config.features.gps_enabled = doc["features"]["gps_enabled"] | true;

    configFile.close();
    error_report(ErrorLevel::INFO, "FileSystem", "Configuration loaded successfully.");
    
bool save_default_config() {
    // Create a default config file
    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile) {
        error_report(ErrorLevel::CRITICAL, "FileSystem", "Failed to create config.json.");
        return false;
    }

    JsonDocument doc;
    doc["j1939_node_address"] = 128;
    doc["j1939_name"] = "0x8012345678ABCD00";
    doc["wifi"]["ssid"] = "";
    doc["wifi"]["password"] = "";
    doc["mqtt"]["broker"] = "";
    doc["mqtt"]["port"] = 1883;
    doc["mqtt"]["topic"] = "j1939/diag_tool/data";
    doc["buttons"]["select_pin"] = 0;
    doc["buttons"]["next_pin"] = 14;
    doc["display"]["spi_host"] = 2;
    doc["display"]["sclk_pin"] = 7;
    doc["display"]["mosi_pin"] = 6;
    doc["display"]["cs_pin"] = 38;
    doc["display"]["dc_pin"] = 39;
    doc["display"]["rst_pin"] = 45;
    doc["display"]["bl_pin"] = 46;
    doc["display"]["display_brightness"] = 255;
    doc["mcp2515"]["spi_host"] = 3;
    doc["mcp2515"]["sclk_pin"] = 12;
    doc["mcp2515"]["mosi_pin"] = 11;
    doc["mcp2515"]["miso_pin"] = 13;
    doc["mcp2515"]["cs_pin"] = 10;
    doc["ota"]["hostname"] = "j1939-diag-tool";
    doc["ota"]["password"] = "";
    doc["features"]["wifi_enabled"] = false;
    doc["features"]["mqtt_enabled"] = false;
    doc["features"]["gps_enabled"] = false;

    if (serializeJson(doc, configFile) == 0) {
        error_report(ErrorLevel::CRITICAL, "FileSystem", "Failed to write to config.json.");
        configFile.close();
        return false;
    }

    configFile.close();
    error_report(ErrorLevel::INFO, "FileSystem", "Default configuration saved.");
    return true;
}

bool filesystem_format_and_create_defaults() {
    error_report(ErrorLevel::WARN, "FileSystem", "Formatting filesystem and creating default files.");

    if (!LittleFS.format()) {
        error_report(ErrorLevel::CRITICAL, "FileSystem", "Filesystem format failed.");
        return false;
    }

    // Re-mount the filesystem after formatting
    if (!LittleFS.begin()) {
        error_report(ErrorLevel::CRITICAL, "FileSystem", "Failed to mount filesystem after format.");
        return false;
    }

    if (!save_default_config()) {
        return false;
    }

    // You can add creation of other default files here if needed

    error_report(ErrorLevel::INFO, "FileSystem", "Filesystem formatted and default files created successfully.");
    return true;
}

