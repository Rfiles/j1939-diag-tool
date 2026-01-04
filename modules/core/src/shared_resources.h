/**
 * @file shared_resources.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Shared Resources Management Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @brief Mutex for protecting access to the global AppConfig struct.
 */
extern SemaphoreHandle_t config_mutex;

/**
 * @brief Mutex for protecting access to the global vehicle database.
 */
extern SemaphoreHandle_t vehicle_db_mutex;

/**
 * @brief Initializes all shared resources, such as mutexes.
 */
void shared_resources_init();

#endif // SHARED_RESOURCES_H