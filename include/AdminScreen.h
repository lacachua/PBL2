#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"

class AdminScreen {
private:
    Font uppercase_font, lowercase_font;  
    RectangleShape sidebar;
    RectangleShape admin_logo; Text admin_logoText;
    RenderWindow* windowPtr = nullptr;
    vector<Button> menuItems;
    vector<bool> expanded;
    vector<vector<Button>> subItems;
    Texture arrow_down, arrow_up;
    Sprite arrow_down_tex, arrow_up_tex;
public:
    AdminScreen(RenderWindow&);
    ~AdminScreen();
    void draw(RenderWindow&);
    void update(Vector2f, bool);
};

