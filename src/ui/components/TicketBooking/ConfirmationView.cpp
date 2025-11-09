#include "UI/components/TicketBooking/ConfirmationView.h"
#include <sstream>
#include <iomanip>

// Helper function: Convert date format (YYYY-MM-DD or any format) to DD/MM/YYYY
static std::string formatDateToDDMMYYYY(const std::string& dateStr) {
    // Nếu đã đúng format DD/MM/YYYY (có chứa '/'), giữ nguyên
    if (dateStr.find('/') != std::string::npos) {
        return dateStr;
    }
    
    // Nếu format YYYY-MM-DD hoặc YYYYMMDD, chuyển đổi
    if (dateStr.length() >= 8) {
        std::string year, month, day;
        
        if (dateStr.find('-') != std::string::npos) {
            // Format: YYYY-MM-DD
            std::stringstream ss(dateStr);
            std::getline(ss, year, '-');
            std::getline(ss, month, '-');
            std::getline(ss, day, '-');
        } else {
            // Format: YYYYMMDD
            year = dateStr.substr(0, 4);
            month = dateStr.substr(4, 2);
            day = dateStr.substr(6, 2);
        }
        
        return day + "/" + month + "/" + year;
    }
    
    return dateStr; // Giữ nguyên nếu không nhận dạng được
}

ConfirmationView::ConfirmationView(Font& f) 
    : font(f),
      homeButton(f, L"Quay lại trang chủ", 300.f, 60.f, 20),
      titleText(f, L"ĐẶT VÉ THÀNH CÔNG", 32),
      labelTicketId(f, L"Mã vé", 20), valueTicketId(f),
      labelCustomer(f, L"Khách hàng", 20), valueCustomer(f),
      labelEmail(f, L"Email", 20), valueEmail(f),
      labelMovie(f, L"Phim", 20), valueMovie(f),
      labelRoom(f, L"Phòng", 20), valueRoom(f),
      labelDateTime(f, "Ngày & Giờ", 20), valueDateTime(f),
      labelSeats(f, L"Ghế", 20), valueSeats(f),
      labelCombo(f, L"Combo", 20), valueCombo(f),
      labelTotal(f, L"Tổng cộng", 24), valueTotal(f)
{
    initializeUI();
}

void ConfirmationView::initializeUI() {
    // mainView dimensions: khoảng 1280x800, bắt đầu từ (174, 100)
    float leftCol = 200.f;   // Cột Mô tả
    float rightCol = 500.f;  // Cột Thông tin
    float startY = 220.f;
    float lineHeight = 35.f;
    
    // Title - Đặt vé thành công
    titleText.setFillColor(Color(46, 204, 113));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.f, titleBounds.size.y / 2.f});
    titleText.setPosition({640.f, startY + 40.f});

    // Mã vé
    startY += 70.f;
    labelTicketId.setFillColor(Color(180, 180, 180));
    labelTicketId.setPosition({leftCol, startY});
    
    valueTicketId.setCharacterSize(20);
    valueTicketId.setFillColor(Color::White);
    valueTicketId.setPosition({rightCol, startY});
    
    // Khách hàng
    startY += lineHeight;
    labelCustomer.setFillColor(Color(180, 180, 180));
    labelCustomer.setPosition({leftCol, startY});
    
    valueCustomer.setCharacterSize(20);
    valueCustomer.setFillColor(Color::White);
    valueCustomer.setPosition({rightCol, startY});
    
    // Email
    startY += lineHeight;
    labelEmail.setFillColor(Color(180, 180, 180));
    labelEmail.setPosition({leftCol, startY});
    
    valueEmail.setCharacterSize(20);
    valueEmail.setFillColor(Color::White);
    valueEmail.setPosition({rightCol, startY});
    
    // Phim
    startY += lineHeight + 10.f;
    labelMovie.setFillColor(Color(180, 180, 180));
    labelMovie.setPosition({leftCol, startY});
    
    valueMovie.setCharacterSize(20);
    valueMovie.setFillColor(Color::White);
    valueMovie.setPosition({rightCol, startY});
    
    // Phòng
    startY += lineHeight;
    labelRoom.setFillColor(Color(180, 180, 180));
    labelRoom.setPosition({leftCol, startY});
    
    valueRoom.setCharacterSize(20);
    valueRoom.setFillColor(Color::White);
    valueRoom.setPosition({rightCol, startY});
    
    // Ngày giờ
    startY += lineHeight;
    labelDateTime.setFillColor(Color(180, 180, 180));
    labelDateTime.setPosition({leftCol, startY});
    
    valueDateTime.setCharacterSize(20);
    valueDateTime.setFillColor(Color::White);
    valueDateTime.setPosition({rightCol, startY});
    
    // Ghế
    startY += lineHeight + 10.f;
    labelSeats.setFillColor(Color(180, 180, 180));
    labelSeats.setPosition({leftCol, startY});
    
    valueSeats.setCharacterSize(20);
    valueSeats.setFillColor(Color::White);
    valueSeats.setPosition({rightCol, startY});
    
    // Combo
    startY += lineHeight;
    labelCombo.setFillColor(Color(180, 180, 180));
    labelCombo.setPosition({leftCol, startY});
    
    valueCombo.setCharacterSize(20);
    valueCombo.setFillColor(Color::White);
    valueCombo.setPosition({rightCol, startY});
    
    // Divider line before total
    startY += lineHeight + 20.f;
    
    // Tổng cộng
    labelTotal.setFillColor(Color::White);
    labelTotal.setPosition({leftCol, startY});
    
    valueTotal.setCharacterSize(24);
    valueTotal.setFillColor(Color(255, 215, 0));
    valueTotal.setPosition({rightCol, startY});
    
    // Home button
    homeButton.setPosition({490.f, 720.f});
    homeButton.setFillColor(Color(20, 118, 172));
}

