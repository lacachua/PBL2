#include "UI/components/AccountInfo/PurchaseHistoryView.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

using std::min;
using std::string;
using std::stringstream;
using std::vector;

static String utf8(const char* text) {
    size_t len = std::strlen(text);
    return String::fromUtf8(text, text + len);
}

static String utf8(const string& text) {
    return String::fromUtf8(text.begin(), text.end());
}

static string formatDateToDDMMYYYY(const string& dateStr) {
    if (dateStr.find('/') != string::npos) return dateStr;

    if (dateStr.length() >= 8) {
        string year, month, day;

        if (dateStr.find('-') != string::npos) {
            stringstream ss(dateStr);
            getline(ss, year, '-');
            getline(ss, month, '-');
            getline(ss, day, '-');
        } else {
            year = dateStr.substr(0, 4);
            month = dateStr.substr(4, 2);
            day = dateStr.substr(6, 2);
        }

        return day + "/" + month + "/" + year;
    }

    return dateStr;
}

static String formatPriceToString(int price) {
    string digits = to_string(price);
    string formatted;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        formatted.insert(formatted.begin(), digits[static_cast<size_t>(i)]);
        count++;
        if (count == 3 && i > 0) {
            formatted.insert(formatted.begin(), '.');
            count = 0;
        }
    }
    formatted += " VNĐ";
    return String::fromUtf8(formatted.begin(), formatted.end());
}

PurchaseHistoryView::PurchaseHistoryView(const Font& f)
        : font(f),
            titleText(f, utf8("LỊCH SỬ ĐẶT VÉ"), 22),
            prevButtonText(f, utf8("← Trước"), 16),
            nextButtonText(f, utf8("Tiếp →"), 16),
            pageInfoText(f, utf8(""), 16)
{
    titleText.setFillColor(Color(238, 238, 238));
    titleText.setStyle(Text::Bold);
    
    scrollableArea.setFillColor(Color(15, 30, 50, 200));
    scrollableArea.setOutlineColor(Color(50, 80, 120));
    scrollableArea.setOutlineThickness(1.f);
    
    // Setup pagination buttons
    prevButton.setSize({120.f, 40.f});
    nextButton.setSize({120.f, 40.f});
    prevButton.setFillColor(Color(0, 153, 255));
    nextButton.setFillColor(Color(0, 153, 255));
    
    prevButtonText.setFillColor(Color::White);
    nextButtonText.setFillColor(Color::White);
    pageInfoText.setFillColor(Color(200, 200, 200));
}

void PurchaseHistoryView::setUserEmail(const string& email) {
    currentUserEmail = email;
}

