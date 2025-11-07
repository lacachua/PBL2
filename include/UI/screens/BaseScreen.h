#pragma once
#include <SFML/Graphics.hpp>
#include "core/AppState.h"
#include "UI/components/TextButton.h"
#include "data-structures/DLL.h"
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
        
        // ✅ Static variables để lưu trạng thái đăng nhập chung cho tất cả screen
        static string loggedInUsername;  // Username để hiển thị (không có @gmail.com)
        static string loggedInUserEmail;  // Email đầy đủ để so sánh với database
    public:
        BaseScreen(Font& f) 
            :   font(f),
                background_tex("../assets/elements/background.png"), 
                searchBar_tex("../assets/elements/search_bar.png"),
                background_sprite(background_tex),
                searchBar_sprite(searchBar_tex)
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
        }

        virtual ~BaseScreen() = default;

        virtual void update(Vector2f mousePos, bool mousePressed, AppState& state) {
            // ✅ Tự động cập nhật text nút đăng nhập dựa vào trạng thái
            if (isUserLoggedIn()) {
                buttons[2].setString(L"Xin chào, " + String::fromUtf8(loggedInUsername.begin(), loggedInUsername.end()) + L"!");
            } else {
                buttons[2].setString(L"Đăng nhập | Đăng ký");
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
                            // Only change state if not already in BOOKING
                            if (state != AppState::BOOKING) {
                                state = AppState::BOOKING;
                            }
                            break;
                        case 2:  // "Đăng nhập | Đăng ký" hoặc "Xin chào, ..."
                            state = AppState::LOGIN;  // ✅ Chuyển sang màn hình đăng nhập
                            break;
                    }
                }
            }
        }

        virtual void draw(RenderWindow& window) {
            window.draw(background_sprite);
            window.draw(searchBar_sprite);
            for (int i = 0; i < buttons.getSize(); i++)
                buttons[i].draw(window);
        }

        void setAccountButtonText(const String& text) { buttons[2].setString(text); }
        
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
};