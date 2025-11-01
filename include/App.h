#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "UI/components/TextButton.h"
#include "UI/screens/HomeScreen.h"
#include "UI/components/PosterSlider.h"
#include "AppState.h"
#include "UI/screens/LoginScreen.h"
#include "UI/screens/RegisterScreen.h"
#include "AuthService.h"
#include "UI/screens/BookingScreen.h"
#include "UI/screens/AccountScreen.h"

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