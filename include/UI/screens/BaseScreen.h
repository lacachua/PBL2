#pragma once
#include <SFML/Graphics.hpp>
#include "core/AppState.h"
#include "UI/components/TextButton.h"
#include "UI/components/GlobalSearchBar.h"
#include "services/MovieSearchManager.h"
#include "data-structures/DLL.h"
#include <memory>
using namespace std;
using namespace sf;

class BaseScreen {
    protected:
        Font& font;
        Texture background_tex;
        Sprite background_sprite;
        Texture searchBar_tex;
        Sprite searchBar_sprite;
        DLL<TextButton> buttons;
        
        // ✅ Global search bar for all screens
        unique_ptr<GlobalSearchBar> globalSearchBar;
        unique_ptr<MovieSearchManager> globalSearchManager;
        
        // ✅ Track selected movie from search
        int selectedMovieIndexFromSearch = -1;
        
        // ✅ Static variables để lưu trạng thái đăng nhập chung cho tất cả screen
        static string loggedInUsername;
        static string loggedInUserEmail;
        
        // Dropdown menu components
        bool showDropdown = false;
        RectangleShape dropdownBox;
        TextButton accountButton;
        TextButton logoutButton;
    public:
        BaseScreen(Font& f) 
            :   font(f),
                background_tex("../assets/elements/background.png"), 
                searchBar_tex("../assets/elements/search_bar.png"),
                background_sprite(background_tex),
                searchBar_sprite(searchBar_tex),
                accountButton(f, L"Thông tin cá nhân", 18, {0.f, 0.f}),
                logoutButton(f, L"Đăng xuất", 18, {0.f, 0.f})
        {
            buttons.push_back(TextButton(font, L"CiNeXíNè", 50, {90.f, 40.f}));
            buttons.push_back(TextButton(font, L"Đặt vé ngay", 23, {1150.f, 50.f}));
            buttons.push_back(TextButton(font, L"Đăng nhập | Đăng ký", 22, {1360.f, 50.f}));
            for (int i = 0; i < buttons.getSize(); i++) {
                buttons[i].setOutlineThickness(i == 0 ? 5.f : 2.f);
                buttons[i].setOutlineColor(Color(20, 118, 172));
            }
            searchBar_sprite.setScale({0.2f, 0.2f});
            searchBar_sprite.setPosition({700.f, 50.f});
            
            // Setup dropdown box
            dropdownBox.setSize({250.f, 100.f});
            dropdownBox.setFillColor(Color(40, 40, 40, 240));
            dropdownBox.setOutlineColor(Color(100, 100, 100));
            dropdownBox.setOutlineThickness(1.f);
            
            // Setup dropdown buttons (styled)
            accountButton.setOutlineThickness(0.f);
            logoutButton.setOutlineThickness(0.f);
            
            // ✅ Initialize global search bar
            FloatRect searchBarBounds = searchBar_sprite.getGlobalBounds();
            float searchBoxX = searchBarBounds.position.x + 40.f;
            float searchBoxY = searchBarBounds.position.y + 8.f;
            float searchBoxWidth = searchBarBounds.size.x - 50.f;
            
            globalSearchBar = make_unique<GlobalSearchBar>(font, Vector2f(searchBoxX, searchBoxY), Vector2f(searchBoxWidth, 40.f));
        }

        virtual ~BaseScreen() = default;

