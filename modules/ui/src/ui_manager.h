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
    /**
     * @brief Gets the singleton instance of the UIManager.
     */
    static UIManager& getInstance() {
        static UIManager instance;
        return instance;
    }

    /**
     * @brief Initializes the UI manager.
     */
    void init();

    /**
     * @brief Runs the UI loop (handles input and drawing).
     */
    void run();

    /**
     * @brief Pushes a new screen onto the screen stack.
     * @param screen A shared pointer to the screen to push.
     */
    void push_screen(std::shared_ptr<Screen> screen);

    /**
     * @brief Pops the current screen from the screen stack.
     */
    void pop_screen();

private:
    std::vector<std::shared_ptr<Screen>> screen_stack;

    // Private constructor for singleton pattern
    UIManager() {}

    // Delete copy constructor and assignment operator
    UIManager(UIManager const&) = delete;
    void operator=(UIManager const&) = delete;
};

// No longer needed as it will be accessed via UIManager::getInstance()
// void ui_manager_init();

// No longer needed as it will be accessed via UIManager::getInstance()
// extern UIManager ui_manager;

#endif // UI_MANAGER_H