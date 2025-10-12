#ifndef SHUTDOWN_SCREEN_H
#define SHUTDOWN_SCREEN_H

#include "screen.h"

class ShutdownScreen : public Screen {
public:
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;
};

#endif // SHUTDOWN_SCREEN_H
