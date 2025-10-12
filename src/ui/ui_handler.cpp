/**
 * J1939 Diagnostic Tool - UI Handler with Advanced Live Data
 * 
 * Versão: 3.18.0
 */

#include "ui_handler.h"
#include "ui_events.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include <map>
#include "../core/config.h"
#include "../core/spn_db_handler.h"
#include "../core/fmi_db_handler.h"
#include "../j1939/j1939_handler.h"
#include "../j1939/spn_parser.h"
#include "../j1939/j1939_pgn_definitions.h"
#include "../driver/button_driver/button_driver.h"
#include "../driver/display/st7789_driver.h"

// ... (UI state and data variables) ...

// --- Drawing Functions ---

void draw_live_data_screen(const std::vector<uint32_t>& spns_to_draw) {
    int y = 25;
    for (uint32_t spn : spns_to_draw) {
        auto pgn_it = spn_to_pgn_map.find(spn);
        if (pgn_it == spn_to_pgn_map.end()) continue;
        uint32_t pgn = pgn_it->second;

        auto frame_it = live_pgn_map.find(pgn);
        const char* spn_desc = spn_get_description(spn);
        char buf[128];

        if (frame_it != live_pgn_map.end()) {
            ParsedSPN parsed = spn_parse(frame_it->second, spn);
            if (parsed.is_valid) {
                sprintf(buf, "%s: %.1f %s", spn_desc, parsed.value, parsed.units.c_str());
                st7789_draw_text(buf, 5, y, COLOR_WHITE, COLOR_BLACK);
            } else {
                sprintf(buf, "%s: N/A", spn_desc);
                st7789_draw_text(buf, 5, y, COLOR_GRAY, COLOR_BLACK);
            }
        } else {
            sprintf(buf, "%s: --", spn_desc);
            st7789_draw_text(buf, 5, y, COLOR_GRAY, COLOR_BLACK);
        }
        y += 15;
        if (y > 160) break; // Don't draw off screen
    }
}

void draw_generic_live_data() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("Live Data (Generic)", 5, 5, COLOR_YELLOW, COLOR_BLACK);
    // Draw a predefined list of common SPNs
    const std::vector<uint32_t> generic_spns = {190, 110, 100, 84, 96, 168};
    draw_live_data_screen(generic_spns);
}

void draw_ecu_live_data() {
    st7789_fill_screen(COLOR_BLACK);
    if (active_ecu == nullptr) {
        st7789_draw_text("No ECU Selected", 5, 5, COLOR_RED, COLOR_BLACK);
        return;
    }
    char title[50];
    sprintf(title, "Live Data: %s", active_ecu->name.c_str());
    st7789_draw_text(title, 5, 5, COLOR_YELLOW, COLOR_BLACK);
    
    // Get SPNs from the active ECU's PGNs of interest
    std::vector<uint32_t> ecu_spns;
    for (uint32_t pgn : active_ecu->pgns_of_interest) {
        auto it = pgn_to_spns_map.find(pgn);
        if (it != pgn_to_spns_map.end()) {
            ecu_spns.insert(ecu_spns.end(), it->second.begin(), it->second.end());
        }
    }
    draw_live_data_screen(ecu_spns);
}

// ... (other draw functions) ...

// --- Main UI Task ---
void ui_task_fn(void* pv) {
    // ...
    for (;;) {
        // ...
        if (screen_dirty) {
            if (current_state == UI_STATE_LIVE_DATA) draw_generic_live_data();
            // ...
            else if (current_state == UI_STATE_ECU_LIVE_DATA) draw_ecu_live_data();
            screen_dirty = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ... (init and other functions) ...
