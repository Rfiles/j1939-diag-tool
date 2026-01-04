/**
 * @file ota_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - OTA Handler
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

/**
 * @brief Initializes the OTA handler.
 * 
 * This function initializes the OTA update mechanism.
 */
void ota_handler_init();

/**
 * @brief The main loop for the OTA handler.
 * 
 * This function is called repeatedly to handle OTA updates.
 */
void ota_handler_loop();

#endif // OTA_HANDLER_H