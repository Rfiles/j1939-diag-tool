/**
 * @file button_driver.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Advanced Button Driver Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <Arduino.h>

/**
 * @brief Enum for all possible button events.
 */
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

/**
 * @brief Queue for the button driver to send events to the UI task.
 */
extern QueueHandle_t button_event_queue;


/**
 * @brief Initializes the button driver and creates the polling task.
 */
void button_driver_init();

#endif // BUTTON_DRIVER_H