/**
 * J1939 Diagnostic Tool - GPS Handler Task Interface
 * 
 * Versão: 2.2.0
 */

#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <Arduino.h>

// Struct to hold parsed GPS data
struct GpsData {
    float latitude = 0.0;
    float longitude = 0.0;
    float speed_kmh = 0.0;
    bool is_valid = false;
};

// --- RTOS Queue ---
// Queue for the GPS task to send parsed data to other tasks
extern QueueHandle_t gps_data_queue;

// --- Public API ---

// Initializes the GPS handler and creates the task
void gps_handler_init();

/**
 * @brief Gets the last known GPS data in a thread-safe way.
 * 
 * @param data Pointer to a GpsData struct to be filled.
 * @return True if the copied data is valid, false otherwise.
 */
bool gps_get_last_data(GpsData* data);

#endif // GPS_HANDLER_H
