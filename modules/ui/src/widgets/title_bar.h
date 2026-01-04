/**
 * @file title_bar.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Title Bar Widget
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <string>

class TitleBar {
public:
    void set_title(const std::string& title);
    void draw();

private:
    void draw_can_status();
    void draw_gps_status();
    void draw_wifi_status();
    std::string title;
};

#endif // TITLE_BAR_H
