// #pragma once
// #include "UI/components/TextButton.h"
// #include "core/AppState.h"
// #include "UI/components/SearchBox.h"
// #include "services/MovieSearchManager.h"
// #include <SFML/Graphics.hpp>
// #include <vector>
// #include <string>

// using namespace sf;

// class HomeScreen {
//     protected:
//         Font& font;
//         Texture background, searchbar;
//         Sprite sprite1, sprite2;

//         bool showDropdown = false;
//         RectangleShape dropdownBox;
//         TextButton accountButton, logoutButton;

//         string currentUser;
//         bool isUserLoggedIn = false;
        
//         SearchBox* searchBox;
//         MovieSearchManager* searchManager;
//         int selectedMovieIndex;
//     public:
//         static const int BUTTON_COUNT = 3;
//         TextButton buttons[BUTTON_COUNT];
//         HomeScreen(Font&);
//         ~HomeScreen();
//         void initializeSearch(const vector<Movie>& movies);
//         void update(Vector2f, bool, AppState&, const Event* event = nullptr);
//         void draw(RenderWindow&);
//         void drawSearchBox(RenderWindow&);
//         void drawDropdown(RenderWindow& window);
//         void setLoggedUser(const string& username);
//         void updateDropdownPosition();
//         int getSelectedMovieIndex() const { return selectedMovieIndex; }
//         void clearSelectedMovieIndex() { selectedMovieIndex = -1; }
// };

#pragma once
#include "BaseScreen.h"
#include "repositories/MovieRepository.h"
#include "core/AppState.h"
#include "UI/components/PosterSlider/PosterSlider.h"
#include <memory>

class HomeScreen : public BaseScreen {
private:
    RenderWindow& win;
    unique_ptr<PosterSlider> slider;
    unique_ptr<MovieRepository> repo;
    Clock clock;

    bool isLoggedIn = false;
    string currentUser;

public:
    HomeScreen(Font&, RenderWindow&);
    void update(Vector2f, bool, AppState&) override;
    void handleEvent(Vector2f, bool, AppState&, const Event* event = nullptr);  // âœ… ThÃªm Event parameter
    void draw(RenderWindow&) override;
    void drawHeaderOnly(RenderWindow&);  // âœ… Váº½ chá»‰ header khÃ´ng cÃ³ slider

    // âœ… setter Ä‘á»ƒ App.cpp bÃ¡o user Ä‘Ã£ Ä‘Äƒng nháº­p
    void setLoggedUser(const string& username);
    
    // âœ… Getters Ä‘á»ƒ App láº¥y thÃ´ng tin phim Ä‘Ã£ chá»n
    MovieRepository* getRepository() const { return repo.get(); }
    int getSelectedMovieIndex() const { 
        // Check if movie selected from global search first
        int searchIdx = getSelectedMovieIndexFromSearch();
        if (searchIdx >= 0) return searchIdx;
        // Otherwise return from repository
        return repo ? repo->getSelectedIndex() : -1; 
    }
    void clearSelectedMovieIndex() { 
        clearSelectedMovieIndexFromSearch();
        if (repo) repo->setSelectedIndex(-1);
    }
};

