#include "main_menu.h"
#include "../../driver/button_driver/button_driver.h"
#include <Arduino.h>
#include "../ui_manager.h"
#include "../../driver/display/st7789_driver.h"
#include "vehicle_selection_screen.h"
#include "configuration_screen.h"
#include "shutdown_screen.h"

MainMenuScreen::MainMenuScreen() : menu({"VIATURA", "CONFIGURACOES", "DESLIGAR"}) {
    title_bar.set_title("MENU PRINCIPAL");
}

void MainMenuScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
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
                UIManager::getInstance().push_screen(std::make_shared<VehicleSelectionScreen>());
            } else if (selected == 1) {
                UIManager::getInstance().push_screen(std::make_shared<ConfigurationScreen>());
            } else if (selected == 2) {
                UIManager::getInstance().push_screen(std::make_shared<ShutdownScreen>());
            }
        }
    }
}

void MainMenuScreen::draw() {
    title_bar.draw();
    menu.draw();
}