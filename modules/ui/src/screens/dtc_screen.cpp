/**
 * J1939 Diagnostic Tool - DTC Screen
 * 
 * Versão: 4.3.0
 */

#include "dtc_screen.h"
#include "../../ui/driver/button_driver/button_driver.h"
#include "../ui_manager.h"
#include "../../ui/driver/display/st7789_driver.h"
#include "../../j1939/src/spn_db_handler.h"
#include "../../j1939/src/fmi_db_handler.h"


QueueHandle_t dtc_queue;

DtcScreen::DtcScreen() {
    dtc_queue = xQueueCreate(10, sizeof(DTC));
    title_bar.set_title("DTC CODES");
}

void DtcScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
}

void DtcScreen::on_exit() {}

void DtcScreen::handle_input() {
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_SELECT) {
            UIManager::getInstance().pop_screen();
        }
    }

    DTC dtc;
    if (xQueueReceive(dtc_queue, &dtc, 0) == pdPASS) {
        dtcs.push_back(dtc);
    }
}

void DtcScreen::draw() {
    title_bar.draw();
    int y = 40;
    for (const auto& dtc : dtcs) {
        char buffer[100];
        const char* spn_desc = spn_get_description(dtc.spn);
        const char* fmi_desc = fmi_get_description(dtc.fmi);
        snprintf(buffer, sizeof(buffer), "SPN: %d, FMI: %d - %s, %s", dtc.spn, dtc.fmi, spn_desc, fmi_desc);
        st7789_draw_text(buffer, 10, y, COLOR_WHITE, COLOR_BLACK);
        y += 15;
    }
}