/**
 * J1939 Diagnostic Tool - J1939 Handler Task with Address Claiming
 * 
 * Versão: 3.1.0
 */

#include "j1939_handler.h"
#include "../core/config.h"
#include "../core/error_handler.h"
#include "tp_handler.h"

// --- PGNs ---
#define PGN_ADDRESS_CLAIMED 0xEE00 // 60928
#define PGN_REQUEST         0xEA00 // 59904
#define PGN_ACKNOWLEDGEMENT 0xE800 // 59392

// --- RTOS Queues ---
QueueHandle_t j1939_tx_queue;
QueueHandle_t j1939_rx_queue;

// --- Module State ---
static J1939_ADDRESS_CLAIM_STATE ac_state = AC_STATE_UNCLAIMED;
static uint8_t current_source_address = 254; // Start with NULL address
static unsigned long claim_start_time = 0;
static unsigned long last_rx_time = 0;

// --- Private Functions ---

void send_address_claim() {
    can_frame frame;
    frame.can_id = (6UL << 26) | (PGN_ADDRESS_CLAIMED << 8) | 0xFF0000 | current_source_address;
    frame.can_dlc = 8;
    // Copy 64-bit NAME into data field (little-endian)
    memcpy(frame.data, &config.j1939_name, 8);
    mcp2515_transmit(&frame);
    char msg[50];
    sprintf(msg, "Sent Address Claim for address %d", current_source_address);
    error_report(ErrorLevel::INFO, "J1939", msg);
}

void process_incoming_address_claim(const can_frame* frame) {
    uint8_t incoming_addr = frame->can_id & 0xFF;
    if (incoming_addr != current_source_address) {
        return; // Claim for a different address, we don't care
    }

    // Conflict! Someone is claiming our address.
    uint64_t their_name;
    memcpy(&their_name, frame->data, 8);

    if (config.j1939_name < their_name) {
        // Our NAME is smaller, we win. Defend our address.
        send_address_claim();
        error_report(ErrorLevel::INFO, "J1939", "Address contention won, defending address.");
    } else {
        // Their NAME is smaller, we lose.
        error_report(ErrorLevel::CRITICAL, "J1939", "Address contention lost. Cannot claim address.");
        ac_state = AC_STATE_CANNOT_CLAIM;
        current_source_address = 254; // Use NULL address
    }
}

void process_acknowledgement(const can_frame* frame) {
    uint8_t control_byte = frame->data[0];
    uint32_t ack_pgn = frame->data[5] | (frame->data[6] << 8) | (frame->data[7] << 16);
    uint8_t src_addr = frame->can_id & 0xFF;
    char msg[100];

    const char* ack_type;
    switch (control_byte) {
        case 0: ack_type = "Positive ACK"; break;
        case 1: ack_type = "Negative ACK"; break;
        default: ack_type = "Unknown ACK"; break;
    }

    sprintf(msg, "%s for PGN %lu from address %d", ack_type, ack_pgn, src_addr);
    error_report(ErrorLevel::INFO, "J1939_ACK", msg);
}

// --- Task Function ---

void j1939_task_fn(void* pvParameters) {
    if (!mcp2515_init()) {
        error_report(ErrorLevel::CRITICAL, "J1939", "MCP2515 Init Failed! Halting task.");
        vTaskDelete(NULL);
    }

    current_source_address = config.j1939_node_address;
    ac_state = AC_STATE_UNCLAIMED;

    J1939TxRequest tx_request;
    can_frame received_frame;

    for (;;) {
        // --- State Machine for Address Claiming ---
        switch (ac_state) {
            case AC_STATE_UNCLAIMED:
                send_address_claim();
                claim_start_time = millis();
                ac_state = AC_STATE_CLAIMING;
                break;

            case AC_STATE_CLAIMING:
                if (millis() - claim_start_time > 250) {
                    // 250ms has passed without conflict, we have successfully claimed the address.
                    ac_state = AC_STATE_CLAIMED;
                    char msg[50];
                    sprintf(msg, "Address %d claimed successfully.", current_source_address);
                    error_report(ErrorLevel::INFO, "J1939", msg);
                }
                break;

            case AC_STATE_CLAIMED:
            case AC_STATE_CANNOT_CLAIM:
                // Do nothing, just continue normal operation
                break;
        }

        // --- Message Processing ---
        if (mcp2515_receive(&received_frame)) {
            last_rx_time = millis();
            uint32_t pgn = (received_frame.can_id >> 8) & 0x1FFFF;

            if (pgn == PGN_ADDRESS_CLAIMED) {
                process_incoming_address_claim(&received_frame);
            } else if (pgn == PGN_ACKNOWLEDGEMENT) {
                process_acknowledgement(&received_frame);
            } else if (!tp_handler_process_frame(&received_frame)) {
                xQueueSend(j1939_rx_queue, &received_frame, 0);
            }
        }

        if (ac_state == AC_STATE_CLAIMED && xQueueReceive(j1939_tx_queue, &tx_request, 0) == pdPASS) {
            can_frame frame_to_send;
            uint8_t request_data[3];
            request_data[0] = tx_request.pgn & 0xFF;
            request_data[1] = (tx_request.pgn >> 8) & 0xFF;
            request_data[2] = (tx_request.pgn >> 16) & 0xFF;

            frame_to_send.can_id = (6UL << 26) | (PGN_REQUEST << 8) | (tx_request.dest_address << 16) | current_source_address;
            frame_to_send.can_dlc = 3;
            memcpy(frame_to_send.data, request_data, 3);
            mcp2515_transmit(&frame_to_send);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void j1939_request_pgn(uint32_t pgn, uint8_t dest_address) {
    J1939TxRequest request;
    request.pgn = pgn;
    request.dest_address = dest_address;
    xQueueSend(j1939_tx_queue, &request, 0);
}

// --- Public API ---

void j1939_handler_init() {
    j1939_tx_queue = xQueueCreate(10, sizeof(J1939TxRequest));
    j1939_rx_queue = xQueueCreate(20, sizeof(can_frame));
    xTaskCreate(j1939_task_fn, "J1939 Task", 4096, NULL, 5, NULL);
}

uint8_t j1939_get_source_address() {
    return current_source_address;
}

unsigned long j1939_get_last_rx_time() {
    return last_rx_time;
}
