#include "live_data_screen.h"
#include "../ui_manager.h"
#include "../../j1939/j1939_handler.h"
#include "../../driver/display/st7789_driver.h"

extern UIManager ui_manager;

LiveDataScreen::LiveDataScreen(const Ecu* ecu) : ecu(ecu) {}

void LiveDataScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("LIVE DATA", 120, 20, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text(ecu->name.c_str(), 120, 40, COLOR_WHITE, COLOR_BLACK);

    // Request PGNs
    for (const auto& pgn : ecu->pgns) {
        j1939_request_pgn(pgn);
    }
}

void LiveDataScreen::on_exit() {}

void LiveDataScreen::handle_input() {
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_BACK) {
            ui_manager.pop_screen();
        }
    }
}

void LiveDataScreen::draw() {
    title_bar.draw();
    // TODO: Draw live data
}

void ui_show_ecu_data(const Ecu* ecu) {
    ui_manager.push_screen(std::make_shared<LiveDataScreen>(ecu));
}
