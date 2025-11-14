#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "UI/components/TicketBooking/TicketRepository.h"
#include <filesystem>
using namespace sf;
using namespace std;

class PurchaseHistoryView {
private:
    const Font& font;
    string currentUserEmail;
    vector<Ticket> userTickets;
    
    // UI Components
    Text titleText;
    RectangleShape scrollableArea;
    
    // Pagination
    int currentPage = 0;
    int itemsPerPage = 5;
    RectangleShape prevButton, nextButton;
    Text prevButtonText, nextButtonText, pageInfoText;
    std::filesystem::file_time_type lastTicketsWriteTime{};
    bool hasLoadedTickets = false;
     const std::filesystem::path ticketsFilePath = "../data/tickets.txt";
    
public:
    PurchaseHistoryView(const Font& f);
    
    void setUserEmail(const string& email);
     void loadTickets(bool preservePage = false);
    void update(Vector2f mousePos, bool mousePressed, Vector2f cardPos);
    void draw(RenderWindow& window, Vector2f cardPos);
    
private:
    int getTotalPages() const;
    vector<Ticket> getCurrentPageTickets() const;
     void refreshTicketsIfChanged();
};
