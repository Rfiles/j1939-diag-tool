/**
 * @file time_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Time Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <Arduino.h>

/**
 * @brief Enum to define the source for time synchronization.
 */
enum TimeSource {
    TIME_SOURCE_NTP,
    TIME_SOURCE_GPS
};

/**
 * @brief Initializes the time service.
 * 
 * @param source The initial time source to use (NTP or GPS).
 */
void time_handler_init(TimeSource source = TIME_SOURCE_NTP);

/**
 * @brief Sets the time synchronization source.
 * 
 * @param source The time source to use (NTP or GPS).
 */
void time_set_source(TimeSource source);

/**
 * @brief Synchronizes the system time from the configured source.
 * 
 * @return True if synchronization was successful, false otherwise.
 */
bool time_sync();

/**
 * @brief Gets the current time as a formatted ISO8601 string.
 * 
 * @return A string with the current timestamp (e.g., "2025-10-11T18:30:00Z"),
 *         or "[TIME NOT SYNCED]" if the time has not been synchronized yet.
 */
String time_handler_get_timestamp();

#endif // TIME_HANDLER_H