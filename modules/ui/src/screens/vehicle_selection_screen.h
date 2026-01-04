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
