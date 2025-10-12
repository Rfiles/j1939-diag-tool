#ifndef SCREEN_H
#define SCREEN_H

#include "../widgets/title_bar.h"

class Screen {
public:
    virtual ~Screen() = default;

    virtual void on_enter() = 0;
    virtual void on_exit() = 0;

    virtual void handle_input() = 0;
    virtual void draw() = 0;

protected:
    TitleBar title_bar;
};

#endif // SCREEN_H
