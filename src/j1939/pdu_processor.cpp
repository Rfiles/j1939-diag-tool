/**
 * J1939 Diagnostic Tool - PDU Processor Task Implementation
 * 
 * Versão: 3.0.0
 */

#include "pdu_processor.h"
#include "tp_handler.h"
#include "../core/error_handler.h"
#include "dm_handler.h"

// This task waits for fully reassembled PDUs and processes them.
// For now, it just prints them. In the future, it will parse DM1/DM2 etc.
void pdu_processor_task_fn(void* pv) {
    ReassembledPDU pdu;

    for (;;) {
        // Wait indefinitely for a message
        if (xQueueReceive(j1939_complete_pdu_queue, &pdu, portMAX_DELAY) == pdPASS) {
            char msg[100];
            sprintf(msg, "Received complete PDU. PGN: %lu, Src: %d, Len: %d", pdu.pgn, pdu.src_addr, pdu.data_length);
            error_report(ErrorLevel::INFO, "PDU_Proc", msg);

            if (pdu.pgn == 65226) { // DM1
                dm_parse_dm1(pdu.data, pdu.data_length);
            } else if (pdu.pgn == 65227) { // DM2
                dm_parse_dm2(pdu.data, pdu.data_length);
            } else if (pdu.pgn == 65238) { // DM24
                dm_parse_dm24(pdu.data, pdu.data_length);
            } else if (pdu.pgn == 65229) { // DM4
                dm_parse_dm4(pdu.data, pdu.data_length);
            } else if (pdu.pgn == 64951) { // DM25
                dm_parse_dm25(pdu.data, pdu.data_length);
            } else if (pdu.pgn == 65260) { // Vehicle Identification (VIN)
                // The VIN is a string of ASCII characters, terminated by a '*'
                char vin_buffer[30]; // VIN is typically 17 chars
                int len = 0;
                for (int i = 0; i < pdu.data_length && i < sizeof(vin_buffer) - 1; ++i) {
                    if (pdu.data[i] == '*') {
                        break;
                    }
                    vin_buffer[i] = (char)pdu.data[i];
                    len++;
                }
                vin_buffer[len] = '\0';
                char vin_msg[50];
                sprintf(vin_msg, "Vehicle VIN: %s", vin_buffer);
                error_report(ErrorLevel::INFO, "VIN", vin_msg);
            }
        }
    }
}

void pdu_processor_init() {
    xTaskCreate(
        pdu_processor_task_fn,
        "PDU Processor Task",
        4096, // Stack size
        NULL,
        4,    // Normal priority
        NULL
    );
}
