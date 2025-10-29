#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Showtime.h"
#include "Movie.h"
using namespace sf;
using namespace std;

class AdminScreen {
public:
    AdminScreen(const Font& font, vector<Showtime>& showtimes, const vector<Movie>& movies);
    void update(Vector2f mouse, bool mousePressed, const Event& event);
    void draw(RenderWindow& window);
    bool shouldClose() const { return closeRequested; }

private:
    const Font& font;
    vector<Showtime>& showtimes;
    const vector<Movie>& movies;
    
    // UI State
    enum class Mode { VIEW, ADD, EDIT, DELETE };
    Mode currentMode = Mode::VIEW;
    int selectedIndex = -1;
    int scrollOffset = 0;
    
    // UI Elements
    RectangleShape background;
    RectangleShape sidebar;
    RectangleShape contentArea;
    RectangleShape scrollbar;
    
    Text title;
    Text btnView, btnAdd, btnEdit, btnDelete, btnBack;
    
    // Form fields for ADD/EDIT
    Text labelMovieId, labelDate, labelTime, labelRoom, labelPrice;
    RectangleShape inputMovieId, inputDate, inputTime, inputRoom, inputPrice;
    Text textMovieId, textDate, textTime, textRoom, textPrice;
    wstring dataMovieId, dataDate, dataTime, dataRoom, dataPrice;
    int activeField = -1; // 0=movieId, 1=date, 2=time, 3=room, 4=price
    
    Text btnSave, btnCancel;
    Text message;
    
    // Caret for text input
    RectangleShape caret;
    Clock caretClock;
    bool caretVisible = true;
    
    bool closeRequested = false;
    
    // Helper methods
    void drawViewMode(RenderWindow& window);
    void drawAddEditMode(RenderWindow& window);
    void drawDeleteMode(RenderWindow& window);
    
    void handleViewMode(Vector2f mouse, bool mousePressed, const Event& event);
    void handleAddEditMode(Vector2f mouse, bool mousePressed, const Event& event);
    void handleDeleteMode(Vector2f mouse, bool mousePressed, const Event& event);
    
    void saveShowtime();
    void deleteShowtime();
    void clearForm();
    void loadShowtimeToForm(int index);
    
    string getMovieTitle(int movieId) const;
    void saveShowtimesToCSV();
};
