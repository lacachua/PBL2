#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

namespace SummaryPanelLayout {
    inline constexpr float PanelX = 1154.f;
    inline constexpr float PanelY = 220.f;
    inline constexpr float PanelWidth = 400.f;
}

class SummaryPanel {
private:
    Font& font;
public:
    SummaryPanel(Font&);
    
    // Vẽ cho state suất chiếu
    void draw(RenderWindow&, const String& movieName, const String& room, 
              const String& date, const String& time, int ticketPrice, bool hasSelectedShowtime);
    
    // Vẽ cho state ghế ngồi (thêm thông tin ghế)
    void drawWithSeats(RenderWindow&, const String& movieName, const String& room,
                       const String& date, const String& time, 
                       const String& selectedSeats, int totalPrice);
    void drawPayment(RenderWindow& window, const String& movieName, const String& room,
                     const String& date, const String& time, int finalTotal);
};
