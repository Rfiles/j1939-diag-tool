/**
 * J1939 Diagnostic Tool - Live Data Screen
 * 
 * Versão: 4.3.0
 */

#include "live_data_screen.h"
#include "../ui_manager.h"
#include "../../j1939/src/j1939_handler.h"
#include "../../ui/driver/display/st7789_driver.h"
#include "../../ui/driver/button_driver/button_driver.h"

LiveDataScreen::LiveDataScreen(const VehicleEcu* ecu) : ecu(ecu) {
    title_bar.set_title("LIVE DATA");
}

void LiveDataScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text(ecu->name.c_str(), 120, 40, COLOR_WHITE, COLOR_BLACK);

    // Request PGNs
    for (const auto& pgn : ecu->pgns_of_interest) {
        j1939_request_pgn(pgn, ecu->source_address);
    }
}

void LiveDataScreen::on_exit() {}

void LiveDataScreen::handle_input() {
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_SELECT) {
            UIManager::getInstance().pop_screen();
        }
    }
}

void LiveDataScreen::draw() {
    title_bar.draw();
    // TODO: Draw live data
}

void ui_show_ecu_data(const VehicleEcu* ecu) {
    UIManager::getInstance().push_screen(std::make_shared<LiveDataScreen>(ecu));
}