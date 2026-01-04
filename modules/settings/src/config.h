/**
 * @file config.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Configuration Structs and Constants
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define FIRMWARE_VERSION "4.3.0"

/**
 * @brief Struct to hold all application configuration.
 */
struct AppConfig {
    uint8_t j1939_node_address;
    uint64_t j1939_name;
    struct {
        String ssid;
        String password;
    } wifi;
    struct {
        String broker;
        int port;
        String topic;
    } mqtt;
    struct {
        String server;
    } ntp;
    struct {
        uint8_t select_pin;
        uint8_t next_pin;
    } buttons;
    struct {
        int spi_host;
        int sclk_pin;
        int mosi_pin;
        int cs_pin;
        int dc_pin;
        int rst_pin;
        int bl_pin;
        uint8_t display_brightness;
    } display;
    struct {
        int spi_host;
        int sclk_pin;
        int mosi_pin;
        int miso_pin;
        int cs_pin;
    } mcp2515;
    struct {
        String hostname;
        String password;
    } ota;
    struct {
        bool wifi_enabled;
        bool mqtt_enabled;
        bool gps_enabled;
    } features;
};

/**
 * @brief Global instance of the application configuration.
 * 
 * This will be populated by the filesystem_handler on startup.
 */
extern AppConfig config;

#endif // CONFIG_H