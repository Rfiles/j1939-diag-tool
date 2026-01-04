/**
 * @file mcp2515_driver.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - MCP2515 Driver Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MCP2515_DRIVER_H
#define MCP2515_DRIVER_H

#include <stdint.h>

/**
 * @brief A generic CAN frame structure.
 * 
 * This structure holds the information for a single CAN frame, including the
 * CAN ID, Data Length Code (DLC), and the data payload.
 */
struct can_frame {
    uint32_t can_id;  /**< 29-bit CAN ID */
    uint8_t  can_dlc; /**< Data Length Code (0-8) */
    uint8_t  data[8]; /**< Data payload */
};

/**
 * @brief Initialize the MCP2515 CAN controller.
 * 
 * This function initializes the SPI communication with the MCP2515 and
 * configures it for J1939 communication (250kbps, 29-bit identifiers).
 * 
 * @return true if initialization is successful, false otherwise.
 */
bool mcp2515_init();

/**
 * @brief Transmit a CAN frame.
 * 
 * This function sends a CAN frame to the CAN bus via the MCP2515.
 * 
 * @param frame A pointer to the can_frame structure to be transmitted.
 * @return true if the frame is successfully queued for transmission, false otherwise.
 */
bool mcp2515_transmit(const can_frame* frame);

/**
 * @brief Receive a CAN frame.
 * 
 * This function checks for and receives a CAN frame from the MCP2515.
 * 
 * @param frame A pointer to a can_frame structure where the received frame will be stored.
 * @return true if a frame is successfully received, false if no frame is available.
 */
bool mcp2515_receive(can_frame* frame);

#endif // MCP2515_DRIVER_H