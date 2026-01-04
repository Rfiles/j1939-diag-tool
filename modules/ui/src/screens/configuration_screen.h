/**
 * @file configuration_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Configuration Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONFIGURATION_SCREEN_H
#define CONFIGURATION_SCREEN_H

#include "screen.h"

#include "../widgets/menu.h"

class ConfigurationScreen : public Screen {
public:
    ConfigurationScreen();
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    Menu menu;
    int brightness_level;
    void update_brightness_label();
};

#endif // CONFIGURATION_SCREEN_H