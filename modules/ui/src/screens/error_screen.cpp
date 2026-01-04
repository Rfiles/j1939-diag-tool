'''
#include "error_screen.h"
#include "../ui_manager.h"
#include "../../driver/display/st7789_driver.h"
#include "../../driver/button_driver/button_driver.h"
#include <Arduino.h>



ErrorScreen::ErrorScreen(const std::string& title, const std::string& message)
    : title(title), message(message) {}

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
    st7789_draw_text(title.c_str(), 10, 20, ST7789_RED, 2);
    st7789_draw_text(message.c_str(), 10, 50, ST7789_WHITE, 1);
    st7789_draw_text("Press SELECT to continue", 10, 100, ST7789_GREEN, 1);
}

void ErrorScreen::on_enter() {
    st7789_fill_screen(ST7789_BLACK);
    // Clear the button event queue to avoid processing old presses
    xQueueReset(button_event_queue);
}

void ErrorScreen::on_exit() {
    // Nothing to do
}
'''