#pragma once
#include <SFML/Graphics.hpp>

#include "UI/screens/HomeScreen.h"
#include "UI/screens/DetailScreen.h"
#include "UI/screens/BookingScreen.h"
#include "UI/screens/LoginScreen.h"
#include "UI/screens/RegisterScreen.h"
#include "UI/screens/AccountScreen.h"
#include "UI/screens/AdminScreen.h"

#include "services/AuthService.h"
#include "services/ShowtimeCleanupService.h"
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
        unique_ptr<AdminScreen> adminScreen;

        unique_ptr<AuthService> authService;
        string currentUser;
        string currentUserEmail;
        bool loginSuccess = false;

        AppState state;
        AppState previousState;

        void handleStateChange();
        void resetAfterLogout();
        BaseScreen* getCurrentScreen();
        void handleGlobalSearch();
        bool isAdminState(AppState);
        void render();
    public:
        App();
        void run();
};