#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "TextButton.h"
#include "HomeScreen.h"
#include "PosterSlider.h"
#include "AppState.h"
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include "AuthService.h"
#include "BookingScreen.h"
#include "AccountScreen.h"

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