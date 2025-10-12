/**
 * J1939 Diagnostic Tool - ST7789 Display Driver Interface
 * 
 * Versão: 2.1.0
 */

#ifndef ST7789_DRIVER_H
#define ST7789_DRIVER_H

#include <stdint.h>

// Basic 16-bit color definitions (RGB565)
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0

// Initialize the display
void st7789_init();

// Fill the screen with a color
void st7789_fill_screen(uint16_t color);

// Draw a single line of text
// Note: This is a very basic implementation with a fixed font.
void st7789_draw_text(const char* text, int16_t x, int16_t y, uint16_t color, uint16_t bg_color);

// Draw a filled rectangle
void st7789_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


#endif // ST7789_DRIVER_H
