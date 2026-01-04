/**
 * J1939 Diagnostic Tool - Title Bar Widget
 * 
 * Versão: 4.3.0
 */

#include "title_bar.h"
#include "../../time/src/time_handler.h"
#include "../../j1939/src/j1939_handler.h"
#include "../../gps/src/gps_handler.h"
#include "../../wifi/src/comms_handler.h"
#include "../../ui/driver/display/st7789_driver.h"
#include <Arduino.h>

void TitleBar::set_title(const std::string& title) {
    this->title = title;
}

void TitleBar::draw() {
    // Clear the title bar area
    st7789_fill_rect(0, 0, 320, 20, COLOR_BLACK);

    // Draw title
    st7789_draw_text(title.c_str(), 50, 5, COLOR_WHITE, COLOR_BLACK);

    draw_can_status();
    draw_wifi_status();
    draw_gps_status();

    // Draw time
    String time_str = time_handler_get_timestamp();
    st7789_draw_text(time_str.c_str(), 200, 5, COLOR_WHITE, COLOR_BLACK);
}

void TitleBar::draw_can_status() {
    unsigned long last_rx = j1939_get_last_rx_time();
    unsigned long now = millis();
    uint16_t color;
    bool blink = false;

    if (last_rx == 0 || now - last_rx > 5000) {
        color = COLOR_RED;
    } else if (now - last_rx > 1000) {
        color = COLOR_ORANGE;
    } else {
        color = COLOR_GREEN;
        // Blink if receiving data
        if ((now / 250) % 2 == 0) {
            blink = true;
        }
    }

    st7789_fill_rect(5, 5, 10, 10, blink ? COLOR_BLACK : color);
}

void TitleBar::draw_wifi_status() {
    WiFiStatus status = comms_get_wifi_status();
    uint16_t color;
    bool blink = false;

    switch (status) {
        case WIFI_STATUS_DISCONNECTED:
            color = COLOR_RED;
            break;
        case WIFI_STATUS_CONNECTING:
            color = COLOR_ORANGE;
            if ((millis() / 500) % 2 == 0) {
                blink = true;
            }
            break;
        case WIFI_STATUS_CONNECTED:
            color = COLOR_ORANGE;
            break;
        case WIFI_STATUS_INTERNET:
            color = COLOR_GREEN;
            break;
    }

    st7789_fill_rect(20, 5, 10, 10, blink ? COLOR_BLACK : color);
}

void TitleBar::draw_gps_status() {
    GpsFixStatus status = gps_get_status();
    uint16_t color;

    switch (status) {
        case GPS_FIX_NONE:
            color = COLOR_RED;
            break;
        case GPS_FIX_2D:
            color = COLOR_ORANGE;
            break;
        case GPS_FIX_3D:
            color = COLOR_GREEN;
            break;
    }

    st7789_fill_rect(35, 5, 10, 10, color);
}
