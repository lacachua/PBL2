// #include "core/App.h"
// #include "ui/screens/DetailScreen.h"
// #include "models/Movie.h"
// #include "ui/screens/BookingScreen.h"
// #include <fstream>
// #include <sstream>

// App::App() : 
//     window(VideoMode({1728, 972}), L"CiNeXíNè", Style::Titlebar | Style::Close),
//     font("../assets/Montserrat_SemiBold.ttf"),
//     home(font),
//     slider(font, window),
//     auth("../data/users.csv"),
//     login(font, auth),
//     registerScreen(font, auth),
//     booking(font),
//     accountScreen(font, auth)
// { 
//     window.setFramerateLimit(60);
//     Image icon("../assets/icon.png");
//     window.setIcon(icon);

//     auth.ensureSampleUser();

//     vector<string> paths = getMoviePosterPaths("../data/movies.csv");        
//     slider.loadPosters(paths, font);
    
//     // Load movies for search functionality
//     vector<Movie> movies = loadMoviesFromCSV("../data/movies.csv");
//     home.initializeSearch(movies);
// }

// void App::run() {
//     Clock clock;

//     while (window.isOpen()) {
//         float dt = clock.restart().asSeconds();
//         processEvents();
//         update(dt);
//         render();
//     }
// }

// void App::processEvents() {
//     mousePressed = false;
//     currentEvent = nullptr;

//     while (auto optEvent = window.pollEvent()) {
//         const Event& event = *optEvent;
//         currentEvent = &event;
        
//         if (event.is<Event::Closed>()) window.close();
//         if (event.is<Event::MouseButtonPressed>()) mousePressed = true;

//         mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

//         switch (state) {
//             case AppState::HOME:
//                 home.setLoggedUser(currentUser);
//                 home.update(mousePos, mousePressed, state, &event);
//                 break;

//             case AppState::LOGIN:
//                 home.update(mousePos, mousePressed, state, &event);
//                 if (login.update(mousePos, mousePressed, event, currentUser, currentUserEmail, state)) {
//                     home.setLoggedUser(currentUser);
//                     state = AppState::HOME;
//                 }
//                 break;
            
//             case AppState::REGISTER:
//                 home.update(mousePos, mousePressed, state, &event);
//                 if (registerScreen.update(mousePos, mousePressed, event))
//                     state = AppState::LOGIN;
//                 break;
            
//             case AppState::ACCOUNT:
//                 home.setLoggedUser(currentUser);
//                 home.update(mousePos, mousePressed, state, &event);
//                 accountScreen.update(mousePos, mousePressed, &event, state);
//                 break;

//             default:
//                 break;
//         }

//         slider.handleEvent(mousePos, mousePressed, state);
//     }
// }

// void App::update(float dt) {
//     slider.update(dt, window);
// }

// void App::render() {
//     window.clear(Color::Black);

//     switch (state) {
//         case AppState::HOME: {
//             home.draw(window);
//             slider.draw(window);
//             home.drawSearchBox(window);
//             home.drawDropdown(window);
//             break;
//         }

//         case AppState::LOGIN: {
//             home.draw(window);
//             slider.draw(window);
//             home.drawSearchBox(window);
//             login.draw(window);
//             break;
//         }

//         case AppState::REGISTER: {
//             home.draw(window);
//             slider.draw(window);
//             home.drawSearchBox(window);
//             registerScreen.draw(window);
//             break;
//         }

//         case AppState::MOVIE_DETAILS: {
//             static DetailScreen* detailScreen = nullptr;
//             int currentIndex = slider.getSelectedIndex();
            
//             if (previousState != AppState::MOVIE_DETAILS || previousMovieIndex != currentIndex) {
//                 delete detailScreen;
//                 detailScreen = new DetailScreen(font, currentIndex, currentUser);
//                 previousMovieIndex = currentIndex;
//             }
            
//             detailScreen->update(mousePos, mousePressed, state);
//             detailScreen->draw(window);

