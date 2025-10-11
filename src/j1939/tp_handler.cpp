/**
 * J1939 Diagnostic Tool - Transport Protocol Handler Implementation
 * 
 * Versão: 3.0.0
 */

#include "tp_handler.h"
#include "../core/error_handler.h"
#include <Arduino.h>

// PGNs for Transport Protocol
#define PGN_TP_CM 0xEC00 // 60416
#define PGN_TP_DT 0xEB00 // 60160

// Control Bytes for TP.CM
#define TP_CM_BAM 32

// State for an ongoing TP session
struct TPSession {
    bool is_active;
    uint32_t pgn;
    uint16_t total_size;
    uint8_t total_packets;
    uint8_t packets_received;
    uint8_t src_addr;
    uint8_t data_buffer[MAX_TP_MESSAGE_SIZE];
};

// --- Module State ---
static TPSession active_sessions[MAX_TP_SESSIONS];
QueueHandle_t j1939_complete_pdu_queue;

// --- Public API ---

void tp_handler_init() {
    j1939_complete_pdu_queue = xQueueCreate(5, sizeof(ReassembledPDU));
    for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
        active_sessions[i].is_active = false;
    }
}

bool tp_handler_process_frame(const can_frame* frame) {
    uint32_t pgn = (frame->can_id >> 8) & 0x1FFFF;

    if (pgn == PGN_TP_CM) {
        uint8_t control_byte = frame->data[0];
        if (control_byte == TP_CM_BAM) {
            // Find a free session slot
            int session_index = -1;
            for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
                if (!active_sessions[i].is_active) {
                    session_index = i;
                    break;
                }
            }

            if (session_index == -1) {
                error_report(ErrorLevel::WARN, "TP", "No free session for new BAM.");
                return true; // Consumed, but couldn't handle
            }

            TPSession* session = &active_sessions[session_index];
            session->is_active = true;
            session->src_addr = frame->can_id & 0xFF;
            session->total_size = (frame->data[2] << 8) | frame->data[1];
            session->total_packets = frame->data[3];
            session->pgn = (frame->data[7] << 16) | (frame->data[6] << 8) | frame->data[5];
            session->packets_received = 0;
            
            return true;
        }
        // TODO: Handle connection-mode TP (RTS, CTS, EOM, Abort)

    } else if (pgn == PGN_TP_DT) {
        uint8_t src = frame->can_id & 0xFF;
        
        // Find the session for this source address
        TPSession* session = nullptr;
        for (int i = 0; i < MAX_TP_SESSIONS; ++i) {
            if (active_sessions[i].is_active && active_sessions[i].src_addr == src) {
                session = &active_sessions[i];
                break;
            }
        }

        if (session == nullptr) {
            return false; // Not for us, or we missed the BAM
        }

        uint8_t sequence_num = frame->data[0];
        if (sequence_num != session->packets_received + 1) {
            // Out of order packet, abort session
            error_report(ErrorLevel::WARN, "TP", "Packet out of order. Aborting session.");
            session->is_active = false;
            return true;
        }

        // Copy data
        int offset = session->packets_received * 7;
        for (int i = 1; i < 8; ++i) {
            if (offset + (i - 1) < session->total_size) {
                session->data_buffer[offset + (i - 1)] = frame->data[i];
            }
        }
        session->packets_received++;

        // Check if message is complete
        if (session->packets_received == session->total_packets) {
            ReassembledPDU pdu;
            pdu.pgn = session->pgn;
            pdu.src_addr = session->src_addr;
            pdu.data_length = session->total_size;
            memcpy(pdu.data, session->data_buffer, session->total_size);

            if (xQueueSend(j1939_complete_pdu_queue, &pdu, 0) != pdPASS) {
                error_report(ErrorLevel::ERROR, "TP", "Failed to queue complete PDU.");
            }

            // Free the session
            session->is_active = false;
        }
        return true;
    }

    return false; // Not a TP frame
}
