/**
 * @file vehicle_selection_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Vehicle Selection Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef VEHICLE_SELECTION_SCREEN_H
#define VEHICLE_SELECTION_SCREEN_H

#include "screen.h"
#include "../widgets/menu.h"

class VehicleSelectionScreen : public Screen {
public:
    VehicleSelectionScreen();

    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    Menu menu;
};

#endif // VEHICLE_SELECTION_SCREEN_H