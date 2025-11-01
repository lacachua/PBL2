#include "ui/components/BookingConfirmation.h"
#include <locale>
#include <codecvt>

BookingConfirmation::BookingConfirmation(Font& detail, Font& button)
    : detailFont(detail), buttonFont(button) {}

void BookingConfirmation::draw(RenderWindow& window, const FloatRect& contentArea,
                               const string& bookingCode, int movieId,
                               const string& date, const string& time, const string& room,
                               const vector<string>& seats) {
    float contentX = contentArea.position.x + 30.f;
    float contentY = contentArea.position.y + 30.f;
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    
    // Success title
    Text successTitle(buttonFont, L"ĐẶT VÉ THÀNH CÔNG!", 36);
    FloatRect titleBounds = successTitle.getLocalBounds();
    successTitle.setPosition({contentX + (900.f - titleBounds.size.x) / 2.f, contentY + 20.f});
    successTitle.setFillColor(Color(100, 255, 100));
    window.draw(successTitle);
    
    // Success message
    Text successMsg(detailFont, L"Vé đã được đặt thành công. Mã vé sẽ được gửi qua email/SMS.", 18);
    FloatRect msgBounds = successMsg.getLocalBounds();
    successMsg.setPosition({contentX + (900.f - msgBounds.size.x) / 2.f, contentY + 70.f});
    successMsg.setFillColor(Color(200, 200, 200));
    window.draw(successMsg);
    
    float startY = contentY + 110.f;
    
    // Info box
    RectangleShape infoBox({850.f, 280.f});
    infoBox.setPosition({contentX + 25.f, startY});
    infoBox.setFillColor(Color(40, 40, 45));
    infoBox.setOutlineThickness(2.f);
    infoBox.setOutlineColor(Color(100, 100, 100));
    window.draw(infoBox);
    
    float infoX = contentX + 50.f;
    float infoY = startY + 20.f;
    float lineHeight = 35.f;
    
    // Booking code
    Text codeLabel(buttonFont, L"MÃ ĐẶT VÉ:", 24);
    codeLabel.setPosition({infoX, infoY});
    codeLabel.setFillColor(Color(255, 200, 100));
    window.draw(codeLabel);
    
    Text codeValue(buttonFont, conv.from_bytes(bookingCode), 26);
    codeValue.setPosition({infoX + 550.f, infoY});
    codeValue.setFillColor(Color(100, 255, 100));
    window.draw(codeValue);
    
    infoY += lineHeight + 10;
    
    // Movie
    Text movieLabel(detailFont, L"Phim:", 18);
    movieLabel.setPosition({infoX, infoY});
    movieLabel.setFillColor(Color(200, 200, 200));
    window.draw(movieLabel);
    
    char movieInfo[128];
    snprintf(movieInfo, sizeof(movieInfo), "Movie ID: %d", movieId);
    Text movieValue(detailFont, conv.from_bytes(movieInfo), 18);
    movieValue.setPosition({infoX + 550.f, infoY});
    movieValue.setFillColor(Color::White);
    window.draw(movieValue);
    
    infoY += lineHeight;
    
    // Date
    Text dateLabel(detailFont, L"Ngày chiếu:", 18);
    dateLabel.setPosition({infoX, infoY});
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
    dateValue.setPosition({infoX + 550.f, infoY});
    dateValue.setFillColor(Color::White);
    window.draw(dateValue);
    
    infoY += lineHeight;
    
    // Time
    Text timeLabel(detailFont, L"Giờ chiếu:", 18);
    timeLabel.setPosition({infoX, infoY});
    timeLabel.setFillColor(Color(200, 200, 200));
    window.draw(timeLabel);
    
    Text timeValue(detailFont, conv.from_bytes(time), 18);
    timeValue.setPosition({infoX + 550.f, infoY});
    timeValue.setFillColor(Color::White);
    window.draw(timeValue);
    
    infoY += lineHeight;
    
    // Room
    Text roomLabel(detailFont, L"Phòng:", 18);
    roomLabel.setPosition({infoX, infoY});
    roomLabel.setFillColor(Color(200, 200, 200));
    window.draw(roomLabel);
    
    Text roomValue(detailFont, conv.from_bytes(room), 18);
    roomValue.setPosition({infoX + 550.f, infoY});
    roomValue.setFillColor(Color::White);
    window.draw(roomValue);
    
    infoY += lineHeight;
    
    // Seats
    Text seatsLabel(detailFont, L"Ghế ngồi:", 18);
    seatsLabel.setPosition({infoX, infoY});
    seatsLabel.setFillColor(Color(200, 200, 200));
    window.draw(seatsLabel);
    
    string seatsList;
    for (size_t i = 0; i < seats.size(); i++) {
        seatsList += seats[i];
        if (i < seats.size() - 1) seatsList += ", ";
    }
    
    Text seatsValue(detailFont, conv.from_bytes(seatsList), 18);
    seatsValue.setPosition({infoX + 550.f, infoY});
    seatsValue.setFillColor(Color::White);
    window.draw(seatsValue);
    
    infoY += lineHeight;
    
    // Quantity
    Text qtyLabel(detailFont, L"Số lượng:", 18);
    qtyLabel.setPosition({infoX, infoY});
    qtyLabel.setFillColor(Color(200, 200, 200));
    window.draw(qtyLabel);
    
    char qtyStr[32];
    snprintf(qtyStr, sizeof(qtyStr), "%zu vé", seats.size());
    Text qtyValue(detailFont, conv.from_bytes(qtyStr), 18);
    qtyValue.setPosition({infoX + 550.f, infoY});
    qtyValue.setFillColor(Color::White);
    window.draw(qtyValue);
    
    // Instruction
    Text instructionText(detailFont, L"Vui lòng đến rạp trước 15 phút để nhận vé. Cảm ơn quý khách!", 16);
    FloatRect instBounds = instructionText.getLocalBounds();
    instructionText.setPosition({contentX + (900.f - instBounds.size.x) / 2.f, contentY + 420.f});
    instructionText.setFillColor(Color(150, 150, 150));
    window.draw(instructionText);
}