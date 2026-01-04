/**
 * @file fmi_db_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - FMI Database Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FMI_DB_HANDLER_H
#define FMI_DB_HANDLER_H

#include <Arduino.h>

/**
 * @brief Initializes the FMI description database by loading it from LittleFS.
 * 
 * @return True if the database was loaded successfully, false otherwise.
 */
bool fmi_db_init();

/**
 * @brief Gets the description for a given Failure Mode Identifier (FMI).
 * 
 * @param fmi The FMI to look up.
 * @return A const char* to the description, or "Unknown FMI" if not found.
 */
const char* fmi_get_description(uint8_t fmi);

#endif // FMI_DB_HANDLER_H