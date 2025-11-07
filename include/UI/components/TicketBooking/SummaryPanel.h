#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class SummaryPanel {
private:
    Font& font;
public:
    SummaryPanel(Font&);
    
    // Draw cho state suatchieu
    void draw(RenderWindow&, const String& movieName, const String& room, 
              const String& date, const String& time, int ticketPrice, bool hasSelectedShowtime);
    
    // Draw cho state ghengoi (thêm thông tin ghế)
    void drawWithSeats(RenderWindow&, const String& movieName, const String& room,
                       const String& date, const String& time, 
                       const String& selectedSeats, int totalPrice);
    void drawPayment(RenderWindow& window, const String& movieName, const String& room, const String& date, const String& time, int finalTotal);
};
