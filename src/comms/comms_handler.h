/**
 * J1939 Diagnostic Tool - Comms Handler Task Interface
 * 
 * Versão: 2.2.0
 */

#ifndef COMMS_HANDLER_H
#define COMMS_HANDLER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// --- RTOS Queue ---
// Queue for high-priority telemetry messages (payloads are heap-allocated strings)
extern QueueHandle_t telemetry_queue;

// Initializes the Communications handler and creates the task
void comms_handler_init();

/**
 * @brief Sends a JSON payload to the telemetry queue for immediate publication.
 * 
 * @param payload A heap-allocated C-string (char*) containing the JSON payload.
 *                The comms task will take ownership and free the memory.
 * @return True if the message was successfully sent to the queue.
 */
bool comms_publish_telemetry(const char* payload);

#endif // COMMS_HANDLER_H
