#pragma once
#include <SFML/Graphics.hpp>
#include "TicketRepository.h"
#include "../Button.h"
#include "core/AppState.h"
using namespace sf;

class ConfirmationView {
private:
    Font& font;
    Button homeButton;
    
    // Ticket info
    Ticket currentTicket;
    string userName;
    string userPhone;
    
    // Display texts - Title
    Text titleText;
    
    // Labels (Cột Mô tả)
    Text labelTicketId;
    Text labelCustomer;
    Text labelEmail;
    Text labelMovie;
    Text labelRoom;
    Text labelDateTime;
    Text labelSeats;
    Text labelCombo;
    Text labelTotal;
    
    // Values (Cột Thông tin)
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
    
    void setTicketData(
        const Ticket& ticket,
        const string& userName,
        const string& userPhone,
        const string& movieName,
        const string& roomName,
        const string& date,
        const string& time
    );
    
    void update(Vector2f mousePos, bool mousePressed);
    bool handleHomeButtonClick(Vector2f mousePos, bool mousePressed, AppState& state);
    void draw(RenderWindow& window);
};
