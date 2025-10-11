/**
 * J1939 Diagnostic Tool - MCP2515 Driver Interface
 * 
 * Versão: 2.1.0
 */

#ifndef MCP2515_DRIVER_H
#define MCP2515_DRIVER_H

#include <stdint.h>

// A generic CAN frame structure
struct can_frame {
    uint32_t can_id;  // 29-bit CAN ID
    uint8_t  can_dlc; // Data Length Code
    uint8_t  data[8];
};

// Initialize the MCP2515
bool mcp2515_init();

// Send a CAN frame
bool mcp2515_transmit(const can_frame* frame);

// Receive a CAN frame
bool mcp2515_receive(can_frame* frame);

#endif // MCP2515_DRIVER_H
