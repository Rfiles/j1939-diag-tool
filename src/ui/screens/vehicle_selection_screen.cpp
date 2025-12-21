#include "vehicle_selection_screen.h"
#include "../../driver/button_driver/button_driver.h"
#include "../ui_manager.h"
#include "../../core/vehicle_db_handler.h"
#include "../../driver/display/st7789_driver.h"
#include "../../core/error_handler.h"



VehicleSelectionScreen::VehicleSelectionScreen() : menu({}) {
    const auto& models = vehicle_db_get_all_models();
    std::vector<std::string> model_names;
    for (const auto& model : models) {
        model_names.push_back(model.model_name.c_str());
    }
    menu = Menu(model_names);
}

void VehicleSelectionScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("SELECIONAR VIATURA", 120, 20, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text("--------------------", 120, 40, COLOR_WHITE, COLOR_BLACK);
}

void VehicleSelectionScreen::on_exit() {
}

void VehicleSelectionScreen::handle_input() {
    menu.handle_input();
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_SELECT) {
            int selected = menu.get_selected_item();
            const auto& models = vehicle_db_get_all_models();
            if (selected >= 0 && selected < models.size()) {
                vehicle_db_set_active_model(models[selected].model_name.c_str());
                char msg[100];
                sprintf(msg, "Selected vehicle: %s", models[selected].model_name.c_str());
                error_report(ErrorLevel::INFO, "UI", msg);
            }
        }
    }
}

void VehicleSelectionScreen::draw() {
    title_bar.draw();
    menu.draw();
}
