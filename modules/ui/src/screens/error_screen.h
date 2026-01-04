'''
#ifndef ERROR_SCREEN_H
#define ERROR_SCREEN_H

#include "screen.h"
#include <string>

class ErrorScreen : public Screen {
public:
    ErrorScreen(const std::string& title, const std::string& message);
    void handle_input() override;
    void draw() override;
    void on_enter() override;
    void on_exit() override;

private:
    std::string title;
    std::string message;
};

#endif // ERROR_SCREEN_H
'''