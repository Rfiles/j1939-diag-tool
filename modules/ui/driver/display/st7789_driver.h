/**
 * @file st7789_driver.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - ST7789 Display Driver Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ST7789_DRIVER_H
#define ST7789_DRIVER_H

#include <stdint.h>

/**
 * @brief Basic 16-bit color definitions (RGB565).
 */
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFDA0

/**
 * @brief Initialize the display.
 */
void st7789_init();

/**
 * @brief Fill the screen with a color.
 * 
 * @param color The 16-bit color to fill the screen with.
 */
void st7789_fill_screen(uint16_t color);

/**
 * @brief Draw a single line of text.
 * 
 * @note This is a very basic implementation with a fixed font.
 * 
 * @param text The text to draw.
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param color The 16-bit color of the text.
 * @param bg_color The 16-bit color of the background.
 */
void st7789_draw_text(const char* text, int16_t x, int16_t y, uint16_t color, uint16_t bg_color);

/**
 * @brief Draw a filled rectangle.
 * 
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The 16-bit color to fill the rectangle with.
 */
void st7789_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


/**
 * @brief Set the display brightness.
 * 
 * @param value The brightness value (0-255).
 */
void st7789_set_brightness(uint8_t value);

#endif // ST7789_DRIVER_H