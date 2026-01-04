/**
 * @file gps_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - GPS Handler Task Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <Arduino.h>

/**
 * @brief Enum to represent GPS fix status.
 */
enum GpsFixStatus {
    GPS_FIX_NONE,
    GPS_FIX_2D,
    GPS_FIX_3D
};

/**
 * @brief Struct to hold parsed GPS data.
 */
struct GpsData {
    float latitude = 0.0;
    float longitude = 0.0;
    float speed_kmh = 0.0;
    bool is_valid = false;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    GpsFixStatus fix_status = GPS_FIX_NONE;
    int satellites = 0;
};

/**
 * @brief Queue for the GPS task to send parsed data to other tasks.
 */
extern QueueHandle_t gps_data_queue;

/**
 * @brief Initializes the GPS handler and creates the task.
 */
void gps_handler_init();

/**
 * @brief Gets the last known GPS data in a thread-safe way.
 * 
 * @param data Pointer to a GpsData struct to be filled.
 * @return True if the copied data is valid, false otherwise.
 */
bool gps_get_last_data(GpsData* data);

/**
 * @brief Gets the last known GPS time and populates a tm struct.
 * 
 * @param timeinfo Pointer to a tm struct to be filled.
 * @return True if the time is valid, false otherwise.
 */
bool gps_get_time(struct tm* timeinfo);

/**
 * @brief Gets the current GPS status.
 * 
 * @return The current GpsFixStatus.
 */
GpsFixStatus gps_get_status();


#endif // GPS_HANDLER_H