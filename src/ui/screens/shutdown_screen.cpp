#include "shutdown_screen.h"
#include "../../driver/display/st7789_driver.h"
#include "../../core/config.h"
#include <Arduino.h>

void ShutdownScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    digitalWrite(config.display.bl_pin, LOW);
}

void ShutdownScreen::on_exit() {
    digitalWrite(config.display.bl_pin, HIGH);
}

void ShutdownScreen::handle_input() {
}

void ShutdownScreen::draw() {
}
