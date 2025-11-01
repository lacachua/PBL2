#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

class SeatSelector {
private:
    static const int SEAT_ROWS = 9;
    static const int SEAT_COLS = 9;
    
    vector<string> selectedSeats;
    vector<string> occupiedSeats;
    Font& font;
    
    bool isSeatOccupied(const string& seat) const;
    bool isSeatSelected(const string& seat) const;
    
public:
    SeatSelector(Font& f);
    
    void loadOccupiedSeats(const string& seat_map);
    void handleClick(const Vector2f& mousePos, const FloatRect& contentArea);
    void draw(RenderWindow& window, const FloatRect& contentArea);
    void drawSummary(RenderWindow& window, const FloatRect& summaryArea, 
                     const string& date, const string& time, const string& room, int price);
    
    const vector<string>& getSelectedSeats() const { return selectedSeats; }
    void clearSelection() { selectedSeats.clear(); }
    
    string generateSeatMap() const;
};