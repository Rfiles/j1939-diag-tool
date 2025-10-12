/**
 * J1939 Diagnostic Tool - FMI Database Handler Interface
 * 
 * Versão: 3.17.0
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
