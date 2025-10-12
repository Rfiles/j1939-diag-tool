#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "screen.h"
#include "../widgets/menu.h"

class MainMenuScreen : public Screen {
public:
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    Menu menu;
};

#endif // MAIN_MENU_H
