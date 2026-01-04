#include "configuration_screen.h"
#include "../ui_manager.h"
#include "../../settings/src/config.h"
#include "../../settings/src/settings_handler.h"
#include "../../ui/driver/display/st7789_driver.h"
#include "../../ui/driver/button_driver/button_driver.h"
#include <Arduino.h>

// Map brightness level (0-9) to PWM value (5% to 100% of 255)
uint8_t level_to_pwm(int level) {
    if (level < 0) level = 0;
    if (level > 9) level = 9;
    return 13 + (level * 242) / 9; // 13 is ~5% of 255
}

int pwm_to_level(uint8_t pwm) {
    if (pwm < 13) return 0;
    return ((pwm - 13) * 9) / 242;
}

ConfigurationScreen::ConfigurationScreen() 
    : menu({"Brilho", "Guardar", "Sair"}) {
    title_bar.set_title("CONFIGURACOES");
}

void ConfigurationScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);

    // Initialize brightness level from current config
    brightness_level = pwm_to_level(config.display.display_brightness);
    update_brightness_label();

    menu.draw(); 
}

void ConfigurationScreen::on_exit() {
    // Revert brightness to saved config if user exits without saving
    st7789_set_brightness(config.display.display_brightness);
}

void ConfigurationScreen::handle_input() {
    menu.handle_input();
    button_event_t event;
    if (xQueueReceive(button_event_queue, &event, 0) == pdPASS) {
        int selected = menu.get_selected_item();

        if (event == BTN_PRESS_SELECT) {
            if (selected == 1) { // Guardar
                settings_save();
                UIManager::getInstance().pop_screen();
            } else if (selected == 2) { // Sair
                UIManager::getInstance().pop_screen();
            }
        } else if (event == BTN_PRESS_NEXT) {
             if (selected == 0) { // Brilho
                brightness_level = (brightness_level + 1) % 10; // 0-9
                uint8_t pwm_value = level_to_pwm(brightness_level);
                // Temporarily set brightness
                st7789_set_brightness(pwm_value);
                // Update the label
                update_brightness_label();
                // Update the config value so it can be saved
                config.display.display_brightness = pwm_value;
             }
        }
    }
}

void ConfigurationScreen::draw() {
    title_bar.draw();
    menu.draw();
}

void ConfigurationScreen::update_brightness_label() {
    char buf[20];
    // Calculate percentage for display: level 0 is 5%, level 9 is 100%
    int percentage = 5 + (brightness_level * 95) / 9;
    sprintf(buf, "Brilho: %d%%", percentage);
    // Overwrite previous label
    st7789_fill_rect(5, 60, 150, 20, COLOR_BLACK);
    st7789_draw_text(buf, 5, 60, COLOR_WHITE, COLOR_BLACK);
}