#include "splash_screen.h"
#include "../ui_manager.h"
#include "../../core/config.h"
#include "../../core/license_handler.h"
#include "../../driver/display/st7789_driver.h"
#include "main_menu.h"
#include <Arduino.h>

extern UIManager ui_manager;

void SplashScreen::on_enter() {
    st7789_fill_screen(COLOR_BLACK);
    st7789_draw_text("BUS DIAG", 120, 60, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text("RICARDO REIS", 120, 80, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text(FIRMWARE_VERSION, 120, 100, COLOR_WHITE, COLOR_BLACK);
    st7789_draw_text(license_get_hw_id().c_str(), 120, 120, COLOR_WHITE, COLOR_BLACK);
    enter_time = millis();
}

void SplashScreen::on_exit() {
}

void SplashScreen::handle_input() {
    if (millis() - enter_time > 3000) {
        ui_manager.push_screen(std::make_shared<MainMenuScreen>());
    }
}

void SplashScreen::draw() {
    // Static screen, no need to redraw
}
