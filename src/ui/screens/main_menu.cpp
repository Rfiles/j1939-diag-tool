#include "main_menu.h"
#include "../ui_manager.h"
#include "../../driver/display/st7789_driver.h"
#include "vehicle_selection_screen.h"
#include "configuration_screen.h"
#include "shutdown_screen.h"

extern UIManager ui_manager;

MainMenuScreen::MainMenuScreen() : menu({"VIATURA", "CONFIGURACOES", "DESLIGAR"}) {}

void MainMenuScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("MENU PRINCIPAL", 120, 20, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text("--------------------", 120, 40, COLOR_WHITE, COLOR_BLACK);
}

void MainMenuScreen::on_exit() {
}

void MainMenuScreen::handle_input() {
    menu.handle_input();
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_SELECT) {
            int selected = menu.get_selected_item();
            if (selected == 0) {
                ui_manager.push_screen(std::make_shared<VehicleSelectionScreen>());
            } else if (selected == 1) {
                ui_manager.push_screen(std::make_shared<ConfigurationScreen>());
            } else if (selected == 2) {
                ui_manager.push_screen(std::make_shared<ShutdownScreen>());
            }
        }
    }
}

void MainMenuScreen::draw() {
    title_bar.draw();
    menu.draw();
}
