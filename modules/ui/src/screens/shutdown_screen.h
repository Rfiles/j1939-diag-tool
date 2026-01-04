/**
 * @file shutdown_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Shutdown Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SHUTDOWN_SCREEN_H
#define SHUTDOWN_SCREEN_H

#include "screen.h"

class ShutdownScreen : public Screen {
public:
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;
};

#endif // SHUTDOWN_SCREEN_H