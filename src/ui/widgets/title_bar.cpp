#include "title_bar.h"
#include "../../core/time_handler.h"
#include "../../j1939/j1939_handler.h"
#include "../../driver/display/st7789_driver.h"
#include <Arduino.h>

void TitleBar::draw() {
    // Draw time
    String time_str = time_handler_get_timestamp();
    st7789_draw_text(time_str.c_str(), 200, 5, COLOR_WHITE, COLOR_BLACK);

    // Draw CAN status icon
    unsigned long last_rx = j1939_get_last_rx_time();
    unsigned long now = millis();
    uint16_t color;

    if (last_rx == 0) {
        color = COLOR_RED;
    } else if (now - last_rx > 5000) {
        color = COLOR_RED;
    } else if (now - last_rx > 1000) {
        color = COLOR_YELLOW;
    } else {
        color = COLOR_GREEN;
    }

    // Draw a small rectangle as the icon
    st7789_fill_rect(5, 5, 10, 10, color);
}