#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "MovieSearchManager.h"

using namespace sf;
using namespace std;

class SearchBox {
private:
    RectangleShape inputBox;
    RectangleShape suggestionBox;
    Text inputText;
    Text placeholderText;
    vector<Text> suggestionTexts;
    
    string inputString;
    bool isActive;
    bool showSuggestions;
    int selectedSuggestion;
    vector<SearchResult> suggestions;
    
    Font& font;
    MovieSearchManager* searchManager;
    
    Clock blinkClock;
    bool showCursor;
    
public:
    SearchBox(Font& f, Vector2f position, Vector2f size);
    void setSearchManager(MovieSearchManager* manager);
    void handleEvent(const Event& event);
    void update(Vector2f mousePos, bool mousePressed);
    void draw(RenderWindow& window);
    bool hasSelectedMovie(int& movieIndex);
    void clear();
    bool isInputActive() const { return isActive; }
    string getInput() const { return inputString; }
};
