/**
 * J1939 Diagnostic Tool - J1939 Handler Task Interface
 * 
 * Versão: 2.1.0
 */

#ifndef J1939_HANDLER_H
#define J1939_HANDLER_H

#include <Arduino.h>
#include "j1939_pgn.h"
#include "../driver/mcp2515_driver.h" // For can_frame struct

// A struct for requesting a PGN to be sent
struct J1939TxRequest {
    uint32_t pgn;
    uint8_t dest_address;
};

// --- RTOS Queues ---
// Queue for other tasks to send TX requests to the J1939 task
extern QueueHandle_t j1939_tx_queue;

// Queue for the J1939 task to send received messages to other tasks
extern QueueHandle_t j1939_rx_queue;


// --- Public API ---

// Initializes the J1939 handler, creating the task and queues
void j1939_handler_init();

#endif // J1939_HANDLER_H