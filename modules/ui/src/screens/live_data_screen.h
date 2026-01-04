/**
 * @file live_data_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Live Data Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LIVE_DATA_SCREEN_H
#define LIVE_DATA_SCREEN_H

#include "screen.h"
#include "../../j1939/src/vehicle_db_handler.h"

class LiveDataScreen : public Screen {
public:
    LiveDataScreen(const VehicleEcu* ecu);

    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    const VehicleEcu* ecu;
};

void ui_show_ecu_data(const VehicleEcu* ecu);

#endif // LIVE_DATA_SCREEN_H