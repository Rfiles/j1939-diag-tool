/**
 * J1939 Diagnostic Tool - Live Data Screen
 * 
 * Versão: 3.8.0
 */

#ifndef LIVE_DATA_SCREEN_H
#define LIVE_DATA_SCREEN_H

#include "screen.h"
#include "../core/vehicle_db_handler.h"

class LiveDataScreen : public Screen {
public:
    LiveDataScreen(const Ecu* ecu);

    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    const Ecu* ecu;
};

void ui_show_ecu_data(const Ecu* ecu);

#endif // LIVE_DATA_SCREEN_H
