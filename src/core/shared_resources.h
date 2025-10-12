/**
 * J1939 Diagnostic Tool - Shared Resources Management Interface
 * 
 * Versão: 3.5.0
 */

#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Mutex for protecting access to the global AppConfig struct
extern SemaphoreHandle_t config_mutex;

// Mutex for protecting access to the global vehicle database
extern SemaphoreHandle_t vehicle_db_mutex;

/**
 * @brief Initializes all shared resources, such as mutexes.
 */
void shared_resources_init();

#endif // SHARED_RESOURCES_H
