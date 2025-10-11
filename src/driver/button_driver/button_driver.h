/**
 * J1939 Diagnostic Tool - Advanced Button Driver Interface
 * 
 * Versão: 2.1.0
 */

#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <Arduino.h>

// Enum for all possible button events
typedef enum {
    BTN_EVENT_NONE,
    // Select Button Events
    BTN_PRESS_SELECT,
    BTN_LONG_PRESS_SELECT,
    BTN_DOUBLE_CLICK_SELECT,
    // Next Button Events
    BTN_PRESS_NEXT,
    BTN_LONG_PRESS_NEXT,
    BTN_DOUBLE_CLICK_NEXT,
} button_event_t;

// --- RTOS Queue ---
// Queue for the button driver to send events to the UI task
extern QueueHandle_t button_event_queue;

// --- Public API ---

// Initializes the button driver and creates the polling task
void button_driver_init();

#endif // BUTTON_DRIVER_H
