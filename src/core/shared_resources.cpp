/**
 * J1939 Diagnostic Tool - Shared Resources Management Implementation
 * 
 * Versão: 3.5.0
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include "shared_resources.h"

// Define the mutex handles
SemaphoreHandle_t config_mutex;
SemaphoreHandle_t vehicle_db_mutex;

void shared_resources_init() {
    config_mutex = xSemaphoreCreateMutex();
    vehicle_db_mutex = xSemaphoreCreateMutex();
}
