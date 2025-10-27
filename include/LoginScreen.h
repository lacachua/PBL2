#pragma once
#include <SFML/Graphics.hpp> 
#include <string>
#include "AuthService.h"
#include "AppState.h"
using namespace sf;
using namespace std;

class LoginScreen {
public:
    LoginScreen(const Font&, AuthService&);
    bool update(Vector2f, bool, const Event&, string&, AppState&);
    void draw(RenderWindow&);

private:
    AuthService& auth;

    RectangleShape overlay;
    RectangleShape card;
    RectangleShape emailBox, passBox, btn;
    Text title, emailPH, passPH, btnText, linkForgot, linkCreate, closeX;
    Text emailDisplay, passDisplay, msg;

    wstring emailInput, passInput;
    bool emailActive = false;
    bool passActive = false;

    // Thin blinking caret to mimic a real text cursor
    RectangleShape caret;

    wstring bullets(size_t n);
    Clock caretClock;
    bool caretVisible = true;

    bool loginSuccess = false;
    Clock loginClock;
    string loggedUser;
};