//             if (state == AppState::BOOKING && detailScreen != nullptr) {
//                 booking.loadFromDetail(*detailScreen);
//                 booking.setLoggedUser(currentUser);
//             }
//             break;
//         }

//         case AppState::BOOKING: {
//             booking.handleEvent(window, mousePos, mousePressed);
//             booking.update(mousePos, mousePressed, state);
//             booking.draw(window);
//             break;
//         }
        
//         case AppState::ACCOUNT: {
//             home.draw(window);
//             home.drawSearchBox(window);
            
//             if (previousState != AppState::ACCOUNT) {
//                 accountScreen.setCurrentUser(currentUserEmail);
//             }
            
//             accountScreen.draw(window);
//             break;
//         }

//         default:
//             break;
//     }

//     previousState = state;
//     window.display();
// }

// #include "core/App.h"

// App::App() 
//     :   window(VideoMode({1728, 972}), L"CiNeXíNè", Style::Titlebar | Style::Close),
//         font("../assets/fonts/Montserrat_SemiBold.ttf"),
//         state(AppState::HOME),
//         previousState(AppState::HOME)
// {
//     window.setFramerateLimit(60);
//     homeScreen = make_unique<HomeScreen>(font, window);
// }

// void App::handleStateChange() {
//     if (state != previousState) {
//         switch (state) {
//             case AppState::MOVIE_DETAILS:
//                 if (homeScreen) {
//                     MovieDetail detail = homeScreen->getMovieDetailbyIndex(homeScreen->getSelectedIndex());
//                     detailScreen = make_unique<DetailScreen>(font, detail);
//                 }
//                 break;
//             case AppState::BOOKING:
//                 // Get movie_id from DetailScreen if coming from detail page
//                 if (detailScreen && previousState == AppState::MOVIE_DETAILS) {
//                     String movieId = detailScreen->getMovieId();
//                     bookingScreen = make_unique<BookingScreen>(font, movieId);
//                 } 
//                 break;
//             case AppState::HOME:
//                 // Home screen always exists
//                 break;
//         }
//         previousState = state;
//     }
// }

// void App::run() {
//     Clock clock;
    
//     while (window.isOpen()) {
//         bool mousePressed = false;
//         while (auto event = window.pollEvent()) {
//             if (event->is<Event::Closed>()) window.close();
//             if (event->is<Event::MouseButtonPressed>()) mousePressed = true;
//         }

//         Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
//         switch (state) {
//             case AppState::HOME:
//                 homeScreen->update(mousePos, mousePressed, state);
//                 homeScreen->handleEvent(mousePos, mousePressed, state);
//                 break;
//             case AppState::MOVIE_DETAILS:
//                 if (detailScreen)
//                     detailScreen->update(mousePos, mousePressed, state);
//                 break;
//             case AppState::BOOKING:
//                 if (bookingScreen)
//                     bookingScreen->update(mousePos, mousePressed, state);
//                 break;
//         }
        
//         handleStateChange();
        
//         window.clear(Color::White);
//         switch (state) {
//             case AppState::HOME:
//                 homeScreen->draw(window);
//                 break;
//             case AppState::MOVIE_DETAILS:
//                 if (detailScreen)
//                     detailScreen->draw(window);
//                 break;
//             case AppState::BOOKING:
//                 if (bookingScreen)
//                     bookingScreen->draw(window);
//                 break;
//         }
//         window.display();
//     }
// }

#include "core/App.h"
#include <SFML/Graphics.hpp>
using namespace sf;

App::App()
    : window(VideoMode({1728, 972}), L"CiNeXíNè", Style::Titlebar | Style::Close),
      font("../assets/fonts/Montserrat_SemiBold.ttf"),
      state(AppState::HOME),
      previousState(AppState::HOME)
{
    window.setFramerateLimit(60);

    authService    = std::make_unique<AuthService>("../data/users.txt");
    authService->ensureSampleUser();

    homeScreen     = std::make_unique<HomeScreen>(font, window);
    loginScreen    = std::make_unique<LoginScreen>(font, *authService);
    registerScreen = std::make_unique<RegisterScreen>(font, *authService);
    // bookingScreen / detailScreen sẽ được khởi tạo khi vào state tương ứng
}