        virtual void update(Vector2f mousePos, bool mousePressed, AppState& state) {
            // ✅ Handle global search bar updates
            if (globalSearchBar) {
                globalSearchBar->update(mousePos, mousePressed);
                
                // Check if a movie was selected from search
                int movieIdx;
                if (globalSearchBar->hasSelectedMovie(movieIdx)) {
                    selectedMovieIndexFromSearch = movieIdx;
                    state = AppState::MOVIE_DETAILS;
                    return;
                }
            }
            
            // Don't process other UI if search box is active
            if (globalSearchBar && globalSearchBar->isInputActive()) {
                return;
            }
            
            // ✅ Tự động cập nhật text nút đăng nhập dựa vào trạng thái
            if (isUserLoggedIn())
                buttons[2].setString(L"Xin chào, " + String::fromUtf8(loggedInUsername.begin(), loggedInUsername.end()) + L"!");
            else
                buttons[2].setString(L"Đăng nhập | Đăng ký");
            
            // ✅ Xử lý dropdown menu nếu user đã đăng nhập
            if (isUserLoggedIn() && showDropdown) {
                updateDropdownPosition();
                accountButton.update(mousePos);
                logoutButton.update(mousePos);
                
                if (mousePressed) {
                    if (accountButton.isClicked(mousePos, mousePressed)) {
                        state = AppState::ACCOUNT;
                        showDropdown = false;
                        return;
                    }
                    else if (logoutButton.isClicked(mousePos, mousePressed)) {
                        handleLogout();
                        return;
                    }
                    // Click bên ngoài dropdown -> đóng dropdown
                    else if (!dropdownBox.getGlobalBounds().contains(mousePos) && !buttons[2].getGlobalBounds().contains(mousePos)) {
                        showDropdown = false;
                    }
                }
            }
            
            for (int i = 0; i < buttons.getSize(); i++) {
                buttons[i].update(mousePos);
                
                // Handle button clicks
                if (buttons[i].isClicked(mousePos, mousePressed)) {
                    switch (i) {
                        case 0:  // "CiNeXíNè" logo - go to HOME
                            state = AppState::HOME;
                            break;
                        case 1:  // "Đặt vé ngay" - go to BOOKING
                            if (state != AppState::BOOKING) {
                                state = AppState::BOOKING;
                            }
                            break;
                        case 2:  // "Đăng nhập | Đăng ký" hoặc "Xin chào, ..."
                            if (isUserLoggedIn()) {
                                // Toggle dropdown menu
                                showDropdown = !showDropdown;
                                if (showDropdown) {
                                    updateDropdownPosition();
                                }
                            } else {
                                state = AppState::LOGIN;
                            }
                            break;
                    }
                }
            }
        }
        
        // ✅ Method to handle events (for search bar keyboard input)
        virtual void handleEvent(const Event& event) {
            if (globalSearchBar) {
                globalSearchBar->handleEvent(event);
            }
        }

        virtual void draw(RenderWindow& window) {
            window.draw(background_sprite);
            window.draw(searchBar_sprite);
            for (int i = 0; i < buttons.getSize(); i++)
                buttons[i].draw(window);
            
            // NOTE: GlobalSearchBar will be drawn separately in drawOverlay() to ensure it's on top
            
            // ✅ Draw dropdown menu nếu đang hiển thị
            if (isUserLoggedIn() && showDropdown) {
                window.draw(dropdownBox);
                accountButton.draw(window);
                logoutButton.draw(window);
            }
        }
        
        // ✅ Draw UI overlay elements that should be on top of everything
        virtual void drawOverlay(RenderWindow& window) {
            // Draw global search bar and suggestions on top of all other content
            if (globalSearchBar) {
                globalSearchBar->draw(window);
            }
        }

        void setAccountButtonText(const String& text) { buttons[2].setString(text); }
        
        // ✅ Initialize search functionality (to be called by derived classes with movie data)
        void initializeGlobalSearch(const DLL<MovieDetail>& movies) {
            if (!globalSearchManager) {
                globalSearchManager = make_unique<MovieSearchManager>();
            }
            globalSearchManager->loadMovies(movies);
            if (globalSearchBar) {
                globalSearchBar->setSearchManager(globalSearchManager.get());
            }
        }
        
        // ✅ Get selected movie index from global search
        int getSelectedMovieIndexFromSearch() const {
            return selectedMovieIndexFromSearch;
        }
        
        // ✅ Clear selected movie index after use
        void clearSelectedMovieIndexFromSearch() {
            selectedMovieIndexFromSearch = -1;
        }
        
        // ✅ Static methods để set/get username và email cho tất cả screens
        static void setLoggedInUser(const string& username, const string& email) { 
            loggedInUsername = username;
            loggedInUserEmail = email;
        }
        static string getLoggedInUser() { 
            return loggedInUsername;  // Trả về username (hiển thị)
        }
        static string getLoggedInUserEmail() { 
            return loggedInUserEmail;  // Trả về email đầy đủ (so sánh database)
        }
        static bool isUserLoggedIn() { 
            return !loggedInUsername.empty(); 
        }
        
        // ✅ Method để logout user
        static void handleLogout() {
            loggedInUsername = "";
            loggedInUserEmail = "";
        }
        
    protected:
        // ✅ Update vị trí dropdown ngay bên dưới nút "Xin chào"
        void updateDropdownPosition() {
            FloatRect loginButtonBounds = buttons[2].getGlobalBounds();
            float dropdownX = loginButtonBounds.position.x + loginButtonBounds.size.x - dropdownBox.getSize().x;
            float dropdownY = loginButtonBounds.position.y + loginButtonBounds.size.y + 10.f;
            
            dropdownBox.setPosition({dropdownX, dropdownY});
            accountButton.setPosition({dropdownX + 15.f, dropdownY + 15.f});
            logoutButton.setPosition({dropdownX + 15.f, dropdownY + 55.f});
        }
};