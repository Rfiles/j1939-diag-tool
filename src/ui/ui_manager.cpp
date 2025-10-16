/**
 * J1939 Diagnostic Tool - UI Manager
 * 
 * Versão: 3.8.0
 */

#include "ui_manager.h"
#include <Arduino.h>
#include "screens/splash_screen.h"
#include "../driver/display/st7789_driver.h"

UIManager ui_manager;

void ui_manager_task_fn(void* pv) {
    ui_manager.run();
}

void UIManager::init() {
    st7789_init();
    push_screen(std::make_shared<SplashScreen>());
}

void UIManager::run() {
    for (;;) {
        if (!screen_stack.empty()) {
            screen_stack.back()->handle_input();
            screen_stack.back()->draw();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void UIManager::push_screen(std::shared_ptr<Screen> screen) {
    if (!screen_stack.empty()) {
        screen_stack.back()->on_exit();
    }
    screen_stack.push_back(screen);
    screen->on_enter();
}

void UIManager::pop_screen() {
    if (!screen_stack.empty()) {
        screen_stack.back()->on_exit();
        screen_stack.pop_back();
    }
    if (!screen_stack.empty()) {
        screen_stack.back()->on_enter();
    }
}

void ui_manager_init() {
    xTaskCreate(ui_manager_task_fn, "UI Manager Task", 8192, NULL, 4, NULL);
}
