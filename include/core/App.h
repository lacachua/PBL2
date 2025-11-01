#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "ui/components/TextButton.h"
#include "ui/screens/HomeScreen.h"
#include "ui/components/PosterSlider.h"
#include "core/AppState.h"
#include "ui/screens/LoginScreen.h"
#include "ui/screens/RegisterScreen.h"
#include "services/AuthService.h"
#include "ui/screens/BookingScreen.h"
#include "ui/screens/AccountScreen.h"

using namespace sf;
using namespace std;

class App {
public:
    App();
    void run();
    
private:
    RenderWindow window;
    Font font;
    PosterSlider slider;

    AppState state = AppState::HOME;
    AppState previousState = AppState::HOME;
    int previousMovieIndex = -1;
    
    HomeScreen home;
    LoginScreen login;
    AuthService auth;
    RegisterScreen registerScreen;
    BookingScreen booking;
    AccountScreen accountScreen;
    
    bool mousePressed = false;
    Vector2f mousePos;
    string currentUser;      // Username để hiển thị
    string currentUserEmail; // Email để lookup trong database
    const Event* currentEvent = nullptr;

    void processEvents();
    void update(float);
    void render();
};