/**
 * J1939 Diagnostic Tool - UI Manager
 * 
 * Versão: 3.8.0
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <vector>
#include <memory>
#include "screens/screen.h"

class UIManager {
public:
    void init();
    void run();

    void push_screen(std::shared_ptr<Screen> screen);
    void pop_screen();

private:
    std::vector<std::shared_ptr<Screen>> screen_stack;
};

void ui_manager_init();

#endif // UI_MANAGER_H
