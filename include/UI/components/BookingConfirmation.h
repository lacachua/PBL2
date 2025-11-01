#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

class BookingConfirmation {
private:
    Font& detailFont;
    Font& buttonFont;
    
public:
    BookingConfirmation(Font& detail, Font& button);
    
    void draw(RenderWindow& window, const FloatRect& contentArea,
              const string& bookingCode, int movieId, 
              const string& date, const string& time, const string& room,
              const vector<string>& seats);
};