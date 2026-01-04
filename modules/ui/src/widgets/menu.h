/**
 * @file menu.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Menu Widget
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>

class Menu {
public:
    Menu(const std::vector<std::string>& items);

    void handle_input();
    void draw();

    int get_selected_item() const;

private:
    std::vector<std::string> items;
    int selected_item;
};

#endif // MENU_H