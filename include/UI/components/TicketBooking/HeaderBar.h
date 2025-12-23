#pragma once
#include <SFML/Graphics.hpp>
#include "core/AppState.h"
#include "BookingState.h"
#include "UI/components/Button.h"
using namespace sf;

class HeaderBar {
private:
    Font headerFont;
    RectangleShape mainView, summaryBox;
    Button back_button, continue_button;
public:
    HeaderBar(Font&);
    void draw(RenderWindow&, BookingState currentState);
    void update(Vector2f, bool, BookingState&);
    
    // Các phương thức điều hướng
    bool isBackButtonClicked(Vector2f mousePos, bool mousePressed);
    bool isContinueButtonClicked(Vector2f mousePos, bool mousePressed);
    void handleNavigation(Vector2f mousePos, bool mousePressed, BookingState& currentState, bool canProceed);
};
