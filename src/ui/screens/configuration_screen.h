#ifndef CONFIGURATION_SCREEN_H
#define CONFIGURATION_SCREEN_H

#include "screen.h"

class ConfigurationScreen : public Screen {
public:
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;
};

#endif // CONFIGURATION_SCREEN_H
