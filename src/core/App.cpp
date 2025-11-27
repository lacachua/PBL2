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

    // Chỉ duy trì showtimes (xóa expired + thêm mới nếu cần), KHÔNG regenerate toàn bộ
    ShowtimeCleanupService::maintainShowtimes("../data/showtimes.txt", 5);

    authService = make_unique<AuthService>("../data/users.txt");
    authService->ensureSampleUser();

    homeScreen = make_unique<HomeScreen>(font, window);
    loginScreen = make_unique<LoginScreen>(font, *authService);
    registerScreen = make_unique<RegisterScreen>(font, *authService);
    accountScreen = make_unique<AccountScreen>(font, *authService);
}

void App::handleStateChange() {
    if (state == previousState) return;

    switch (state) {
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
            if (detailScreen && previousState == AppState::MOVIE_DETAILS) {
                String movieId = detailScreen->getMovieId();
                bookingScreen = make_unique<BookingScreen>(font, movieId);
            } 
            else bookingScreen = make_unique<BookingScreen>(font, "");
            break;

        default:
            break;
    }
    previousState = state;
}

void App::resetAfterLogout() {
    authService->logout();
    detailScreen.reset();
    bookingScreen.reset();
    accountScreen.reset();
    adminScreen.reset();
    
    loginScreen = make_unique<LoginScreen>(font, *authService);
    registerScreen = make_unique<RegisterScreen>(font, *authService);
    accountScreen = make_unique<AccountScreen>(font, *authService);
    
    homeScreen.reset();
    homeScreen = make_unique<HomeScreen>(font, window);
    
    currentUser = "";
    currentUserEmail = "";
    
    state = AppState::HOME;
    previousState = AppState::HOME;

    BaseScreen::clearLogoutFlag();
}

