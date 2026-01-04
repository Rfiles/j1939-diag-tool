#include "menu.h"
#include "../../driver/display/st7789_driver.h"
#include "../../driver/button_driver/button_driver.h"

Menu::Menu(const std::vector<std::string>& items) : items(items), selected_item(0) {}

void Menu::handle_input() {
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        if (event == BTN_PRESS_NEXT) {
            selected_item = (selected_item + 1) % items.size();
        } else if (event == BTN_PRESS_SELECT) {
            // Handle selection in the screen that uses the menu
        }
    }
}

void Menu::draw() {
    int y = 60;
    for (int i = 0; i < items.size(); ++i) {
        if (i == selected_item) {
            st7789_draw_text(items[i].c_str(), 120, y, COLOR_YELLOW, COLOR_BLACK);
        } else {
            st7789_draw_text(items[i].c_str(), 120, y, COLOR_WHITE, COLOR_BLACK);
        }
        y += 20;
    }
}

int Menu::get_selected_item() const {
    return selected_item;
}
