/**
 * J1939 Diagnostic Tool - SPN Database Handler Interface
 * 
 * Versão: 3.14.0
 */

#ifndef SPN_DB_HANDLER_H
#define SPN_DB_HANDLER_H

#include <Arduino.h>

/**
 * @brief Initializes the SPN description database by loading it from LittleFS.
 * 
 * @return True if the database was loaded successfully, false otherwise.
 */
bool spn_db_init();

/**
 * @brief Gets the description for a given Suspect Parameter Number (SPN).
 * 
 * @param spn The SPN to look up.
 * @return A const char* to the description, or "Unknown SPN" if not found.
 */
const char* spn_get_description(uint32_t spn);

#endif // SPN_DB_HANDLER_H
