#include "vehicle_selection_screen.h"
#include "../../ui/driver/button_driver/button_driver.h"
#include "../ui_manager.h"
#include "../../j1939/src/vehicle_db_handler.h"
#include "../../ui/driver/display/st7789_driver.h"
#include "../../core/error_handler.h"



VehicleSelectionScreen::VehicleSelectionScreen() : menu({}) {
    const auto& models = vehicle_db_get_all_models();
    std::vector<std::string> model_names;
    for (const auto& model : models) {
        model_names.push_back(model.model_name.c_str());
    }
    menu = Menu(model_names);
    title_bar.set_title("SELECIONAR VIATURA");
}

void VehicleSelectionScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
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