void ConfirmationView::setTicketData(
    const Ticket& ticket,
    const string& userName,
    const string& userPhone,
    const string& movieName,
    const string& roomName,
    const string& date,
    const string& time
) {
    this->currentTicket = ticket;
    this->userName = userName;
    
    // Mã vé - String::fromUtf8
    valueTicketId.setString(String::fromUtf8(ticket.ticketId.begin(), ticket.ticketId.end()));
    
    // Khách hàng - String::fromUtf8
    valueCustomer.setString(String::fromUtf8(ticket.fullName.begin(), ticket.fullName.end()));
    
    // Email - String::fromUtf8
    valueEmail.setString(String::fromUtf8(ticket.email.begin(), ticket.email.end()));
    
    // Phim - String::fromUtf8
    valueMovie.setString(String::fromUtf8(movieName.begin(), movieName.end()));
    
    // Phòng - String::fromUtf8
    valueRoom.setString(String::fromUtf8(roomName.begin(), roomName.end()));
    
    // Ngày & Giờ - Format ngày sang DD/MM/YYYY, String::fromUtf8
    string formattedDate = formatDateToDDMMYYYY(date);
    string dateTime = formattedDate + " - " + time;
    valueDateTime.setString(String::fromUtf8(dateTime.begin(), dateTime.end()));
    
    // Ghế - String::fromUtf8
    valueSeats.setString(String::fromUtf8(ticket.booked.begin(), ticket.booked.end()));
    
    // Combo - String::fromUtf8
    string comboDisplay = (ticket.comboName.empty() || ticket.comboName == "Không có") ? "Không có" : ticket.comboName;
    valueCombo.setString(String::fromUtf8(comboDisplay.begin(), comboDisplay.end()));
    
    // Tổng tiền - Format with thousand separators
    stringstream ss;
    ss << ticket.price;
    string priceStr = ss.str();
    
    // Manual thousand separator for Vietnamese format
    string formattedPrice;
    int count = 0;
    for (int i = priceStr.length() - 1; i >= 0; --i) {
        if (count == 3) {
            formattedPrice = "." + formattedPrice;
            count = 0;
        }
        formattedPrice = priceStr[i] + formattedPrice;
        count++;
    }
    
    string totalStr = formattedPrice + " VNĐ";
    valueTotal.setString(String::fromUtf8(totalStr.begin(), totalStr.end()));
}

void ConfirmationView::update(Vector2f mousePos, bool mousePressed) {
    Color normalColor = Color(20, 118, 172);
    Color hoverColor = Color(30, 138, 192);
    homeButton.update(mousePos, mousePressed, normalColor, hoverColor);
}

bool ConfirmationView::handleHomeButtonClick(Vector2f mousePos, bool mousePressed, AppState& state) {
    if (homeButton.isClicked(mousePos, mousePressed)) {
        state = AppState::HOME;
        return true;
    }
    return false;
}

void ConfirmationView::draw(RenderWindow& window) {
    // Title
    window.draw(titleText);
    
    // Draw all labels (Cột Mô tả)
    window.draw(labelTicketId);
    window.draw(labelCustomer);
    window.draw(labelEmail);
    window.draw(labelMovie);
    window.draw(labelRoom);
    window.draw(labelDateTime);
    window.draw(labelSeats);
    window.draw(labelCombo);
    window.draw(labelTotal);
    
    // Draw all values (Cột Thông tin)
    window.draw(valueTicketId);
    window.draw(valueCustomer);
    window.draw(valueEmail);
    window.draw(valueMovie);
    window.draw(valueRoom);
    window.draw(valueDateTime);
    window.draw(valueSeats);
    window.draw(valueCombo);
    window.draw(valueTotal);
    
    // Footer
    homeButton.draw(window);
}
