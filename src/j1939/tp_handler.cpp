/**
 * J1939 Diagnostic Tool - Transport Protocol Handler with TX
 * 
 * Versão: 3.24.0
 */

#include "tp_handler.h"
#include "../core/error_handler.h"
#include "../driver/mcp2515_driver.h"
#include "j1939_handler.h"
#include <Arduino.h>

// ... (PGN defines and TPSession struct)

// --- Module State ---
static TPSession active_sessions[MAX_TP_SESSIONS];
QueueHandle_t j1939_complete_pdu_queue;
QueueHandle_t j1939_tx_tp_queue;

// --- TX Task ---
void tp_tx_task_fn(void* pv) {
    OutgoingTPMessage msg_to_send;
    for (;;) {
        if (xQueueReceive(j1939_tx_tp_queue, &msg_to_send, portMAX_DELAY) == pdPASS) {
            // For now, we only implement BAM (Broadcast Announce Message)
            uint8_t num_packets = (msg_to_send.data_length + 6) / 7;

            // 1. Send BAM (TP.CM)
            can_frame bam_frame;
            bam_frame.can_id = (6UL << 26) | (PGN_TP_CM << 8) | 0xFF0000 | j1939_get_source_address();
            bam_frame.can_dlc = 8;
            bam_frame.data[0] = TP_CM_BAM;
            bam_frame.data[1] = msg_to_send.data_length & 0xFF;
            bam_frame.data[2] = (msg_to_send.data_length >> 8) & 0xFF;
            bam_frame.data[3] = num_packets;
            bam_frame.data[4] = 0xFF; // Reserved
            bam_frame.data[5] = msg_to_send.pgn & 0xFF;
            bam_frame.data[6] = (msg_to_send.pgn >> 8) & 0xFF;
            bam_frame.data[7] = (msg_to_send.pgn >> 16) & 0xFF;
            mcp2515_transmit(&bam_frame);

            vTaskDelay(pdMS_TO_TICKS(50)); // Wait 50ms after BAM

            // 2. Send Data Packets (TP.DT)
            for (int i = 0; i < num_packets; ++i) {
                can_frame dt_frame;
                dt_frame.can_id = (6UL << 26) | (PGN_TP_DT << 8) | 0xFF0000 | j1939_get_source_address();
                dt_frame.can_dlc = 8;
                dt_frame.data[0] = i + 1; // Sequence number
                int offset = i * 7;
                int data_len = (i == num_packets - 1) ? (msg_to_send.data_length - offset) : 7;
                memcpy(&dt_frame.data[1], &msg_to_send.data[offset], data_len);
                // Fill remaining bytes with 0xFF
                for (int j = 1 + data_len; j < 8; ++j) {
                    dt_frame.data[j] = 0xFF;
                }
                mcp2515_transmit(&dt_frame);
                vTaskDelay(pdMS_TO_TICKS(50)); // 50ms between packets
            }
        }
    }
}

// --- Public API ---

void tp_handler_init() {
    j1939_complete_pdu_queue = xQueueCreate(5, sizeof(ReassembledPDU));
    j1939_tx_tp_queue = xQueueCreate(5, sizeof(OutgoingTPMessage));
    for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
        active_sessions[i].is_active = false;
    }
    xTaskCreate(tp_tx_task_fn, "TP TX Task", 4096, NULL, 4, NULL);
}

void tp_send_message(uint32_t pgn, uint8_t dest_addr, const uint8_t* data, uint16_t length) {
    if (length > MAX_TP_MESSAGE_SIZE) return;
    OutgoingTPMessage msg;
    msg.pgn = pgn;
    msg.dest_addr = dest_addr;
    msg.data_length = length;
    memcpy(msg.data, data, length);
    xQueueSend(j1939_tx_tp_queue, &msg, 0);
}

// ... (tp_handler_process_frame implementation remains the same) ...