/**
 * @file error_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Error Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ERROR_SCREEN_H
#define ERROR_SCREEN_H

#include "screen.h"
#include <string>

class ErrorScreen : public Screen {
public:
    ErrorScreen(const std::string& title, const std::string& message);
    void handle_input() override;
    void draw() override;
    void on_enter() override;
    void on_exit() override;

private:
    std::string title;
    std::string message;
};

#endif // ERROR_SCREEN_H
