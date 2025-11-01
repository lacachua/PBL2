#include "ui/components/SeatSelector.h"
#include <algorithm>
#include <locale>
#include <codecvt>

SeatSelector::SeatSelector(Font& f) : font(f) {}

void SeatSelector::loadOccupiedSeats(const string& seat_map) {
    occupiedSeats.clear();
    
    if (seat_map.length() != 81) return;
    
    for (int i = 0; i < 81; i++) {
        if (seat_map[i] == '0') {
            int row = i / 9;
            int col = i % 9;
            char rowLabel = 'A' + row;
            string seatID = string(1, rowLabel) + to_string(col + 1);
            occupiedSeats.push_back(seatID);
        }
    }
}

bool SeatSelector::isSeatOccupied(const string& seat) const {
    return find(occupiedSeats.begin(), occupiedSeats.end(), seat) != occupiedSeats.end();
}

bool SeatSelector::isSeatSelected(const string& seat) const {
    return find(selectedSeats.begin(), selectedSeats.end(), seat) != selectedSeats.end();
}

void SeatSelector::handleClick(const Vector2f& mousePos, const FloatRect& contentArea) {
    float seatStartX = contentArea.position.x + 30.f;
    float seatStartY = contentArea.position.y + 30.f;
    float seatSize = 30.f;
    float seatSpacing = 6.f;
    
    for (int row = 0; row < SEAT_ROWS; row++) {
        for (int col = 0; col < SEAT_COLS; col++) {
            char rowLabel = 'A' + row;
            string seatID = string(1, rowLabel) + to_string(col + 1);
            
            if (isSeatOccupied(seatID)) continue;
            
            float x = seatStartX + 65 + col * (seatSize + seatSpacing);
            float y = seatStartY + 161 + row * (seatSize + seatSpacing);
            
            FloatRect seatRect({x, y}, {seatSize, seatSize});
            if (seatRect.contains(mousePos)) {
                auto it = find(selectedSeats.begin(), selectedSeats.end(), seatID);
                if (it != selectedSeats.end()) {
                    selectedSeats.erase(it);
                } else {
                    selectedSeats.push_back(seatID);
                }
                return;
            }
        }
    }
}

