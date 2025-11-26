#pragma once
#include <SFML/Graphics.hpp> 
#include <string>
#include <memory>
#include "services/AuthService.h"
#include "core/AppState.h"
using namespace sf;
using namespace std;

class LoginScreen {
public:
    LoginScreen(const Font&, AuthService&);
    bool update(Vector2f, bool, const Event&, string& currentUser, string& currentUserEmail, AppState&);
    void draw(RenderWindow&);

private:
    AuthService& auth;

    RectangleShape overlay;
    RectangleShape card;
    RectangleShape emailBox, passBox, btn;
    RectangleShape eyeToggleArea;  // Clickable area for show/hide password
    Text title, emailPH, passPH, btnText, linkForgot, linkCreate, closeX;
    Text emailDisplay, passDisplay, msg;
    
    // Eye icon textures and sprite
    Texture eyeOpenTexture;
    Texture eyeClosedTexture;
    unique_ptr<Sprite> eyeSprite;

    wstring emailInput, passInput;
    bool emailActive = false;
    bool passActive = false;
    bool showPassword = false;  // Toggle password visibility

    // Thin blinking caret to mimic a real text cursor
    RectangleShape caret;

    wstring bullets(size_t n);
    Clock caretClock;
    bool caretVisible = true;

    bool loginSuccess = false;
    Clock loginClock;
    string loggedUser;
};
