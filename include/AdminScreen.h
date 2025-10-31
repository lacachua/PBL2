#pragma once
#include <SFML/Graphics.hpp>
#include "Movie.h"
#include "DoublyLinkedList.h"
#include "Button.h"
#include "AdminMovieForm.h"
#include <locale>
#include <codecvt>

enum class AdminView;

class AdminScreen {
private:
    Font uppercase_font, lowercase_font;  
    RectangleShape sidebar;
    RectangleShape admin_logo; Text admin_logoText;
    RenderWindow* windowPtr = nullptr;
    vector<Button> menuItems;
    vector<bool> expanded;
    vector<vector<Button>> subItems;
    Texture arrow_down, arrow_up;
    Sprite arrow_down_tex, arrow_up_tex;

    RectangleShape contentArea;
    AdminView currentView;
    vector<Movie> movies;

    // Movie form
    AdminMovieForm* movieForm;

    // Form state
    int currentPage = 0;
    int itemsPerPage = 10;
    int selectedMovieIndex = -1;
    
    // Search
    wstring searchText = L"";
    bool isSearching = false;

    // Helper
    wstring stringToWString(const string& str);
    
public:
    AdminScreen(RenderWindow&);
    ~AdminScreen();
    void draw(RenderWindow&);
    void update(Vector2f, bool);
    void handleEvent(const Event& event, Vector2f mousePos, bool mousePressed);

    void loadMovies(const string&);
    void saveMovies(const string&);
    void drawBreadcrumb(RenderWindow& window, const wstring& path);
    void drawMovieList(RenderWindow&);
    void drawAddMovieForm(RenderWindow&);
    void drawEditMovieForm(RenderWindow&);
    void drawDeleteMovieForm(RenderWindow&);
};

