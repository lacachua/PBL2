// #pragma once
// #include <SFML/Graphics.hpp>
// #include "data-structures/DLL.h"
// #include "BookingState.h"
// #include "UI/components/Button.h"
// using namespace sf;

// class HeaderBar {
//     private:
//         Font headerFont;  // Font riêng cho HeaderBar
//         DLL<RectangleShape> boxes;
//         DLL<Text> texts;
//         RectangleShape mainView, summaryBox;
//         Button back_button, continue_button;
//     public:
//         HeaderBar(Font&);  // Không cần nhận Font từ bên ngoài
//         void update(Vector2f, bool, BookingState);
//         void draw(RenderWindow&);
// };

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
    
    // Navigation methods
    bool isBackButtonClicked(Vector2f mousePos, bool mousePressed);
    bool isContinueButtonClicked(Vector2f mousePos, bool mousePressed);
    void handleNavigation(Vector2f mousePos, bool mousePressed, BookingState& currentState, bool canProceed);
};