void App::handleStateChange() {
    if (state == previousState) return;

    switch (state) {
        case AppState::MOVIE_DETAILS:
            // Khởi tạo DetailScreen từ dữ liệu trong MovieRepository
            if (homeScreen && homeScreen->getRepository()) {
                int selectedIndex = homeScreen->getSelectedMovieIndex();
                if (selectedIndex >= 0) {
                    MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(selectedIndex);
                    detailScreen = std::make_unique<DetailScreen>(font, detail);
                }
            }
            break;

        case AppState::BOOKING:
            // Nếu đi từ MOVIE_DETAILS và đã có detailScreen, ưu tiên lấy movieId từ đó
            if (detailScreen && previousState == AppState::MOVIE_DETAILS) {
                String movieId = detailScreen->getMovieId();
                bookingScreen = std::make_unique<BookingScreen>(font, movieId);
            } else {
                // Mặc định khởi tạo rỗng (lọc theo tất cả)
                bookingScreen = std::make_unique<BookingScreen>(font, "");
            }
            break;

        default:
            break;
    }

    previousState = state;
}

void App::run() {
    while (window.isOpen()) {
        bool mousePressed = false;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        // --- Event loop DUY NHẤT ---
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
            if (event->is<Event::MouseButtonPressed>()) mousePressed = true;

            // chuyển các event ký tự / text cho Login & Register (đúng chữ ký cũ)
            if (state == AppState::LOGIN) {
                // LoginScreen::update(mousePos, mousePressed, event, currentUser, currentUserEmail, state)
                if (loginScreen->update(mousePos, mousePressed, *event, currentUser, currentUserEmail, state)) {
                    // đăng nhập thành công
                    BaseScreen::setLoggedInUser(currentUser, currentUserEmail);  // ✅ Lưu cả username VÀ email
                    homeScreen->setLoggedUser(currentUser); // đổi nút "Đăng nhập | Đăng ký" -> "Xin chào, ..."
                    state = AppState::HOME;
                }
            }
            else if (state == AppState::REGISTER) {
                // RegisterScreen::update(mousePos, mousePressed, event)
                if (registerScreen->update(mousePos, mousePressed, *event)) {
                    state = AppState::LOGIN;
                }
            }
        }

        // --- Update theo state ---
        switch (state) {
            case AppState::HOME:
                // Đồng bộ nút account trên header của BaseScreen qua HomeScreen
                homeScreen->setLoggedUser(
                    authService->isLoggedIn() ? authService->getCurrentUser()->username : ""
                );
                homeScreen->update(mousePos, mousePressed, state);
                homeScreen->handleEvent(mousePos, mousePressed, state);  // ✅ Thêm để slider nhận sự kiện
                break;

            case AppState::MOVIE_DETAILS:
                if (detailScreen)
                    detailScreen->update(mousePos, mousePressed, state);
                break;

            case AppState::BOOKING:
                if (bookingScreen)
                    bookingScreen->update(mousePos, mousePressed, state);
                break;

            case AppState::LOGIN:
                // phần nhập liệu đã xử lý trong event loop
                break;

            case AppState::REGISTER:
                // phần nhập liệu đã xử lý trong event loop
                break;

            default:
                break;
        }

        handleStateChange();

        // --- Draw ---
        window.clear(Color::White);
        switch (state) {
            case AppState::HOME:
                homeScreen->draw(window);
                break;
            case AppState::MOVIE_DETAILS:
                if (detailScreen) detailScreen->draw(window);
                break;
            case AppState::BOOKING:
                if (bookingScreen) bookingScreen->draw(window);
                break;
            case AppState::LOGIN:
                loginScreen->draw(window);
                break;
            case AppState::REGISTER:
                registerScreen->draw(window);
                break;
            default:
                break;
        }
        window.display();
    }
}