void App::run() {
    while (window.isOpen()) {
        bool mousePressed = false;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
            if (event->is<Event::MouseButtonPressed>()) mousePressed = true;

            switch (state) {
                case AppState::LOGIN: {
                    AppState oldState = state;
                    bool loginCompleted = loginScreen->update(mousePos, mousePressed, *event, currentUser, currentUserEmail, state);
                    if (loginCompleted && state != AppState::LOGIN) {
                        BaseScreen::setLoggedInUser(currentUser, currentUserEmail);
                        homeScreen->setLoggedUser(currentUser);
                    } 
                    else if (oldState != state && state == AppState::HOME) {
                        currentUser.clear();
                        currentUserEmail.clear();
                    }
                    break;
                }
                case AppState::REGISTER:
                    registerScreen->update(mousePos, mousePressed, *event, state);
                    break;
                case AppState::HOME:
                    homeScreen->handleEvent(mousePos, mousePressed, state, &(*event));
                    break;
                case AppState::MOVIE_DETAILS:
                    if (detailScreen) detailScreen->handleEvent(*event);
                    break;
                case AppState::BOOKING:
                    if (bookingScreen) bookingScreen->handleEvent(*event);
                    break;
                case AppState::ACCOUNT:
                    if (accountScreen) accountScreen->update(mousePos, mousePressed, &(*event), state);
                    break;
                case AppState::ADMIN_DASHBOARD:
                case AppState::ADMIN_MOVIES:
                case AppState::ADMIN_ROOMS:
                case AppState::ADMIN_SHOWTIMES:
                case AppState::ADMIN_TICKETS:
                case AppState::ADMIN_COMBOS:
                case AppState::ADMIN_CUSTOMERS:
                case AppState::ADMIN_REVENUE:
                case AppState::ADMIN_SOLD_TICKETS:
                case AppState::ADMIN_CHANGE_PASSWORD:
                    if (!adminScreen) adminScreen = make_unique<AdminScreen>(font, window, state, authService.get());
                    adminScreen->handleEvent(*event);
                    break;
                default:
                    break;
            }
        }

        AppState stateBeforeEventProcessing = state;
        
        bool movieSelectedFromSearch = false;
        int globalSearchIdx = -1;

        if (homeScreen) {
            globalSearchIdx = homeScreen->getSelectedMovieIndexFromSearch();
            if (globalSearchIdx >= 0) {
                homeScreen->clearSelectedMovieIndexFromSearch();
                movieSelectedFromSearch = true;
            }
        }
        
        if (!movieSelectedFromSearch && detailScreen) {
            globalSearchIdx = detailScreen->getSelectedMovieIndexFromSearch();
            if (globalSearchIdx >= 0) {
                detailScreen->clearSelectedMovieIndexFromSearch();
                movieSelectedFromSearch = true;
            }
        }
        
        if (!movieSelectedFromSearch && bookingScreen) {
            globalSearchIdx = bookingScreen->getSelectedMovieIndexFromSearch();
            if (globalSearchIdx >= 0) {
                bookingScreen->clearSelectedMovieIndexFromSearch();
                movieSelectedFromSearch = true;
            }
        }
        
        if (!movieSelectedFromSearch && accountScreen) {
            globalSearchIdx = accountScreen->getSelectedMovieIndexFromSearch();
            if (globalSearchIdx >= 0) {
                accountScreen->clearSelectedMovieIndexFromSearch();
                movieSelectedFromSearch = true;
            }
        }
        
        if (movieSelectedFromSearch && homeScreen && homeScreen->getRepository()) {
            MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(globalSearchIdx);
            detailScreen = make_unique<DetailScreen>(font, font, title_font, detail_font, detail);
            if (state == AppState::BOOKING || previousState == AppState::BOOKING) bookingScreen.reset();
        }
        
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

            case AppState::LOGIN:
                break;

            case AppState::REGISTER:
                break;

            case AppState::ADMIN_DASHBOARD:
            case AppState::ADMIN_MOVIES:
            case AppState::ADMIN_ROOMS:
            case AppState::ADMIN_SHOWTIMES:
            case AppState::ADMIN_TICKETS:
            case AppState::ADMIN_COMBOS:
            case AppState::ADMIN_CUSTOMERS:
            case AppState::ADMIN_REVENUE:
            case AppState::ADMIN_SOLD_TICKETS:
            case AppState::ADMIN_CHANGE_PASSWORD:
                if (adminScreen) adminScreen->update(mousePos, mousePressed, state);
                break;

            default:
                break;
        }

        handleStateChange();
        
        // ✅ Check if logout was requested and reset everything
        if (BaseScreen::isLogoutRequested()) {
            resetAfterLogout();
            // After reset, skip this frame to avoid drawing deleted screens
            continue;
        }

        // --- Draw ---
        window.clear(Color::White);
        switch (state) {
            case AppState::HOME:
                homeScreen->draw(window);
                // ✅ Draw search bar overlay on top of everything
                homeScreen->drawOverlay(window);
                break;
            case AppState::MOVIE_DETAILS:
                if (detailScreen) {
                    detailScreen->draw(window);
                    // ✅ Draw search bar overlay on top of everything
                    detailScreen->drawOverlay(window);
                }
                break;
            case AppState::BOOKING:
                if (bookingScreen) {
                    bookingScreen->draw(window);
                    // ✅ Draw search bar overlay on top of everything
                    bookingScreen->drawOverlay(window);
                }
                break;
            case AppState::ACCOUNT:
                // ✅ AccountScreen giờ kế thừa BaseScreen nên tự vẽ header
                if (accountScreen) {
                    accountScreen->draw(window);
                    // ✅ Draw search bar overlay on top of everything
                    accountScreen->drawOverlay(window);
                }
                break;
            case AppState::LOGIN:
                loginScreen->draw(window);
                break;
            case AppState::REGISTER:
                registerScreen->draw(window);
                break;
            case AppState::ADMIN_DASHBOARD:
            case AppState::ADMIN_MOVIES:
            case AppState::ADMIN_ROOMS:
            case AppState::ADMIN_SHOWTIMES:
            case AppState::ADMIN_TICKETS:
            case AppState::ADMIN_COMBOS:
            case AppState::ADMIN_CUSTOMERS:
            case AppState::ADMIN_REVENUE:
            case AppState::ADMIN_SOLD_TICKETS:
            case AppState::ADMIN_CHANGE_PASSWORD:
                if (adminScreen) {
                    adminScreen->render();
                }
                break;
            default:
                break;
        }
        window.display();
    }
}