void SeatSelector::draw(RenderWindow& window, const FloatRect& contentArea) {
    float seatStartX = contentArea.position.x + 30.f;
    float seatStartY = contentArea.position.y + 30.f;
    float seatSize = 30.f;
    float seatSpacing = 6.f;
    
    // Legend
    float legendX = contentArea.position.x + 95.f;
    float legendY = contentArea.position.y + 75.f;
    
    RectangleShape legend1({25.f, 25.f});
    legend1.setPosition({legendX, legendY});
    legend1.setFillColor(Color(60, 60, 70));
    legend1.setOutlineThickness(1.f);
    legend1.setOutlineColor(Color(100, 100, 110));
    window.draw(legend1);
    
    Text legend1Text(font, L"Trống", 14);
    legend1Text.setPosition({legendX + 32.f, legendY + 4.f});
    legend1Text.setFillColor(Color::White);
    window.draw(legend1Text);
    
    RectangleShape legend2({25.f, 25.f});
    legend2.setPosition({legendX + 113.f, legendY});
    legend2.setFillColor(Color(52, 150, 52));
    legend2.setOutlineThickness(2.f);
    legend2.setOutlineColor(Color(100, 255, 100));
    window.draw(legend2);
    
    Text legend2Text(font, L"Đã chọn", 14);
    legend2Text.setPosition({legendX + 145.f, legendY + 4.f});
    legend2Text.setFillColor(Color::White);
    window.draw(legend2Text);
    
    RectangleShape legend3({25.f, 25.f});
    legend3.setPosition({legendX + 235.f, legendY});
    legend3.setFillColor(Color(80, 80, 80));
    legend3.setOutlineThickness(1.f);
    legend3.setOutlineColor(Color(100, 100, 100));
    window.draw(legend3);
    
    Font buttonFont("../assets/BEBAS_NEUE_ZSMALL.ttf");
    Text xMarkLegend(buttonFont, "X", 18);
    xMarkLegend.setPosition({legendX + 244.f, legendY + 1.f});
    xMarkLegend.setFillColor(Color(150, 150, 150));
    window.draw(xMarkLegend);
    
    Text legend3Text(font, L"Đã đặt", 14);
    legend3Text.setPosition({legendX + 267.f, legendY + 4.f});
    legend3Text.setFillColor(Color::White);
    window.draw(legend3Text);
    
    // Screen
    RectangleShape screen({460.f, 8.f});
    screen.setPosition({seatStartX, seatStartY + 105.f});
    screen.setFillColor(Color(200, 200, 200));
    window.draw(screen);
    
    Text screenText(font, L"MÀN HÌNH", 16);
    screenText.setPosition({screen.getPosition().x + 186, screen.getPosition().y - 20});
    screenText.setFillColor(Color(200, 200, 200));
    window.draw(screenText);
    
    Vector2f mousePos = Vector2f(Mouse::getPosition(window));
    
    // Draw seats
    for (int row = 0; row < SEAT_ROWS; row++) {
        char rowLabel = 'A' + row;
        
        for (int col = 0; col < SEAT_COLS; col++) {
            string seatID = string(1, rowLabel) + to_string(col + 1);
            
            float x = seatStartX + 65 + col * (seatSize + seatSpacing);
            float y = seatStartY + 161 + row * (seatSize + seatSpacing);
            
            RectangleShape seat({seatSize, seatSize});
            seat.setPosition({x, y});
            
            bool isHovered = FloatRect({x, y}, {seatSize, seatSize}).contains(mousePos);
            
            if (isSeatOccupied(seatID)) {
                seat.setFillColor(Color(80, 80, 80));
                seat.setOutlineThickness(1.f);
                seat.setOutlineColor(Color(100, 100, 100));
                window.draw(seat);
                
                Text xMark(buttonFont, "X", 28);
                FloatRect textBounds = xMark.getLocalBounds();
                xMark.setPosition({
                    x + (seatSize - textBounds.size.x) / 2.f - textBounds.position.x,
                    y + (seatSize - textBounds.size.y) / 2.f - textBounds.position.y
                });
                xMark.setFillColor(Color(150, 150, 150));
                window.draw(xMark);
            } else if (isSeatSelected(seatID)) {
                seat.setFillColor(Color(52, 150, 52));
                seat.setOutlineThickness(2.f);
                seat.setOutlineColor(Color(100, 255, 100));
                window.draw(seat);
            } else {
                if (isHovered) {
                    seat.setFillColor(Color(100, 100, 120));
                    seat.setOutlineThickness(2.f);
                    seat.setOutlineColor(Color(150, 150, 180));
                } else {
                    seat.setFillColor(Color(60, 60, 70));
                    seat.setOutlineThickness(1.f);
                    seat.setOutlineColor(Color(100, 100, 110));
                }
                window.draw(seat);
            }
        }
    }
}

