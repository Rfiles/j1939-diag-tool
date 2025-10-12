/**
 * J1939 Diagnostic Tool - DM (Diagnostic Message) Handler Implementation
 * 
 * Versão: 3.6.0
 */

#include "dm_handler.h"
#include "../core/error_handler.h"
#include "../core/spn_db_handler.h"
#include "../ui/ui_events.h"

// Helper function to parse a single 4-byte DTC from a buffer
DTC parse_single_dtc(const uint8_t* buffer) {
    DTC dtc;
    uint32_t raw_spn_fmi = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16);
    
    dtc.spn = raw_spn_fmi & 0x7FFFF; // 19 bits
    dtc.fmi = (raw_spn_fmi >> 19) & 0x1F; // 5 bits
    
    dtc.cm = (buffer[3] >> 7) & 0x01; // 1 bit
    dtc.oc = buffer[3] & 0x7F; // 7 bits

    return dtc;
}

void dm_parse_dm1(const uint8_t* data, uint16_t length) {
    if (length < 2) return;

    // Bytes 0 and 1 contain the lamp status
    // (We can still log this, but the main goal is to send DTCs to the UI)
    char lamp_msg[100];
    sprintf(lamp_msg, "DM1 Lamp Status - MIL: %d, Red: %d, Amber: %d, Protect: %d", (data[0] >> 4) & 0x03, (data[0] >> 2) & 0x03, data[0] & 0x03, (data[1] >> 6) & 0x03);
    error_report(ErrorLevel::INFO, "DM1", lamp_msg);

    int dtc_count = (length - 2) / 4;
    if (dtc_count == 0) {
        error_report(ErrorLevel::INFO, "DM1", "No active DTCs reported.");
        return;
    }

    for (int i = 0; i < dtc_count; ++i) {
        DTC dtc = parse_single_dtc(&data[2 + i * 4]);
        // Send the parsed DTC to the UI queue
        if (xQueueSend(dtc_queue, &dtc, 0) != pdPASS) {
            error_report(ErrorLevel::WARN, "DM1", "Failed to send DTC to UI queue.");
        }
    }
}

void dm_parse_dm2(const uint8_t* data, uint16_t length) {
    if (length < 2) return;

    char lamp_msg[100];
    sprintf(lamp_msg, "DM2 Lamp Status - MIL: %d, Red: %d, Amber: %d, Protect: %d", (data[0] >> 4) & 0x03, (data[0] >> 2) & 0x03, data[0] & 0x03, (data[1] >> 6) & 0x03);
    error_report(ErrorLevel::INFO, "DM2", lamp_msg);

    int dtc_count = (length - 2) / 4;
    if (dtc_count == 0) {
        error_report(ErrorLevel::INFO, "DM2", "No previously active DTCs reported.");
        return;
    }

    for (int i = 0; i < dtc_count; ++i) {
        DTC dtc = parse_single_dtc(&data[2 + i * 4]);
        // Send the parsed DTC to the UI queue
        if (xQueueSend(dtc_queue, &dtc, 0) != pdPASS) {
            error_report(ErrorLevel::WARN, "DM2", "Failed to send DTC to UI queue.");
        }
    }
}

void dm_parse_dm24(const uint8_t* data, uint16_t length) {
    // ... (implementation of dm_parse_dm24)
}

void dm_parse_dm4(const uint8_t* data, uint16_t length) {
    // ... (implementation of dm_parse_dm4)
}

void dm_parse_dm25(const uint8_t* data, uint16_t length) {
    // DM25 has a more complex format, but for now, we can treat it like a DM4
    // for a basic implementation.
    if (length < 4) return;

    DTC trigger_dtc = parse_single_dtc(data);
    char dtc_msg[200];
    sprintf(dtc_msg, "Expanded Freeze Frame for DTC: SPN %lu, FMI %u", trigger_dtc.spn, trigger_dtc.fmi);
    error_report(ErrorLevel::INFO, "DM25", dtc_msg);

    // This is a major simplification. A real DM25 parser is much more complex.
    int offset = 4;
    while (offset < length) {
        if (offset + 2 > length) break;
        uint16_t spn = data[offset] | (data[offset+1] << 8);
        offset += 2;

        const char* spn_desc = spn_get_description(spn);
        char spn_msg[100];
        sprintf(spn_msg, "  - Captured SPN %u: %s", spn, spn_desc);
        error_report(ErrorLevel::INFO, "DM25", spn_msg);

        offset += 2;
    }
}

void dm_parse_dm11(const uint8_t* data, uint16_t length) {
    if (length < 1) {
        error_report(ErrorLevel::WARN, "DM11", "Invalid payload length.");
        return;
    }
    char msg[100];
    sprintf(msg, "DM11 received. Length: %d", length);
    error_report(ErrorLevel::INFO, "DM11", msg);
}

void dm_parse_dm13(const uint8_t* data, uint16_t length) {
    if (length < 1) {
        error_report(ErrorLevel::WARN, "DM13", "Invalid payload length.");
        return;
    }
    char msg[100];
    sprintf(msg, "DM13 received. Length: %d", length);
    error_report(ErrorLevel::INFO, "DM13", msg);
}

void dm_parse_dm3(const uint8_t* data, uint16_t length) {
    if (length < 1) {
        error_report(ErrorLevel::WARN, "DM3", "Invalid payload length.");
        return;
    }
    char msg[100];
    sprintf(msg, "DM3 received. Length: %d", length);
    error_report(ErrorLevel::INFO, "DM3", msg);
}
