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

void App::resetAfterLogout() {
    // ✅ Reset AuthService to clear session
    authService->logout();
    
    // ✅ Reset all screens to nullptr (unique_ptr automatically deletes)
    detailScreen.reset();
    bookingScreen.reset();
    accountScreen.reset();
    
    // ✅ Recreate login and register screens to clear input fields
    loginScreen = std::make_unique<LoginScreen>(font, *authService);
    registerScreen = std::make_unique<RegisterScreen>(font, *authService);
    accountScreen = std::make_unique<AccountScreen>(font, *authService);
    
    // ✅ Reset homeScreen to initial state (this will recreate it)
    homeScreen.reset();
    homeScreen = std::make_unique<HomeScreen>(font, window);
    
    // ✅ Clear current user info
    currentUser = "";
    currentUserEmail = "";
    
    // ✅ Reset state to HOME
    state = AppState::HOME;
    previousState = AppState::HOME;
    
    // ✅ Clear the logout flag
    BaseScreen::clearLogoutFlag();
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
                    // state đã được set bởi LoginScreen dựa trên role
                    // Không cần ghi đè lại state = AppState::HOME
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
            else if (state == AppState::ADMIN_DASHBOARD || 
                     state == AppState::ADMIN_MOVIES ||
                     state == AppState::ADMIN_ROOMS ||
                     state == AppState::ADMIN_SHOWTIMES ||
                     state == AppState::ADMIN_TICKETS ||
                     state == AppState::ADMIN_COMBOS ||
                     state == AppState::ADMIN_STAFF ||
                     state == AppState::ADMIN_CUSTOMERS ||
                     state == AppState::ADMIN_REVENUE ||
                     state == AppState::ADMIN_SOLD_TICKETS ||
                     state == AppState::ADMIN_CHANGE_PASSWORD) {
                // ✅ Admin screens
                if (!adminScreen) {
                    adminScreen = std::make_unique<AdminScreen>(font, window, state, authService.get());
                }
                adminScreen->handleEvent(*event);
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
                    authService->isLoggedIn() ? authService->getCurrentUser()->getUsername() : ""
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

            case AppState::ADMIN_DASHBOARD:
            case AppState::ADMIN_MOVIES:
            case AppState::ADMIN_ROOMS:
            case AppState::ADMIN_SHOWTIMES:
            case AppState::ADMIN_TICKETS:
            case AppState::ADMIN_COMBOS:
            case AppState::ADMIN_STAFF:
            case AppState::ADMIN_CUSTOMERS:
            case AppState::ADMIN_REVENUE:
            case AppState::ADMIN_SOLD_TICKETS:
            case AppState::ADMIN_CHANGE_PASSWORD:
                if (adminScreen) {
                    adminScreen->update(mousePos, mousePressed, state);
                }
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
            case AppState::ADMIN_STAFF:
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
