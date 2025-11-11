#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "services/MovieSearchManager.h"

using namespace sf;
using namespace std;

/**
 * GlobalSearchBar - A reusable search component that can be used across all screens
 * Follows SOLID principles:
 * - Single Responsibility: Only handles search UI and user interaction
 * - Dependency Inversion: Depends on MovieSearchManager abstraction
 * - Open/Closed: Can be extended without modification
 */
class GlobalSearchBar {
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
    int hoveredSuggestion;  // Track which suggestion is hovered
    vector<SearchResult> suggestions;
    
    Font& font;
    MovieSearchManager* searchManager;
    
    Clock blinkClock;
    bool showCursor;
    
    // Store original position for repositioning
    Vector2f originalPosition;
    Vector2f originalSize;
    
public:
    /**
     * Constructor
     * @param f Font reference for text rendering
     * @param position Position of the search bar
     * @param size Size of the search bar
     */
    GlobalSearchBar(Font& f, Vector2f position, Vector2f size);
    
    /**
     * Set the search manager for movie searching
     * @param manager Pointer to MovieSearchManager instance
     */
    void setSearchManager(MovieSearchManager* manager);
    
    /**
     * Handle keyboard and mouse events
     * @param event SFML event to process
     */
    void handleEvent(const Event& event);
    
    /**
     * Update search bar state (cursor blink, hover effects)
     * @param mousePos Current mouse position
     * @param mousePressed Whether mouse button is pressed
     */
    void update(Vector2f mousePos, bool mousePressed);
    
    /**
     * Draw the search bar and suggestions
     * @param window SFML render window
     */
    void draw(RenderWindow& window);
    
    /**
     * Check if a movie has been selected and get its index
     * @param movieIndex Output parameter for selected movie index
     * @return true if a movie was selected
     */
    bool hasSelectedMovie(int& movieIndex);
    
    /**
     * Clear search input and hide suggestions
     */
    void clear();
    
    /**
     * Check if the search input is active (focused)
     * @return true if active
     */
    bool isInputActive() const { return isActive; }
    
    /**
     * Get the current input string
     * @return Current input string
     */
    string getInput() const { return inputString; }
    
    /**
     * Set the position of the search bar
     * @param position New position
     */
    void setPosition(Vector2f position);
};
