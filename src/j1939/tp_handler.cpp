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

// PGNs for Transport Protocol
#define PGN_TP_CM 0x00EC00
#define PGN_TP_DT 0x00EB00

// Control bytes for TP.CM
#define TP_CM_RTS 16
#define TP_CM_CTS 17
#define TP_CM_EndOfMsgACK 19
#define TP_CM_BAM 32
#define TP_CM_Abort 255

// Represents the state of a single transport protocol session (incoming)
struct TPSession {
    bool is_active;
    bool is_bam;
    uint32_t pgn;
    uint16_t total_size;
    uint8_t total_packets;
    uint8_t packets_received;
    uint8_t src_addr;
    uint8_t dest_addr;
    uint8_t data[MAX_TP_MESSAGE_SIZE];
    uint32_t last_activity;
};

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

static TPSession* find_session(uint8_t src_addr, uint8_t dest_addr, uint32_t pgn) {
    for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
        if (active_sessions[i].is_active && 
            active_sessions[i].src_addr == src_addr &&
            active_sessions[i].dest_addr == dest_addr &&
            active_sessions[i].pgn == pgn) {
            return &active_sessions[i];
        }
    }
    return NULL;
}

bool tp_handler_process_frame(const can_frame* frame) {
    uint32_t pgn = (frame->can_id >> 8) & 0x1FFFF;

    if (pgn == PGN_TP_CM) {
        uint8_t control_byte = frame->data[0];
        uint8_t src_addr = frame->can_id & 0xFF;

        if (control_byte == TP_CM_BAM) {
            // Find an inactive session
            TPSession* session = NULL;
            for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
                if (!active_sessions[i].is_active) {
                    session = &active_sessions[i];
                    break;
                }
            }

            if (session) {
                session->is_active = true;
                session->is_bam = true;
                session->src_addr = src_addr;
                session->dest_addr = 0xFF; // Broadcast
                session->total_size = (frame->data[2] << 8) | frame->data[1];
                session->total_packets = frame->data[3];
                session->pgn = (frame->data[7] << 16) | (frame->data[6] << 8) | frame->data[5];
                session->packets_received = 0;
                session->last_activity = millis();
            }
            return true; // Frame consumed
        }
        // ... (Handle other TP.CM messages like RTS, CTS, etc. here)

    } else if (pgn == PGN_TP_DT) {
        uint8_t src_addr = frame->can_id & 0xFF;
        uint8_t seq_num = frame->data[0];

        // Find the session this packet belongs to (BAM sessions are identified by src_addr and dest_addr 0xFF)
        TPSession* session = find_session(src_addr, 0xFF, 0); // PGN is not known at this point for DTs in a BAM
        if (!session) {
             for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
                if (active_sessions[i].is_active && active_sessions[i].is_bam && active_sessions[i].src_addr == src_addr) {
                    session = &active_sessions[i];
                    break;
                }
            }
        }

        if (session && (seq_num == session->packets_received + 1)) {
            int offset = (seq_num - 1) * 7;
            int data_len = (session->total_size - offset < 7) ? (session->total_size - offset) : 7;
            memcpy(&session->data[offset], &frame->data[1], data_len);
            session->packets_received++;
            session->last_activity = millis();

            if (session->packets_received == session->total_packets) {
                // Message fully reassembled
                ReassembledPDU pdu;
                pdu.pgn = session->pgn;
                pdu.src_addr = session->src_addr;
                pdu.data_length = session->total_size;
                memcpy(pdu.data, session->data, session->total_size);
                xQueueSend(j1939_complete_pdu_queue, &pdu, 0);
                session->is_active = false; // Deactivate session
            }
            return true; // Frame consumed
        }
    }

    return false; // Frame not part of a TP session
}