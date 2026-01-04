/**
 * @file settings_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - NVS Settings Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SETTINGS_HANDLER_H
#define SETTINGS_HANDLER_H

/**
 * @brief Initializes the NVS (Preferences) system.
 */
void settings_init();

/**
 * @brief Loads settings from NVS and overrides the default config.
 */
void settings_load();

/**
 * @brief Saves the current in-memory config to NVS.
 */
void settings_save();

/**
 * @brief Checks for a firmware version change and handles necessary updates (e.g., resetting flags).
 */
void settings_check_version_update();

/**
 * @brief Checks if the developer command has been used for the current firmware version.
 * 
 * @return True if the command is disabled, false otherwise.
 */
bool settings_is_dev_cmd_disabled();

/**
 * @brief Disables the developer command for the current firmware version.
 */
void settings_disable_dev_cmd();

#endif // SETTINGS_HANDLER_H