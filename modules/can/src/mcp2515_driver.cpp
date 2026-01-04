/**
 * J1939 Diagnostic Tool - MCP2515 Driver Implementation (from scratch)
 * 
 * Versão: 2.1.0
 */

#include "mcp2515_driver.h"
#include "../core/config.h"
#include <SPI.h>

// MCP2515 SPI Commands
#define MCP_RESET       0xC0
#define MCP_READ        0x03
#define MCP_WRITE       0x02
#define MCP_READ_STATUS 0xA0
#define MCP_RX_STATUS   0xB0
#define MCP_BITMOD      0x05

// MCP2515 Registers
#define MCP_CANSTAT     0x0E
#define MCP_CANCTRL     0x0F
#define MCP_CNF1        0x2A
#define MCP_CNF2        0x29
#define MCP_CNF3        0x28
#define MCP_TXB0CTRL    0x30
#define MCP_RXB0CTRL    0x60
#define MCP_RXF0SIDH    0x00

static SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0); // 10MHz SPI

// --- Internal Functions ---

void mcp_write_register(uint8_t reg, uint8_t value) {
    digitalWrite(config.mcp2515.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    SPI.transfer(MCP_WRITE);
    SPI.transfer(reg);
    SPI.transfer(value);
    SPI.endTransaction();
    digitalWrite(config.mcp2515.cs_pin, HIGH);
}

uint8_t mcp_read_register(uint8_t reg) {
    uint8_t value;
    digitalWrite(config.mcp2515.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    SPI.transfer(MCP_READ);
    SPI.transfer(reg);
    value = SPI.transfer(0x00); // Send dummy byte to receive data
    SPI.endTransaction();
    digitalWrite(config.mcp2515.cs_pin, HIGH);
    return value;
}

void mcp_reset() {
    digitalWrite(config.mcp2515.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    SPI.transfer(MCP_RESET);
    SPI.endTransaction();
    digitalWrite(config.mcp2515.cs_pin, HIGH);
    delay(10);
}

// --- Public API Implementation ---

bool mcp2515_init() {
    pinMode(config.mcp2515.cs_pin, OUTPUT);
    digitalWrite(config.mcp2515.cs_pin, HIGH);

    SPI.begin(config.mcp2515.sclk_pin, config.mcp2515.miso_pin, config.mcp2515.mosi_pin, -1);

    mcp_reset();

    // Set CAN speed to 250kbps for 16MHz crystal
    // CNF1 = 0x03 (SJW=1, BRP=3)
    // CNF2 = 0xB1 (BTLMODE=1, SAM=1, PHSEG1=2, PRSEG=1)
    // CNF3 = 0x05 (PHSEG2=5)
    mcp_write_register(MCP_CNF1, 0x03);
    mcp_write_register(MCP_CNF2, 0xB1);
    mcp_write_register(MCP_CNF3, 0x05);

    // Enable receiving extended frames, disable masks/filters
    mcp_write_register(MCP_RXB0CTRL, 0x64); // 01100100 - Receive all, no filters

    // Set mode to Normal
    mcp_write_register(MCP_CANCTRL, 0x00);

    // Verify mode change
    if ((mcp_read_register(MCP_CANSTAT) & 0xE0) != 0x00) {
        return false; // Failed to enter normal mode
    }

    return true;
}

bool mcp2515_transmit(const can_frame* frame) {
    // Check if transmit buffer 0 is free
    if ((mcp_read_register(MCP_TXB0CTRL) & 0x08) == 0) {
        // Load the frame into TXB0
        mcp_write_register(0x31, (frame->can_id >> 21));
        mcp_write_register(0x32, (((frame->can_id >> 13) & 0xE0) | (1 << 3) | ((frame->can_id >> 16) & 0x03)));
        mcp_write_register(0x33, (frame->can_id >> 8));
        mcp_write_register(0x34, (frame->can_id & 0xFF));
        mcp_write_register(0x35, frame->can_dlc);
        for (int i = 0; i < frame->can_dlc; i++) {
            mcp_write_register(0x36 + i, frame->data[i]);
        }

        // Request to send
        mcp_write_register(MCP_TXB0CTRL, 0x08);
        return true;
    }
    return false; // Buffer was not free
}

bool mcp2515_receive(can_frame* frame) {
    // Check if a message is available in RXB0
    if ((mcp_read_register(MCP_READ_STATUS) & 0x01) == 0x01) {
        // Read CAN ID
        uint32_t id_h = mcp_read_register(0x61);
        uint32_t id_m = mcp_read_register(0x62);
        uint32_t id_l = mcp_read_register(0x64);
        frame->can_id = (id_h << 21) | ((id_m & 0xE0) << 13) | ((id_m & 0x03) << 16) | (mcp_read_register(0x63) << 8) | id_l;

        // Read DLC
        frame->can_dlc = mcp_read_register(0x65) & 0x0F;

        // Read data
        for (int i = 0; i < frame->can_dlc; i++) {
            frame->data[i] = mcp_read_register(0x66 + i);
        }
        return true;
    }
    return false; // No message available
}
