/**
 * J1939 Diagnostic Tool - Transport Protocol Handler Interface
 * 
 * Versão: 3.0.0
 */

#ifndef TP_HANDLER_H
#define TP_HANDLER_H

#include "../driver/mcp2515_driver.h" // For can_frame
#include <Arduino.h>

#define MAX_TP_MESSAGE_SIZE 1785 // Max size for a J1939 TP message
#define MAX_TP_SESSIONS 5      // Max concurrent TP sessions

// Represents a fully reassembled multi-packet message
struct ReassembledPDU {
    uint32_t pgn;                  // PGN of the message
    uint8_t src_addr;              // Source address
    uint8_t data[MAX_TP_MESSAGE_SIZE];
    uint16_t data_length;
};

// --- RTOS Queue ---
// Queue for the TP handler to send fully reassembled PDUs to the application layer
extern QueueHandle_t j1939_complete_pdu_queue;

// Struct for an outgoing multi-packet message request
struct OutgoingTPMessage {
    uint32_t pgn;
    uint8_t dest_addr;
    uint8_t data[MAX_TP_MESSAGE_SIZE];
    uint16_t data_length;
};

// Queue for sending multi-packet messages
extern QueueHandle_t j1939_tx_tp_queue;

/**
 * @brief Initializes the Transport Protocol handler.
 */
void tp_handler_init();

/**
 * @brief Queues a multi-packet message to be sent via Transport Protocol.
 * 
 * @param pgn The PGN of the message to send.
 * @param dest_addr The destination address.
 * @param data A pointer to the data payload.
 * @param length The length of the data payload.
 */
void tp_send_message(uint32_t pgn, uint8_t dest_addr, const uint8_t* data, uint16_t length);

/**
 * @brief Processes an incoming CAN frame to check if it's part of a transport protocol session.
 * 
 * @param frame The incoming CAN frame.
 * @return True if the frame was consumed by the TP handler, false otherwise.
 */
bool tp_handler_process_frame(const can_frame* frame);

#endif // TP_HANDLER_H
