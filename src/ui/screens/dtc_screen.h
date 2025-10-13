/**
 * J1939 Diagnostic Tool - DTC Screen
 * 
 * Versão: 3.8.0
 */

#ifndef DTC_SCREEN_H
#define DTC_SCREEN_H

#include "screen.h"
#include "../j1939/dm_handler.h"
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
