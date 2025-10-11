/**
 * J1939 Diagnostic Tool - PDU Processor Task Implementation
 * 
 * Versão: 3.0.0
 */

#include "pdu_processor.h"
#include "tp_handler.h"
#include "../core/error_handler.h"
#include <Arduino.h>

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

            // TODO: Add parsing logic for DM1, DM2, etc. based on PGN
            if (pdu.pgn == 65226) { // DM1
                // Parse DM1 message
            } else if (pdu.pgn == 65227) { // DM2
                // Parse DM2 message
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
