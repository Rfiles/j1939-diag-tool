/**
 * J1939 Diagnostic Tool - UI Handler Task Implementation
 * 
 * Versão: 2.1.0
 */

#include "ui_handler.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include <map>
#include "../core/config.h"
#include "../j1939/j1939_handler.h"
#include "../driver/button_driver/button_driver.h"
#include "../driver/display/st7789_driver.h"

// --- UI State and Data ---
typedef enum {
    UI_STATE_LIVE_DATA,
    UI_STATE_MAIN_MENU,
} ui_state_t;

static ui_state_t current_state = UI_STATE_LIVE_DATA;
static std::map<uint32_t, can_frame> live_data_map;
static std::vector<std::string> menu_items = {"Live Data", "Clear Data"};
static int selected_menu_item = 0;
static bool screen_dirty = true;

// --- Drawing Functions ---
void draw_live_data() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("Live J1939 Data (v2.1)", 5, 5, COLOR_YELLOW, COLOR_BLACK);
    int y = 25;
    if (live_data_map.empty()) {
        st7789_draw_text("Waiting for data...", 5, y, COLOR_WHITE, COLOR_BLACK);
    } else {
        for (auto const& [pgn, frame] : live_data_map) {
            char buf[40];
            sprintf(buf, "PGN %lu: %02X %02X %02X %02X", pgn, frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
            st7789_draw_text(buf, 5, y, COLOR_WHITE, COLOR_BLACK);
            y += 15;
            if (y > 160) break;
        }
    }
}

void draw_main_menu() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("Main Menu", 5, 5, COLOR_YELLOW, COLOR_BLACK);
    for (size_t i = 0; i < menu_items.size(); i++) {
        uint16_t color = (i == selected_menu_item) ? COLOR_GREEN : COLOR_WHITE;
        st7789_draw_text(menu_items[i].c_str(), 20, 40 + i * 20, color, COLOR_BLACK);
    }
}

// --- Main UI Task ---
void ui_task_fn(void* pv) {
    can_frame rx_frame;
    button_event_t button_event;

    for (;;) {
        // 1. Check for incoming J1939 data (non-blocking)
        if (xQueueReceive(j1939_rx_queue, &rx_frame, 0) == pdPASS) {
            uint32_t pgn = (rx_frame.can_id >> 8) & 0x1FFFF;
            live_data_map[pgn] = rx_frame;
            if (current_state == UI_STATE_LIVE_DATA) screen_dirty = true;
        }

        // 2. Check for button events (non-blocking)
        if (xQueueReceive(button_event_queue, &button_event, 0) == pdPASS) {
            if (current_state == UI_STATE_MAIN_MENU) {
                switch(button_event) {
                    case BTN_PRESS_NEXT:
                        selected_menu_item = (selected_menu_item + 1) % menu_items.size();
                        break;
                    case BTN_PRESS_SELECT:
                        if (menu_items[selected_menu_item] == "Live Data") current_state = UI_STATE_LIVE_DATA;
                        if (menu_items[selected_menu_item] == "Clear Data") { live_data_map.clear(); current_state = UI_STATE_LIVE_DATA; }
                        break;
                    default: break;
                }
            } else {
                // In any other screen, a long press of select returns to the menu
                if (button_event == BTN_LONG_PRESS_SELECT) {
                    current_state = UI_STATE_MAIN_MENU;
                }
            }
            screen_dirty = true;
        }

        // 3. Render screen if needed
        if (screen_dirty) {
            if (current_state == UI_STATE_LIVE_DATA) draw_live_data();
            else if (current_state == UI_STATE_MAIN_MENU) draw_main_menu();
            screen_dirty = false;
        }

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

// --- Public API ---
void ui_handler_init() {
    st7789_init();
    button_driver_init(); // Initialize the separate button driver
    xTaskCreate(ui_task_fn, "UI Task", 8192, NULL, 3, NULL);
}
