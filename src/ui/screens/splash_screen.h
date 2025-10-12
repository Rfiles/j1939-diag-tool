#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "screen.h"

class SplashScreen : public Screen {
public:
    void on_enter() override;
    void on_exit() override;

    void handle_input() override;
    void draw() override;

private:
    unsigned long enter_time;
};

#endif // SPLASH_SCREEN_H
