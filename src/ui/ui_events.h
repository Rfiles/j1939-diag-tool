#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "../j1939/dm_handler.h"

// Queue for sending decoded DTCs to the UI task
extern QueueHandle_t dtc_queue;

#endif // UI_EVENTS_H
