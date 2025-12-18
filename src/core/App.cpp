#include "core/App.h"
#include <SFML/Graphics.hpp>

using namespace sf;

App::App()
    : window(VideoMode({1728, 972}), L"CiNeXíNè", Style::Titlebar | Style::Close),
      font("../assets/fonts/Montserrat_SemiBold.ttf"),
      detail_font("../assets/fonts/quicksand_medium.ttf"),
      title_font("../assets/fonts/BEBAS_NEUE_ZSMALL.ttf"),
      state(AppState::HOME),
      previousState(AppState::HOME)
{
    window.setFramerateLimit(60);

    ShowtimeCleanupService::maintainShowtimes("../data/showtimes.txt", 5);

    authService = make_unique<AuthService>("../data/users.txt");

    homeScreen = make_unique<HomeScreen>(font, window);
    loginScreen = make_unique<LoginScreen>(font, *authService);
    registerScreen = make_unique<RegisterScreen>(font, *authService);
    accountScreen = make_unique<AccountScreen>(font, *authService);
}

bool App::isAdminState(AppState s) {
    return (s >= AppState::ADMIN_DASHBOARD && s <= AppState::ADMIN_CHANGE_PASSWORD);
}

BaseScreen* App::getCurrentScreen() {
    if (isAdminState(state)) return adminScreen.get();

    switch (state) {
        case AppState::HOME:          return homeScreen.get();
        case AppState::MOVIE_DETAILS: return detailScreen.get();
        case AppState::BOOKING:       return bookingScreen.get();
        case AppState::ACCOUNT:       return accountScreen.get();
        default:                      return nullptr;
    }
}

void App::handleStateChange() {
    if (state == previousState) return;

    switch (state) {
        case AppState::HOME:
            bookingScreen.reset();
            detailScreen.reset();
            break;
            
        case AppState::MOVIE_DETAILS:
            if (homeScreen && homeScreen->getRepository()) {
                int selectedIndex = homeScreen->getSelectedMovieIndex();
                if (selectedIndex >= 0) {
                    MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(selectedIndex);
                    detailScreen = make_unique<DetailScreen>(font, font, title_font, detail_font, detail);
                    homeScreen->clearSelectedMovieIndex();
                }
            }
            break;

        case AppState::BOOKING:
            bookingScreen.reset();
            if (detailScreen && previousState == AppState::MOVIE_DETAILS) {
                String movieId = detailScreen->getMovieId();
                bookingScreen = make_unique<BookingScreen>(font, movieId);
            } 
            else bookingScreen = make_unique<BookingScreen>(font, "");
            break;

        default: break;
    }
    previousState = state;
}

void App::resetAfterLogout() {
    authService->logout();

    detailScreen.reset();
    bookingScreen.reset();
    accountScreen.reset();
    adminScreen.reset();
    homeScreen.reset();
    
    loginScreen = make_unique<LoginScreen>(font, *authService);
    registerScreen = make_unique<RegisterScreen>(font, *authService);
    accountScreen = make_unique<AccountScreen>(font, *authService);
    homeScreen = make_unique<HomeScreen>(font, window);
    
    currentUser = "";
    currentUserEmail = "";
    state = AppState::HOME;
    previousState = AppState::HOME;

    BaseScreen::clearLogoutFlag();
}

void App::handleGlobalSearch() {
    BaseScreen* cur = getCurrentScreen();
    if (!cur) return;

    if (state == AppState::LOGIN || state == AppState::REGISTER || isAdminState(state)) return;

    int globalSearchIdx = cur->getSelectedMovieIndexFromSearch();

    if (globalSearchIdx >= 0) {
        cur->clearSelectedMovieIndexFromSearch();

        if (homeScreen && homeScreen->getRepository()) {
            MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(globalSearchIdx);
            detailScreen = make_unique<DetailScreen>(font, font, title_font, detail_font, detail);

            if (state == AppState::BOOKING) bookingScreen.reset();

            state = AppState::MOVIE_DETAILS;
        }
    }
}

void App::render() {
    window.clear(Color::White);
    
    if (isAdminState(state)) {
        if (adminScreen) adminScreen->render();
    }
    else if (state == AppState::LOGIN) {
        loginScreen->draw(window);
    }
    else if (state == AppState::REGISTER) {
        registerScreen->draw(window);
    }
    else {
        BaseScreen* current = getCurrentScreen();
        if (current) {
            current->draw(window);
            current->drawOverlay(window);
        }
    }
    window.display();
}

void App::run() {
    while (window.isOpen()) {
        bool mousePressed = false;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        // 1. Event Polling
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
            if (event->is<Event::MouseButtonPressed>()) mousePressed = true;

            if (isAdminState(state)) {
                if (!adminScreen) adminScreen = make_unique<AdminScreen>(font, window, state, authService.get());
                adminScreen->handleEvent(*event);
            }
            else {
                switch (state) {
                    case AppState::HOME: 
                        homeScreen->handleEvent(mousePos, mousePressed, state, &(*event)); 
                        break;
                    case AppState::MOVIE_DETAILS: 
                        if(detailScreen) detailScreen->handleEvent(*event); 
                        break;
                    case AppState::BOOKING: 
                        if(bookingScreen) bookingScreen->handleEvent(*event); 
                        break;
                    case AppState::LOGIN: 
                        loginScreen->handleEvent(*event, state, currentUser, currentUserEmail, loginSuccess); 
                        break;
                    case AppState::REGISTER: 
                        registerScreen->handleEvent(*event, state); 
                        break;
                    case AppState::ACCOUNT: 
                        if(accountScreen) accountScreen->update(mousePos, mousePressed, &(*event), state); 
                        break;
                    default: break; 
                }
            }
        }

        // 2. Logic Updates & Search
        handleGlobalSearch();

        if (isAdminState(state)) {
            if (adminScreen) adminScreen->update(mousePos, mousePressed, state);
        }
        else {
            switch (state) {
                case AppState::HOME:
                    homeScreen->setLoggedUser(authService->isLoggedIn() ? authService->getCurrentUser()->getUsername() : "");
                    homeScreen->update(mousePos, mousePressed, state);
                    break;

                case AppState::MOVIE_DETAILS:
                    if (detailScreen) detailScreen->update(mousePos, mousePressed, state);
                    break;
                
                case AppState::BOOKING:
                    if (bookingScreen) bookingScreen->update(mousePos, mousePressed, state);
                    break;

                case AppState::ACCOUNT:
                    if (accountScreen) {
                        string email = BaseScreen::getLoggedInUserEmail();
                        if (!email.empty()) accountScreen->setCurrentUser(email);
                        accountScreen->update(mousePos, mousePressed, nullptr, state);
                    }
                    break;

                case AppState::LOGIN: {
                    loginSuccess = false; // Reset mỗi frame
                    bool loginCompleted = loginScreen->update(mousePos, mousePressed, currentUser, currentUserEmail, state);
                    if (loginCompleted && state != AppState::LOGIN) {
                        BaseScreen::setLoggedInUser(currentUser, currentUserEmail);
                        homeScreen->setLoggedUser(currentUser);
                    } 
                    break;
                }

                case AppState::REGISTER:
                    registerScreen->update(mousePos, mousePressed, state);
                    break;

                default: break;
            }
        }

        // 3. State Management
        handleStateChange();
        
        if (BaseScreen::isLogoutRequested()) {
            resetAfterLogout();
            continue;
        }

        // 4. Render
        render();
    }
}
