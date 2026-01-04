/**
 * @file j1939_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - J1939 Handler Task Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef J1939_HANDLER_H
#define J1939_HANDLER_H

#include <Arduino.h>
#include "j1939_pgn.h"
#include "../../can/src/mcp2515_driver.h"

/**
 * @brief Enum for the state of the J1939 Address Claiming process.
 */
enum J1939_ADDRESS_CLAIM_STATE {
    AC_STATE_UNCLAIMED,
    AC_STATE_CLAIMING,
    AC_STATE_CLAIMED,
    AC_STATE_CANNOT_CLAIM
};

/**
 * @brief Struct for a J1939 transmit request.
 */
struct J1939TxRequest {
    uint32_t pgn;
    uint8_t dest_address;
};

/**
 * @brief Queue for J1939 transmit requests.
 */
extern QueueHandle_t j1939_tx_queue;

/**
 * @brief Queue for received J1939 messages.
 */
extern QueueHandle_t j1939_rx_queue;

/**
 * @brief Initializes the J1939 handler and creates the task.
 */
void j1939_handler_init();

/**
 * @brief Requests a PGN from a specific destination address.
 * 
 * @param pgn The PGN to request.
 * @param dest_address The destination address.
 */
void j1939_request_pgn(uint32_t pgn, uint8_t dest_address);


/**
 * @brief Gets the current, claimed J1939 source address of this device.
 * 
 * @return The current source address (0-253), or 254 if unable to claim an address.
 */
uint8_t j1939_get_source_address();

/**
 * @brief Gets the timestamp of the last received J1939 message.
 * 
 * @return The timestamp of the last received message, in milliseconds.
 */
unsigned long j1939_get_last_rx_time();

#endif // J1939_HANDLER_H
