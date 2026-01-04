/**
 * @file dtc_screen.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - DTC Screen
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DTC_SCREEN_H
#define DTC_SCREEN_H

#include "screen.h"
#include "../../j1939/src/dm_handler.h"
#include <vector>

extern QueueHandle_t dtc_queue;

class DtcScreen : public Screen {
public:
    DtcScreen();

    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    std::vector<DTC> dtcs;
};

#endif // DTC_SCREEN_H