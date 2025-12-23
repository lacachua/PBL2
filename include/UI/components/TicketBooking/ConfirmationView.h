#pragma once
#include <SFML/Graphics.hpp>
#include "repositories/booking/TicketRepository.h"
#include "BookingState.h"  // Su dung BookingData
#include "../Button.h"
#include "core/AppState.h"
using namespace sf;

class ConfirmationView {
private:
    Font& font;
    Button homeButton;
    
    // Tieu de hien thi
    Text titleText;
    
    // Nhan ben trai (Mo ta)
    Text labelTicketId;
    Text labelCustomer;
    Text labelEmail;
    Text labelMovie;
    Text labelRoom;
    Text labelDateTime;
    Text labelSeats;
    Text labelCombo;
    Text labelTotal;
    
    // Gia tri ben phai (Thong tin)
    Text valueTicketId;
    Text valueCustomer;
    Text valueEmail;
    Text valueMovie;
    Text valueRoom;
    Text valueDateTime;
    Text valueSeats;
    Text valueCombo;
    Text valueTotal;
    
    void initializeUI();
    
public:
    ConfirmationView(Font& f);
    void setBookingData(const BookingData& data);
    
    void update(Vector2f mousePos, bool mousePressed);
    bool handleHomeButtonClick(Vector2f mousePos, bool mousePressed, AppState& state);
    void draw(RenderWindow& window);
};
