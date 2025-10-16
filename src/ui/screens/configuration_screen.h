#ifndef CONFIGURATION_SCREEN_H
#define CONFIGURATION_SCREEN_H

#include "screen.h"

#include "../widgets/menu.h"

class ConfigurationScreen : public Screen {
public:
    ConfigurationScreen();
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    Menu menu;
    int brightness_level;
    void update_brightness_label();
};

#endif // CONFIGURATION_SCREEN_H
