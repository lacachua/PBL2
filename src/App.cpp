#include "App.h"
#include "DetailScreen.h"
#include "Movie.h"
#include "BookingScreen.h"
#include <fstream>
#include <sstream>

App::App() : 
    window(VideoMode({1728, 972}), L"CiNeXíNè", Style::Titlebar | Style::Close),
    font("../assets/Montserrat_SemiBold.ttf"),
    home(font),
    slider(font, window),
    auth("../data/users.csv"),
    login(font, auth),
    registerScreen(font, auth),
    booking(font)
    { 
        window.setFramerateLimit(60);
        Image icon("../assets/icon.png");
        window.setIcon(icon);

        auth.ensureSampleUser();

        vector<string> paths = getMoviePosterPaths("../data/movies.csv");        
        slider.loadPosters(paths, font);
// <<<<<<< HEAD
        
//         // Load movies for search functionality
        vector<Movie> movies = loadMoviesFromCSV("../data/movies.csv");
        home.initializeSearch(movies);

// =======
// >>>>>>> feature-datvengay
}

void App::run() {
    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void App::processEvents() {
    mousePressed = false;
    currentEvent = nullptr;

    while (auto optEvent = window.pollEvent()) {
        const Event& event = *optEvent;
        currentEvent = &event; // Lưu event để dùng trong render()
        
        if (event.is<Event::Closed>()) window.close();
        if (event.is<Event::MouseButtonPressed>()) mousePressed = true;

        mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        switch (state) {
            case AppState::HOME:
                home.setLoggedUser(currentUser);
                home.update(mousePos, mousePressed, state, &event);
                break;

            case AppState::LOGIN:
                // Cho phép SearchBox hoạt động ở màn LOGIN
                home.update(mousePos, mousePressed, state, &event);
                if (login.update(mousePos, mousePressed, event, currentUser, state)) {
                    home.setLoggedUser(currentUser);
                    state = AppState::HOME;
                }
                break;
            
            case AppState::REGISTER:
                // Cho phép SearchBox hoạt động ở màn REGISTER
                home.update(mousePos, mousePressed, state, &event);
                if (registerScreen.update(mousePos, mousePressed, event))
                    state = AppState::LOGIN;
                break;

            default:
                break;
        }

    slider.handleEvent(mousePos, mousePressed, state);
    }
}

void App::update(float dt) {
    slider.update(dt, window);
}

void App::render() {
    window.clear(Color::Black);

    switch (state) {
        case AppState::HOME: {
            home.draw(window);
            slider.draw(window);
            home.drawSearchBox(window); // Vẽ SearchBox SAU slider
            home.drawDropdown(window);
            break;
        }

        case AppState::LOGIN: {
            home.draw(window);
            slider.draw(window);
            home.drawSearchBox(window); // Vẽ SearchBox SAU slider
            login.draw(window);
            break;
        }

        case AppState::REGISTER: {
            home.draw(window);
            slider.draw(window);
            home.drawSearchBox(window); // Vẽ SearchBox SAU slider
            registerScreen.draw(window);
            break;
        }

        case AppState::MOVIE_DETAILS: {
// <<<<<<< HEAD
//             // Check if navigation came from search
//             int searchMovieIdx = home.getSelectedMovieIndex();
//             if (searchMovieIdx >= 0) {
//                 DetailScreen detail(font, searchMovieIdx, currentUser);
//                 detail.update(mousePos, mousePressed, state, currentEvent);
//                 detail.draw(window);
                
//                 // Clear the selection when returning to home
//                 if (state != AppState::MOVIE_DETAILS) {
//                     home.clearSelectedMovieIndex();
//                 }
//             } else {
//                 DetailScreen detail(font, slider.getSelectedIndex(), currentUser);
//                 detail.update(mousePos, mousePressed, state, currentEvent);
//                 detail.draw(window);
//             }
// =======
            static DetailScreen* detailScreen = nullptr;
            int currentIndex = slider.getSelectedIndex();
            
            // Chỉ tạo mới khi chuyển state hoặc đổi phim
            if (previousState != AppState::MOVIE_DETAILS || previousMovieIndex != currentIndex) {
                delete detailScreen;
                detailScreen = new DetailScreen(font, currentIndex, currentUser);
                previousMovieIndex = currentIndex;
            }
            
            detailScreen->update(mousePos, mousePressed, state);
            detailScreen->draw(window);

            if (state == AppState::BOOKING && detailScreen != nullptr) {
                booking.loadFromDetail(*detailScreen);
                booking.setLoggedUser(currentUser);
            }
            break;
        }

        case AppState::BOOKING: {
            booking.handleEvent(window, mousePos, mousePressed);
            booking.update(mousePos, mousePressed, state);
            booking.draw(window);
// >>>>>>> feature-datvengay
            break;
        }

        default:
            break;
    }

    previousState = state;
    window.display();
}