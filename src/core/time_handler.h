/**
 * J1939 Diagnostic Tool - NTP Time Handler Interface
 * 
 * Versão: 2.5.0
 */

#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <Arduino.h>

/**
 * @brief Initializes the NTP time service. Must be called after WiFi is connected.
 */
void time_handler_init();

/**
 * @brief Gets the current time as a formatted ISO8601 string.
 * 
 * @return A string with the current timestamp (e.g., "2025-10-11T18:30:00Z"),
 *         or "[TIME NOT SYNCED]" if the time has not been synchronized yet.
 */
String time_handler_get_timestamp();

#endif // TIME_HANDLER_H
