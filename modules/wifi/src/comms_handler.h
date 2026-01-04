/**
 * @file comms_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Comms Handler Task Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef COMMS_HANDLER_H
#define COMMS_HANDLER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/**
 * @brief Enum for the state of the Communications Handler state machine.
 */
enum CommsState {
    COMMS_STATE_STARTING,
    COMMS_STATE_WIFI_CONNECTING,
    COMMS_STATE_MQTT_CONNECTING,
    COMMS_STATE_OPERATIONAL,
    COMMS_STATE_WIFI_DISCONNECTED
};

/**
 * @brief Enum for the detailed WiFi status.
 */
enum WiFiStatus {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_INTERNET
};

/**
 * @brief Queue for high-priority telemetry messages (payloads are heap-allocated strings).
 */
extern QueueHandle_t telemetry_queue;

/**
 * @brief Initializes the Communications handler and creates the task.
 */
void comms_handler_init();

/**
 * @brief Sends a JSON payload to the telemetry queue for immediate publication.
 * 
 * @param payload A heap-allocated C-string (char*) containing the JSON payload.
 *                The comms task will take ownership and free the memory.
 * @return True if the message was successfully sent to the queue.
 */
bool comms_publish_telemetry(const char* payload);

/**
 * @brief Gets the current WiFi status.
 * 
 * @return The current WiFiStatus.
 */
WiFiStatus comms_get_wifi_status();

#endif // COMMS_HANDLER_H