#pragma once
#include "TextButton.h"
#include "AppState.h"
#include "SearchBox.h"
#include "MovieSearchManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;

class HomeScreen {
    protected:
        Font& font;
        Texture background, searchbar;
        Sprite sprite1, sprite2;

        bool showDropdown = false;
        RectangleShape dropdownBox;
        TextButton myticketsButton, accountButton, logoutButton;

        string currentUser;
        bool isUserLoggedIn = false;
        
        SearchBox* searchBox;
        MovieSearchManager* searchManager;
        int selectedMovieIndex;
    public:
        static const int BUTTON_COUNT = 3;
        TextButton buttons[BUTTON_COUNT];
        HomeScreen(Font&);
        ~HomeScreen();
        void initializeSearch(const vector<Movie>& movies);
        void update(Vector2f, bool, AppState&, const Event* event = nullptr);
        void draw(RenderWindow&);
        void drawSearchBox(RenderWindow&);
        void drawDropdown(RenderWindow& window);
        void setLoggedUser(const string& username);
        void updateDropdownPosition();
        int getSelectedMovieIndex() const { return selectedMovieIndex; }
        void clearSelectedMovieIndex() { selectedMovieIndex = -1; }
};