#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>

class Menu {
public:
    Menu(const std::vector<std::string>& items);

    void handle_input();
    void draw();

    int get_selected_item() const;

private:
    std::vector<std::string> items;
    int selected_item;
};

#endif // MENU_H
