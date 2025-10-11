/**
 * J1939 Diagnostic Tool - J1939 Handler Task Implementation
 * 
 * Versão: 2.1.0
 */

#include "j1939_handler.h"
#include "../core/config.h"

// --- RTOS Queues ---
QueueHandle_t j1939_tx_queue;
QueueHandle_t j1939_rx_queue;

// --- Private Task Function ---

void j1939_task_fn(void* pvParameters) {
    // Initialize the CAN driver
    if (!mcp2515_init()) {
        Serial.println("MCP2515 Init Failed! Halting J1939 Task.");
        vTaskDelete(NULL);
    }

    J1939TxRequest tx_request;
    can_frame received_frame;

    for (;;) {
        // 1. Check for requests to transmit a PGN
        if (xQueueReceive(j1939_tx_queue, &tx_request, 0) == pdPASS) {
            can_frame frame_to_send;
            uint8_t request_data[3];
            request_data[0] = tx_request.pgn & 0xFF;
            request_data[1] = (tx_request.pgn >> 8) & 0xFF;
            request_data[2] = (tx_request.pgn >> 16) & 0xFF;

            frame_to_send.can_id = (6UL << 26) | ((uint32_t)J1939_PGN_REQUEST << 8) | ((uint32_t)tx_request.dest_address << 16) | config.j1939_node_address;
            frame_to_send.can_dlc = 3;
            memcpy(frame_to_send.data, request_data, 3);
            mcp2515_transmit(&frame_to_send);
        }

        // 2. Check for received CAN frames
        if (mcp2515_receive(&received_frame)) {
            // Send the raw frame to the RX queue for other tasks to process
            xQueueSend(j1939_rx_queue, &received_frame, pdMS_TO_TICKS(10));
        }

        // 3. Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// --- Public API Implementation ---

void j1939_handler_init() {
    // Create the queues, allocating them in PSRAM
    j1939_tx_queue = xQueueCreateStatic(10, sizeof(J1939TxRequest), (uint8_t*)ps_malloc(10 * sizeof(J1939TxRequest)), new StaticQueue_t);
    j1939_rx_queue = xQueueCreateStatic(20, sizeof(can_frame), (uint8_t*)ps_malloc(20 * sizeof(can_frame)), new StaticQueue_t);

    // Create the J1939 task
    xTaskCreate(
        j1939_task_fn,
        "J1939 Task",
        4096, // Stack size
        NULL,
        5,    // High priority to not miss CAN frames
        NULL
    );
}