// #pragma once
// #include <SFML/Graphics.hpp>
// #include <cmath>
// #include "ui/components/TextButton.h"
// #include "ui/screens/HomeScreen.h"
// #include "ui/components/PosterSlider.h"
// #include "core/AppState.h"
// #include "ui/screens/LoginScreen.h"
// #include "ui/screens/RegisterScreen.h"
// #include "services/AuthService.h"
// #include "ui/screens/BookingScreen.h"
// #include "ui/screens/AccountScreen.h"

// using namespace sf;
// using namespace std;

// class App {
// public:
//     App();
//     void run();
    
// private:
//     RenderWindow window;
//     Font font;
//     PosterSlider slider;

//     AppState state = AppState::HOME;
//     AppState previousState = AppState::HOME;
//     int previousMovieIndex = -1;
    
//     HomeScreen home;
//     LoginScreen login;
//     AuthService auth;
//     RegisterScreen registerScreen;
//     BookingScreen booking;
//     AccountScreen accountScreen;
    
//     bool mousePressed = false;
//     Vector2f mousePos;
//     string currentUser;      // Username để hiển thị
//     string currentUserEmail; // Email để lookup trong database
//     const Event* currentEvent = nullptr;

//     void processEvents();
//     void update(float);
//     void render();
// };

#pragma once
#include <SFML/Graphics.hpp>
#include "UI/screens/HomeScreen.h"
#include "UI/screens/DetailScreen.h"
#include "UI/screens/BookingScreen.h"
#include "UI/screens/LoginScreen.h"
#include "UI/screens/RegisterScreen.h"
#include "UI/screens/AccountScreen.h"
#include "services/AuthService.h"
#include "core/AppState.h"
using namespace std;
using namespace sf;

class App {
    private:
        RenderWindow window;
        Font font, detail_font, title_font;
        unique_ptr<HomeScreen> homeScreen;
        unique_ptr<DetailScreen> detailScreen;
        unique_ptr<BookingScreen> bookingScreen;
        unique_ptr<LoginScreen> loginScreen;
        unique_ptr<RegisterScreen> registerScreen;
        unique_ptr<AccountScreen> accountScreen;
        unique_ptr<AuthService> authService;
        string currentUser;
        string currentUserEmail;
        AppState state;
        AppState previousState;

        void handleStateChange();
    public:
        App();
        void run();
};