void PurchaseHistoryView::loadTickets() {
    userTickets.clear();
    
    ifstream file("../data/tickets.txt", ios::binary);
    if (!file.is_open()) return;
    
    // Read entire file
    string data((istreambuf_iterator<char>(file)), {});
    file.close();
    
    // Remove BOM if present
    if (data.size() >= 3 &&
        (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB &&
        (unsigned char)data[2] == 0xBF) {
        data.erase(0, 3);
    }
    
    stringstream ss(data);
    string line;
    getline(ss, line); // Skip header
    
    while (getline(ss, line)) {
        if (line.empty()) continue;
        
        stringstream ls(line);
        Ticket ticket;
        
        getline(ls, ticket.ticketId, '|');
        getline(ls, ticket.showtimeId, '|');
        getline(ls, ticket.title, '|');
        getline(ls, ticket.date, '|');
        getline(ls, ticket.time, '|');
        getline(ls, ticket.roomName, '|');
        getline(ls, ticket.booked, '|');
        getline(ls, ticket.comboName, '|');
        
        string priceStr;
        getline(ls, priceStr, '|');
        try {
            ticket.price = stoi(priceStr);
        } catch (...) {
            ticket.price = 0;
        }
        
        getline(ls, ticket.email, '|');
        getline(ls, ticket.fullName, '|');
        getline(ls, ticket.bookedDate, '|');
        getline(ls, ticket.bookedTime, '|');
        
        // Only add tickets for current user
        if (ticket.email == currentUserEmail) {
            userTickets.push_back(ticket);
        }
    }
    
    // Sort by booked date/time (newest first)
    sort(userTickets.begin(), userTickets.end(), [](const Ticket& a, const Ticket& b) {
        if (a.bookedDate != b.bookedDate)
            return a.bookedDate > b.bookedDate;
        return a.bookedTime > b.bookedTime;
    });
    
    currentPage = 0;
}

int PurchaseHistoryView::getTotalPages() const {
    if (userTickets.empty()) return 1;
    return (userTickets.size() + itemsPerPage - 1) / itemsPerPage;
}

vector<Ticket> PurchaseHistoryView::getCurrentPageTickets() const {
    vector<Ticket> pageTickets;
    int startIdx = currentPage * itemsPerPage;
    int endIdx = min(startIdx + itemsPerPage, (int)userTickets.size());
    
    for (int i = startIdx; i < endIdx; i++) {
        pageTickets.push_back(userTickets[i]);
    }
    
    return pageTickets;
}

void PurchaseHistoryView::update(Vector2f mousePos, bool mousePressed, Vector2f cardPos) {
    // Update button positions
    float buttonY = cardPos.y + 650.f;
    prevButton.setPosition({cardPos.x + 26.f, buttonY});
    nextButton.setPosition({cardPos.x + 860.f, buttonY});
    
    prevButtonText.setPosition({cardPos.x + 45.f, buttonY + 8.f});
    nextButtonText.setPosition({cardPos.x + 885.f, buttonY + 8.f});
    
    // Page info
    int totalPages = getTotalPages();
    string pageInfo = "Trang " + to_string(currentPage + 1) + " / " + to_string(totalPages);
    pageInfoText.setString(utf8(pageInfo));
    FloatRect textBounds = pageInfoText.getLocalBounds();
    pageInfoText.setPosition({cardPos.x + 505.f - textBounds.size.x / 2.f, buttonY + 10.f});
    
    // Handle button clicks (mousePressed is actually mouseJustPressed from parent)
    if (mousePressed) {
        bool clickedPrev = prevButton.getGlobalBounds().contains(mousePos) && currentPage > 0;
        bool clickedNext = nextButton.getGlobalBounds().contains(mousePos) && currentPage < totalPages - 1;
        
        if (clickedPrev) currentPage--;
        else if (clickedNext) currentPage++;
    }
    
    // Button hover effects
    if (prevButton.getGlobalBounds().contains(mousePos) && currentPage > 0) {
        prevButton.setFillColor(Color(0, 191, 255));
    } else {
        prevButton.setFillColor(currentPage > 0 ? Color(0, 153, 255) : Color(100, 100, 100));
    }
    
    if (nextButton.getGlobalBounds().contains(mousePos) && currentPage < totalPages - 1) {
        nextButton.setFillColor(Color(0, 191, 255));
    } else {
        nextButton.setFillColor(currentPage < totalPages - 1 ? Color(0, 153, 255) : Color(100, 100, 100));
    }
}

void PurchaseHistoryView::draw(RenderWindow& window, Vector2f cardPos) {
    // Draw title
    titleText.setPosition({cardPos.x + 26.f, cardPos.y + 26.f});
    window.draw(titleText);
    
    // Draw scrollable area
    scrollableArea.setSize({958.f, 560.f});
    scrollableArea.setPosition({cardPos.x + 26.f, cardPos.y + 70.f});
    window.draw(scrollableArea);
    
    // Draw tickets
    vector<Ticket> pageTickets = getCurrentPageTickets();
    
    if (pageTickets.empty()) {
        // No tickets message
    Text noTicketsText(font, utf8("Chưa có lịch sử đặt vé"), 18);
        noTicketsText.setFillColor(Color(150, 150, 150));
        FloatRect bounds = noTicketsText.getLocalBounds();
        noTicketsText.setPosition({
            cardPos.x + 505.f - bounds.size.x / 2.f,
            cardPos.y + 350.f
        });
        window.draw(noTicketsText);
    } else {
        float startY = cardPos.y + 90.f;
        float itemHeight = 105.f;
        
        for (size_t i = 0; i < pageTickets.size(); i++) {
            const Ticket& ticket = pageTickets[i];
            float itemY = startY + i * itemHeight;
            
            // Draw ticket card
            RectangleShape ticketCard;
            ticketCard.setSize({918.f, 95.f});
            ticketCard.setPosition({cardPos.x + 46.f, itemY});
            ticketCard.setFillColor(Color(25, 45, 70, 255));
            ticketCard.setOutlineColor(Color(60, 100, 140));
            ticketCard.setOutlineThickness(1.f);
            window.draw(ticketCard);
            
            // Movie title with ticket code
            String titleDisplay = utf8(ticket.ticketId + ": ") + TicketRepository::getTitleUtf8(ticket);
            Text titleText(font, titleDisplay, 18);
            titleText.setPosition({cardPos.x + 60.f, itemY + 10.f});
            titleText.setFillColor(Color(100, 200, 255));
            titleText.setStyle(Text::Bold);
            window.draw(titleText);
            
            // Show date, time, room
            Text detailText(font, "", 14);
            detailText.setFillColor(Color(200, 200, 200));
            string formattedDateStr = formatDateToDDMMYYYY(ticket.date);
            String formattedDate = utf8(formattedDateStr);
            String detailStr = formattedDate +
                               utf8(" - ") + TicketRepository::getTimeUtf8(ticket) +
                               utf8(" - ") + TicketRepository::getRoomUtf8(ticket);
            detailText.setString(detailStr);
            detailText.setPosition({cardPos.x + 60.f, itemY + 38.f});
            window.draw(detailText);
            
            // Seats + Combo
            String seatLabel = utf8("Ghế: ") + TicketRepository::getBookedUtf8(ticket);
            Text seatText(font, seatLabel, 14);
            seatText.setFillColor(Color(180, 220, 255));
            seatText.setPosition({cardPos.x + 60.f, itemY + 60.f});
            window.draw(seatText);
            
            if (!ticket.comboName.empty() && ticket.comboName != "Không có") {
                String comboTextStr = utf8("  |  Combo: ") + TicketRepository::getComboForHistoryUtf8(ticket);
                Text comboText(font, comboTextStr, 14);
                comboText.setFillColor(Color(255, 200, 100));
                FloatRect seatBounds = seatText.getLocalBounds();
                comboText.setPosition({cardPos.x + 60.f + seatBounds.size.x, itemY + 60.f});
                window.draw(comboText);
            }
            
            // Price (right align)
            Text priceText(font, formatPriceToString(ticket.price), 16);
            priceText.setFillColor(Color(100, 255, 100));
            priceText.setStyle(Text::Bold);
            FloatRect priceBounds = priceText.getLocalBounds();
            priceText.setPosition({cardPos.x + 940.f - priceBounds.size.x, itemY + 10.f});
            window.draw(priceText);
            
            // Booked date/time (small, bottom right)
            string formattedBookedDate = formatDateToDDMMYYYY(ticket.bookedDate);
            String bookedInfo = utf8("Đặt: ") +
                                utf8(formattedBookedDate) +
                                utf8(" ") + utf8(ticket.bookedTime);
            Text bookedText(font, bookedInfo, 12);
            bookedText.setFillColor(Color(120, 120, 120));
            FloatRect bookedBounds = bookedText.getLocalBounds();
            bookedText.setPosition({cardPos.x + 940.f - bookedBounds.size.x, itemY + 70.f});
            window.draw(bookedText);
        }
    }
    
    // Draw pagination buttons
    window.draw(prevButton);
    window.draw(nextButton);
    window.draw(prevButtonText);
    window.draw(nextButtonText);
    window.draw(pageInfoText);
}
