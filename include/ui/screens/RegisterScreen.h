#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "AuthService.h"

using namespace sf;
using namespace std;

class RegisterScreen {
private:
    AuthService& auth;

    RectangleShape overlay;
    RectangleShape card;
    vector<RectangleShape> inputBoxes;
    vector<Text> labels;
    vector<Text> placeholders;
    vector<Text> displays; 

    Text title;
    RectangleShape registerBtn;
    RectangleShape backBtn;
    Text registerBtnText;
    Text backBtnText;
    Text closeX;
    Text msg;

    RectangleShape cursor;
    Clock cursorClock;
    bool showCursor = true;

    int activeField = -1;
    wstring inputs[6];

    bool showSuccessMessage = false;
    Clock messageTimer;

public:
    RegisterScreen(const Font& font, AuthService& authRef);

    bool update(Vector2f mouse, bool mousePressed, const Event& event);
    void draw(RenderWindow& window);
    void reset();

private:
    wstring bullets(size_t n);
    bool validateInputs();
};
