#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ui/components/SnackMenu.h"

using namespace sf;
using namespace std;

class PaymentSummary {
private:
    Font& detailFont;
    Font& buttonFont;
    
public:
    PaymentSummary(Font& detail, Font& button);
    
    void draw(RenderWindow& window, const FloatRect& contentArea,
              int movieId, const string& date, const string& time, const string& room,
              const vector<string>& seats, int seatPrice,
              const SnackMenu& snackMenu);
};