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
    public:
        BaseScreen(Font& f) 
            :   font(f),
                background_tex("../assets/elements/background.png"), 
                searchBar_tex("../assets/elements/search_bar.png"),
                background_sprite(background_tex),
                searchBar_sprite(searchBar_tex)
        {
            setupUI();
        }

        virtual ~BaseScreen() = default;

        virtual void setupUI() {
            buttons.push_back(TextButton(font, L"CiNeXíNè", 50, {60.f, 40.f}));
            buttons.push_back(TextButton(font, L"Đặt vé ngay", 23, {1150.f, 50.f}));
            buttons.push_back(TextButton(font, L"Đăng nhập | Đăng ký", 22, {1360.f, 50.f}));
            for (int i = 0; i < buttons.getSize(); i++) {
                buttons[i].setOutlineThickness(i == 0 ? 5.f : 2.f);
                buttons[i].setOutlineColor(Color(20, 118, 172));
            }
            searchBar_sprite.setScale({0.2f, 0.2f});
            searchBar_sprite.setPosition({600.f, 50.f});
        }

        virtual void update(Vector2f mousePos, bool mousePressed, AppState& state) {
            for (int i = 0; i < buttons.getSize(); i++) 
                buttons[i].update(mousePos);
        }

        virtual void draw(RenderWindow& window) {
            window.draw(background_sprite);
            window.draw(searchBar_sprite);
            for (int i = 0; i < buttons.getSize(); i++)
                buttons[i].draw(window);
        }
};