/**
 * @file error_screen.cpp
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Error Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "error_screen.h"
#include "../ui_manager.h"
#include "../../ui/driver/display/st7789_driver.h"
#include "../../ui/driver/button_driver/button_driver.h"
#include <Arduino.h>

ErrorScreen::ErrorScreen(const std::string& title, const std::string& message)
    : title(title), message(message) {
    title_bar.set_title(title);
}

void ErrorScreen::handle_input() {
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, pdMS_TO_TICKS(10)) == pdPASS) {
        if (event == BTN_PRESS_SELECT) {
            // This will just pop the screen, allowing the app to continue.
            UIManager::getInstance().pop_screen();
        }
    }
}

void ErrorScreen::draw() {
    title_bar.draw();
    st7789_draw_text(message.c_str(), 10, 50, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text("Press SELECT to continue", 10, 100, COLOR_GREEN, COLOR_BLACK);
}

void ErrorScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    // Clear the button event queue to avoid processing old presses
    xQueueReset(button_event_queue);
}

void ErrorScreen::on_exit() {
    // Nothing to do
}