void SeatSelector::drawSummary(RenderWindow& window, const FloatRect& summaryArea,
                               const string& date, const string& time, 
                               const string& room, int price) {
    float summaryX = summaryArea.position.x;
    float summaryY = summaryArea.position.y;
    
    Font buttonFont("../assets/BEBAS_NEUE_ZSMALL.ttf");
    
    RectangleShape summaryBox({410.f, 380.f});
    summaryBox.setPosition({summaryX, summaryY + 100});
    summaryBox.setFillColor(Color(30, 30, 35));
    summaryBox.setOutlineThickness(2.f);
    summaryBox.setOutlineColor(Color(60, 60, 70));
    window.draw(summaryBox);
    
    Text title(buttonFont, L"THÔNG TIN ĐẶT VÉ", 36);
    title.setPosition({summaryX + 120.f, summaryY + 45.f});
    title.setFillColor(Color(255, 200, 100));
    window.draw(title);
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    
    Text dateLabel(font, L"Ngày chiếu:", 16);
    dateLabel.setPosition({summaryX + 20.f, summaryY + 120.f});
    dateLabel.setFillColor(Color(200, 200, 200));
    window.draw(dateLabel);
    
    string formattedDate = date;
    if (formattedDate.length() == 10) {
        string year = formattedDate.substr(0, 4);
        string month = formattedDate.substr(5, 2);
        string day = formattedDate.substr(8, 2);
        formattedDate = day + " - " + month + " - " + year;
    }
    
    Text dateValue(font, conv.from_bytes(formattedDate), 16);
    dateValue.setPosition({summaryX + 160.f, summaryY + 120.f});
    dateValue.setFillColor(Color::White);
    window.draw(dateValue);
    
    Text timeLabel(font, L"Giờ chiếu:", 16);
    timeLabel.setPosition({summaryX + 20.f, summaryY + 150.f});
    timeLabel.setFillColor(Color(200, 200, 200));
    window.draw(timeLabel);
    
    Text timeValue(font, conv.from_bytes(time), 16);
    timeValue.setPosition({summaryX + 160.f, summaryY + 150.f});
    timeValue.setFillColor(Color::White);
    window.draw(timeValue);
    
    Text roomLabel(font, L"Phòng:", 16);
    roomLabel.setPosition({summaryX + 20.f, summaryY + 180.f});
    roomLabel.setFillColor(Color(200, 200, 200));
    window.draw(roomLabel);
    
    Text roomValue(font, conv.from_bytes(room), 16);
    roomValue.setPosition({summaryX + 160.f, summaryY + 180.f});
    roomValue.setFillColor(Color::White);
    window.draw(roomValue);
    
    Text seatsLabel(buttonFont, L"GHẾ ĐÃ CHỌN:", 34);
    seatsLabel.setPosition({summaryX + 20.f, summaryY + 220.f});
    seatsLabel.setFillColor(Color(255, 200, 100));
    window.draw(seatsLabel);
    
    if (selectedSeats.empty()) {
        Text noSeats(font, L"Chưa chọn ghế nào", 16);
        noSeats.setPosition({summaryX + 20.f, summaryY + 275.f});
        noSeats.setFillColor(Color(200, 100, 100));
        window.draw(noSeats);
    } else {
        const int MAX_SEATS_DISPLAY = 11;
        const int ROWS_PER_COL = 3;
        float startX = summaryX + 20.f;
        float startY = summaryY + 275.f;
        float colWidth = 75.f;
        float rowHeight = 30.f;
        
        size_t seatsToShow = min((size_t)MAX_SEATS_DISPLAY, selectedSeats.size());
        
        for (size_t i = 0; i < seatsToShow; i++) {
            int col = i / ROWS_PER_COL;
            int row = i % ROWS_PER_COL;
            
            Text seatItem(font, conv.from_bytes("• Ghế " + selectedSeats[i]), 16);
            seatItem.setPosition({startX + col * colWidth, startY + row * rowHeight});
            seatItem.setFillColor(Color(100, 255, 100));
            window.draw(seatItem);
        }
        
        if (selectedSeats.size() > MAX_SEATS_DISPLAY) {
            int col = MAX_SEATS_DISPLAY / ROWS_PER_COL;
            int row = MAX_SEATS_DISPLAY % ROWS_PER_COL;
            
            Text moreSeats(font, L"...", 16);
            moreSeats.setPosition({startX + col * colWidth, startY + row * rowHeight});
            moreSeats.setFillColor(Color::White);
            window.draw(moreSeats);
        }
        
        int totalPrice = price * selectedSeats.size();
        
        RectangleShape line({300.f, 2.f});
        line.setPosition({summaryX + 20.f, summaryY + 375.f});
        line.setFillColor(Color(100, 100, 100));
        window.draw(line);
        
        Text totalLabel(buttonFont, L"TỔNG TIỀN:", 36);
        totalLabel.setPosition({summaryX + 20.f, summaryY + 395.f});
        totalLabel.setFillColor(Color::White);
        window.draw(totalLabel);
        
        char priceStr[32];
        snprintf(priceStr, sizeof(priceStr), "%d VNĐ", totalPrice);
        Text totalValue(buttonFont, conv.from_bytes(priceStr), 36);
        totalValue.setPosition({summaryX + 160.f, summaryY + 395.f});
        totalValue.setFillColor(Color(255, 200, 100));
        window.draw(totalValue);
    }
}

string SeatSelector::generateSeatMap() const {
    string seat_map = string(81, '1');
    
    for (const auto& seat : occupiedSeats) {
        char rowLabel = seat[0];
        int col = stoi(seat.substr(1)) - 1;
        int row = rowLabel - 'A';
        int index = row * 9 + col;
        if (index >= 0 && index < 81) {
            seat_map[index] = '0';
        }
    }
    
    for (const auto& seat : selectedSeats) {
        char rowLabel = seat[0];
        int col = stoi(seat.substr(1)) - 1;
        int row = rowLabel - 'A';
        int index = row * 9 + col;
        if (index >= 0 && index < 81) {
            seat_map[index] = '0';
        }
    }
    
    return seat_map;
}