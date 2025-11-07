#pragma once
#include <SFML/Graphics.hpp>
#include "UI/components/Button.h"
#include "core/AppState.h"
using namespace sf;

class LoginRequiredPopup {
private:
    Font& font;
    RectangleShape overlay;      // Màn nền mờ
    RectangleShape popup;         // Hộp thoại
    Text messageText;
    Button backButton;
    Button loginButton;
    
public:
    LoginRequiredPopup(Font&);
    
    // Trả về: 0 = không click gì, 1 = click "Quay lại", 2 = click "Đăng nhập"
    int handleClick(Vector2f mousePos, bool mousePressed);
    void update(Vector2f mousePos, bool mousePressed);
    void draw(RenderWindow&);
};
