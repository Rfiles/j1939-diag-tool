/**
 * J1939 Diagnostic Tool - ST7789 Display Driver Implementation (from scratch)
 * 
 * Versão: 2.1.0
 */

#include "st7789_driver.h"
#include "../../core/config.h"
#include <SPI.h>

// A very simple 5x7 font
#include "simple_font.h"

// Use a dedicated SPIClass object if using non-default host
// static SPIClass spi(config.display.spi_host);

static SPISettings spi_settings(40000000, MSBFIRST, SPI_MODE0); // 40MHz SPI

// --- Internal Functions ---

void write_command(uint8_t cmd) {
    digitalWrite(config.display.dc_pin, LOW);
    digitalWrite(config.display.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    SPI.transfer(cmd);
    SPI.endTransaction();
    digitalWrite(config.display.cs_pin, HIGH);
}

void write_data(uint8_t data) {
    digitalWrite(config.display.dc_pin, HIGH);
    digitalWrite(config.display.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    SPI.transfer(data);
    SPI.endTransaction();
    digitalWrite(config.display.cs_pin, HIGH);
}

void set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    write_command(0x2A); // Column Address Set
    write_data(x0 >> 8);
    write_data(x0 & 0xFF);
    write_data(x1 >> 8);
    write_data(x1 & 0xFF);

    write_command(0x2B); // Row Address Set
    write_data(y0 >> 8);
    write_data(y0 & 0xFF);
    write_data(y1 >> 8);
    write_data(y1 & 0xFF);

    write_command(0x2C); // Memory Write
}

#include <Arduino.h>

// LEDC channels for PWM backlight control
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_8_BIT 8
#define LEDC_BASE_FREQ 5000

// --- Public API Implementation ---

void st7789_init() {
    pinMode(config.display.cs_pin, OUTPUT);
    pinMode(config.display.dc_pin, OUTPUT);
    pinMode(config.display.rst_pin, OUTPUT);

    // Setup PWM for backlight
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT);
    ledcAttachPin(config.display.bl_pin, LEDC_CHANNEL_0);

    digitalWrite(config.display.cs_pin, HIGH);
    st7789_set_brightness(config.display.display_brightness);

    // Reset the display
    digitalWrite(config.display.rst_pin, HIGH);
    delay(50);
    digitalWrite(config.display.rst_pin, LOW);
    delay(50);
    digitalWrite(config.display.rst_pin, HIGH);
    delay(150);

    SPI.begin(config.display.sclk_pin, -1, config.display.mosi_pin, -1);

    write_command(0x11); // Sleep Out
    delay(120);

    write_command(0x36); // Memory Data Access Control
    write_data(0x00);    // Default orientation

    write_command(0x3A); // Pixel Format Set
    write_data(0x55);    // 16 bits per pixel

    write_command(0x29); // Display ON
    delay(120);
}

void st7789_set_brightness(uint8_t value) {
    ledcWrite(LEDC_CHANNEL_0, value);
}

void st7789_fill_screen(uint16_t color) {
    set_addr_window(0, 0, 320 - 1, 170 - 1);
    digitalWrite(config.display.dc_pin, HIGH);
    digitalWrite(config.display.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    for (long i = 0; i < 320 * 170; i++) {
        SPI.transfer16(color);
    }
    SPI.endTransaction();
    digitalWrite(config.display.cs_pin, HIGH);
}

void st7789_draw_char(char c, int16_t x, int16_t y, uint16_t color, uint16_t bg_color) {
    if (c < 32 || c > 127) return;
    c -= 32;
    set_addr_window(x, y, x + 4, y + 6);
    digitalWrite(config.display.dc_pin, HIGH);
    digitalWrite(config.display.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    for (int i = 0; i < 5; i++) { // 5 columns
        uint8_t line = font[c][i];
        for (int j = 0; j < 7; j++) { // 7 rows
            if ((line >> j) & 1) {
                SPI.transfer16(color);
            } else {
                SPI.transfer16(bg_color);
            }
        }
    }
    SPI.endTransaction();
    digitalWrite(config.display.cs_pin, HIGH);
}

void st7789_draw_text(const char* text, int16_t x, int16_t y, uint16_t color, uint16_t bg_color) {
    while (*text) {
        st7789_draw_char(*text, x, y, color, bg_color);
        x += 6; // 5 pixels for char + 1 for space
        if (x > 320 - 5) { // Wrap text
            x = 0;
            y += 8;
        }
        text++;
    }
}

void st7789_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    set_addr_window(x, y, x + w - 1, y + h - 1);
    digitalWrite(config.display.dc_pin, HIGH);
    digitalWrite(config.display.cs_pin, LOW);
    SPI.beginTransaction(spi_settings);
    for (long i = 0; i < w * h; i++) {
        SPI.transfer16(color);
    }
    SPI.endTransaction();
    digitalWrite(config.display.cs_pin, HIGH);
}