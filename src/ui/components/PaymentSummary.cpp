#include "ui/components/PaymentSummary.h"
#include <locale>
#include <codecvt>

PaymentSummary::PaymentSummary(Font& detail, Font& button) 
    : detailFont(detail), buttonFont(button) {}

void PaymentSummary::draw(RenderWindow& window, const FloatRect& contentArea,
                         int movieId, const string& date, const string& time, 
                         const string& room, const vector<string>& seats, int seatPrice,
                         const SnackMenu& snackMenu) {
    float contentX = contentArea.position.x + 30.f;
    float contentY = contentArea.position.y + 30.f;
    
    Text title(buttonFont, L"TỔNG HỢP THANH TOÁN", 32);
    title.setPosition({contentX, contentY});
    title.setFillColor(Color::White);
    window.draw(title);
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    float startY = contentY + 70.f;
    float lineHeight = 30.f;
    
    // Movie info
    Text movieLabel(buttonFont, L"PHIM:", 24);
    movieLabel.setPosition({contentX, startY});
    movieLabel.setFillColor(Color(255, 200, 100));
    window.draw(movieLabel);
    
    char movieInfo[128];
    snprintf(movieInfo, sizeof(movieInfo), "Phim ID: %d", movieId);
    Text movieValue(detailFont, conv.from_bytes(movieInfo), 20);
    movieValue.setPosition({contentX + 150.f, startY + 3});
    movieValue.setFillColor(Color::White);
    window.draw(movieValue);
    
    startY += lineHeight + 10;
    
    // Date
    Text dateLabel(detailFont, L"Ngày chiếu:", 18);
    dateLabel.setPosition({contentX, startY});
    dateLabel.setFillColor(Color(200, 200, 200));
    window.draw(dateLabel);
    
    string formattedDate = date;
    if (formattedDate.length() == 10) {
        string year = formattedDate.substr(0, 4);
        string month = formattedDate.substr(5, 2);
        string day = formattedDate.substr(8, 2);
        formattedDate = day + " - " + month + " - " + year;
    }
    
    Text dateValue(detailFont, conv.from_bytes(formattedDate), 18);
    dateValue.setPosition({contentX + 150.f, startY});
    dateValue.setFillColor(Color::White);
    window.draw(dateValue);
    
    startY += lineHeight;
    
    // Time
    Text timeLabel(detailFont, L"Giờ chiếu:", 18);
    timeLabel.setPosition({contentX, startY});
    timeLabel.setFillColor(Color(200, 200, 200));
    window.draw(timeLabel);
    
    Text timeValue(detailFont, conv.from_bytes(time), 18);
    timeValue.setPosition({contentX + 150.f, startY});
    timeValue.setFillColor(Color::White);
    window.draw(timeValue);
    
    startY += lineHeight;
    
    // Room
    Text roomLabel(detailFont, L"Phòng:", 18);
    roomLabel.setPosition({contentX, startY});
    roomLabel.setFillColor(Color(200, 200, 200));
    window.draw(roomLabel);
    
    Text roomValue(detailFont, conv.from_bytes(room), 18);
    roomValue.setPosition({contentX + 150.f, startY});
    roomValue.setFillColor(Color::White);
    window.draw(roomValue);
    
    startY += lineHeight + 20;
    
    // Seats
    Text seatsLabel(buttonFont, L"GHẾ NGỒI:", 24);
    seatsLabel.setPosition({contentX, startY});
    seatsLabel.setFillColor(Color(255, 200, 100));
    window.draw(seatsLabel);
    
    startY += 35;
    
    const int ROWS_PER_COL = 3;
    float seatStartX = contentX + 20.f;
    float seatColWidth = 70.f;
    float seatRowHeight = 25.f;
    
    for (size_t i = 0; i < seats.size() && i < 12; i++) {
        int col = i / ROWS_PER_COL;
        int row = i % ROWS_PER_COL;
        
        Text seatText(detailFont, conv.from_bytes("• " + seats[i]), 16);
        seatText.setPosition({seatStartX + col * seatColWidth, startY + row * seatRowHeight});
        seatText.setFillColor(Color(100, 255, 100));
        window.draw(seatText);
    }
    
    if (seats.size() > 12) {
        Text moreSeats(detailFont, L"...", 16);
        moreSeats.setPosition({seatStartX + 280, startY});
        moreSeats.setFillColor(Color::White);
        window.draw(moreSeats);
    }
    
    startY += 90;
    
    // Snacks
    Text snackLabel(buttonFont, L"ĐỒ ĂN & NƯỚC:", 24);
    snackLabel.setPosition({contentX, startY});
    snackLabel.setFillColor(Color(255, 200, 100));
    window.draw(snackLabel);
    
    startY += 35;
    
    int totalSnackPrice = 0;
    bool hasSnacks = false;
    
    const auto& snackItems = snackMenu.getItems();
    for (const auto& item : snackItems) {
        if (item.quantity > 0) {
            hasSnacks = true;
            char snackInfo[128];
            int itemTotal = item.price * item.quantity;
            snprintf(snackInfo, sizeof(snackInfo), "%s x %d", item.name.c_str(), item.quantity);
            
            Text snackText(detailFont, conv.from_bytes(snackInfo), 16);
            snackText.setPosition({contentX + 20.f, startY});
            snackText.setFillColor(Color::White);
            window.draw(snackText);
            
            char priceStr[32];
            snprintf(priceStr, sizeof(priceStr), "%d VNĐ", itemTotal);
            Text priceText(detailFont, conv.from_bytes(priceStr), 16);
            priceText.setPosition({contentX + 400.f, startY});
            priceText.setFillColor(Color(255, 200, 100));
            window.draw(priceText);
            
            totalSnackPrice += itemTotal;
            startY += 25;
        }
    }
    
    if (!hasSnacks) {
        Text noSnacks(detailFont, L"Không chọn đồ ăn", 16);
        noSnacks.setPosition({contentX + 20.f, startY});
        noSnacks.setFillColor(Color(150, 150, 150));
        window.draw(noSnacks);
        startY += 25;
    }
    
    startY += 15;
    
    // Total
    RectangleShape separator({900.f, 3.f});
    separator.setPosition({contentX, startY});
    separator.setFillColor(Color(100, 100, 100));
    window.draw(separator);
    
    startY += 20;
    
    // Ticket price
    int ticketPrice = seatPrice * seats.size();
    
    Text ticketLabel(detailFont, L"Tiền vé:", 20);
    ticketLabel.setPosition({contentX, startY});
    ticketLabel.setFillColor(Color::White);
    window.draw(ticketLabel);
    
    char ticketStr[32];
    snprintf(ticketStr, sizeof(ticketStr), "%d VNĐ", ticketPrice);
    Text ticketValue(detailFont, conv.from_bytes(ticketStr), 20);
    ticketValue.setPosition({contentX + 400.f, startY});
    ticketValue.setFillColor(Color::White);
    window.draw(ticketValue);
    
    startY += 30;
    
    // Snack price
    Text snackPriceLabel(detailFont, L"Tiền đồ ăn:", 20);
    snackPriceLabel.setPosition({contentX, startY});
    snackPriceLabel.setFillColor(Color::White);
    window.draw(snackPriceLabel);
    
    char snackStr[32];
    snprintf(snackStr, sizeof(snackStr), "%d VNĐ", totalSnackPrice);
    Text snackPriceValue(detailFont, conv.from_bytes(snackStr), 20);
    snackPriceValue.setPosition({contentX + 400.f, startY});
    snackPriceValue.setFillColor(Color::White);
    window.draw(snackPriceValue);
    
    startY += 40;
    
    // Grand total
    int grandTotal = ticketPrice + totalSnackPrice;
    
    Text totalLabel(buttonFont, L"TỔNG CỘNG:", 28);
    totalLabel.setPosition({contentX, startY});
    totalLabel.setFillColor(Color(255, 200, 100));
    window.draw(totalLabel);
    
    char totalStr[32];
    snprintf(totalStr, sizeof(totalStr), "%d VNĐ", grandTotal);
    Text totalValue(buttonFont, conv.from_bytes(totalStr), 32);
    totalValue.setPosition({contentX + 400.f, startY - 5});
    totalValue.setFillColor(Color(100, 255, 100));
    window.draw(totalValue);
}