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
      detail_font("../assets/fonts/quicksand_medium.ttf"),
      title_font("../assets/fonts/BEBAS_NEUE_ZSMALL.ttf"),
      state(AppState::HOME),
      previousState(AppState::HOME)
{
    window.setFramerateLimit(60);

    // ✅ Maintain showtimes: Remove expired ones and add new ones for next 7 days
    ShowtimeCleanupService::maintainShowtimes("../data/showtimes.txt", 7);

    authService    = std::make_unique<AuthService>("../data/users.txt");
    authService->ensureSampleUser();

    homeScreen     = std::make_unique<HomeScreen>(font, window);
    loginScreen    = std::make_unique<LoginScreen>(font, *authService);
    registerScreen = std::make_unique<RegisterScreen>(font, *authService);
    accountScreen  = std::make_unique<AccountScreen>(font, *authService);
    // bookingScreen / detailScreen sẽ được khởi tạo khi vào state tương ứng
}

void App::handleStateChange() {
    // ✅ Xử lý state MOVIE_DETAILS đặc biệt: cần check ngay cả khi state không đổi
    if (state == AppState::MOVIE_DETAILS) {
        // Chỉ tạo DetailScreen từ HomeScreen nếu chưa có DetailScreen từ search
        // Điều này tránh ghi đè lên DetailScreen vừa được tạo từ search trong update loop
        if (homeScreen && homeScreen->getRepository()) {
            int selectedIndex = homeScreen->getSelectedMovieIndex();
            if (selectedIndex >= 0) {
                // Có movie mới được chọn từ poster/home → tạo DetailScreen mới
                MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(selectedIndex);
                detailScreen = std::make_unique<DetailScreen>(font, font, title_font, detail_font, detail);
                // Clear selected index sau khi dùng
                homeScreen->clearSelectedMovieIndex();
            }
        }
        
        // ✅ CRITICAL FIX: Nếu không có selectedIndex từ HomeScreen và không có DetailScreen
        // (tức là search từ screen khác nhưng update loop chưa tạo DetailScreen)
        // thì KHÔNG làm gì, tránh giữ DetailScreen cũ hoặc tạo màn hình trắng
    }
    
    // Xử lý các state transition khác
    if (state == previousState) return;

    switch (state) {
        case AppState::MOVIE_DETAILS:
            // Đã xử lý ở trên
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
            else if (state == AppState::HOME) {
                // ✅ Truyền event cho HomeScreen để xử lý SearchBox
                homeScreen->handleEvent(mousePos, mousePressed, state, &(*event));
            }
            else if (state == AppState::MOVIE_DETAILS) {
                // ✅ Truyền event cho DetailScreen để xử lý GlobalSearchBar
                if (detailScreen) {
                    detailScreen->handleEvent(*event);
                }
            }
            else if (state == AppState::BOOKING) {
                // ✅ Truyền event cho BookingScreen để xử lý GlobalSearchBar
                if (bookingScreen) {
                    bookingScreen->handleEvent(*event);
                }
            }
            else if (state == AppState::ACCOUNT) {
                // ✅ Truyền MỖI event cho AccountScreen để xử lý text input
                if (accountScreen) {
                    accountScreen->update(mousePos, mousePressed, &(*event), state);
                }
            }
        }

        // --- ✅ CRITICAL FIX: Save state BEFORE checking search results ---
        // State might change during event processing, so we need to check all screens
        AppState stateBeforeEventProcessing = state;
        
        // --- ✅ Check search results from ALL screens (state might have changed) ---
        bool movieSelectedFromSearch = false;
        int globalSearchIdx = -1;
        
        // Check ALL screens, not just current one (because state might have changed)
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
        
        // If movie was selected from search, create DetailScreen immediately
        if (movieSelectedFromSearch && homeScreen && homeScreen->getRepository()) {
            MovieDetail detail = homeScreen->getRepository()->getMovieDetailbyIndex(globalSearchIdx);
            detailScreen = std::make_unique<DetailScreen>(font, font, title_font, detail_font, detail);
            // If coming from BookingScreen, reset it
            if (state == AppState::BOOKING || previousState == AppState::BOOKING) {
                bookingScreen.reset();
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
                // handleEvent đã được gọi trong event loop
                break;

        case AppState::MOVIE_DETAILS:
            if (detailScreen) {
                detailScreen->update(mousePos, mousePressed, state);
                // Search handling moved to before switch statement
            }
            break;
            
            case AppState::BOOKING:
                if (bookingScreen) {
                    bookingScreen->update(mousePos, mousePressed, state);
                    // Search handling moved to before switch statement
                }
                break;

            case AppState::ACCOUNT:
                // ✅ Update AccountScreen
                if (accountScreen) {
                    // ✅ ALWAYS set current user when in ACCOUNT state to ensure data is loaded
                    string email = BaseScreen::getLoggedInUserEmail();
                    if (!email.empty()) {
                        accountScreen->setCurrentUser(email);
                    }
                    // Gọi update với nullptr (không có event mới) để update hover, cursor, etc
                    accountScreen->update(mousePos, mousePressed, nullptr, state);
                    // Search handling moved to before switch statement
                }
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
            default:
                break;
        }
        window.display();
    }
}
