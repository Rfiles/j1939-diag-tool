/**
 * @file main_menu.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Main Menu Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "screen.h"
#include "../widgets/menu.h"

class MainMenuScreen : public Screen {
public:
    MainMenuScreen();
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    Menu menu;
};

#endif // MAIN_MENU_H