#include "UI/components/TicketBooking/ConfirmationView.h"
#include <sstream>
#include <iomanip>

ConfirmationView::ConfirmationView(Font& f) 
    : font(f),
      homeButton(f, L"Quay lại trang chủ", 300.f, 60.f, 20),
      titleText(f),
      sectionTitle(f),
      labelTicketId(f), valueTicketId(f),
      labelCustomer(f), valueCustomer(f),
      labelEmail(f), valueEmail(f),
      labelPhone(f), valuePhone(f),
      labelMovie(f), valueMovie(f),
      labelRoom(f), valueRoom(f),
      labelDateTime(f), valueDateTime(f),
      labelSeats(f), valueSeats(f),
      labelCombo(f), valueCombo(f),
      labelTotal(f), valueTotal(f),
      thankYouText(f) {
    initializeUI();
}

void ConfirmationView::initializeUI() {
    // mainView dimensions: khoảng 1280x800, bắt đầu từ (174, 100)
    float leftCol = 200.f;   // Cột Mô tả
    float rightCol = 500.f;  // Cột Thông tin
    float startY = 220.f;
    float lineHeight = 35.f;
    
    // Title - Đặt vé thành công
    titleText.setCharacterSize(32);
    titleText.setFillColor(Color(46, 204, 113));
    string titleStr = "ĐẶT VÉ THÀNH CÔNG";
    titleText.setString(String::fromUtf8(titleStr.begin(), titleStr.end()));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.f, titleBounds.size.y / 2.f});
    titleText.setPosition({640.f, startY + 60.f});
    
    // Section title
    startY += 60.f;
    sectionTitle.setCharacterSize(24);
    sectionTitle.setFillColor(Color::White);
    string sectionStr = "Tóm tắt đơn hàng";
    sectionTitle.setString(String::fromUtf8(sectionStr.begin(), sectionStr.end()));
    sectionTitle.setPosition({leftCol, startY});
    
    // Initialize all labels (Cột Mô tả)
    startY += 50.f;
    
    // Mã vé
    labelTicketId.setFont(font);
    labelTicketId.setCharacterSize(20);
    labelTicketId.setFillColor(Color(180, 180, 180));
    string maVe = "Mã vé";
    labelTicketId.setString(String::fromUtf8(maVe.begin(), maVe.end()));
    labelTicketId.setPosition({leftCol, startY});
    
    valueTicketId.setFont(font);
    valueTicketId.setCharacterSize(20);
    valueTicketId.setFillColor(Color::White);
    valueTicketId.setPosition({rightCol, startY});
    
    // Khách hàng
    startY += lineHeight;
    labelCustomer.setFont(font);
    labelCustomer.setCharacterSize(20);
    labelCustomer.setFillColor(Color(180, 180, 180));
    string khachHang = "Khách hàng";
    labelCustomer.setString(String::fromUtf8(khachHang.begin(), khachHang.end()));
    labelCustomer.setPosition({leftCol, startY});
    
    valueCustomer.setFont(font);
    valueCustomer.setCharacterSize(20);
    valueCustomer.setFillColor(Color::White);
    valueCustomer.setPosition({rightCol, startY});
    
    // Email
    startY += lineHeight;
    labelEmail.setFont(font);
    labelEmail.setCharacterSize(20);
    labelEmail.setFillColor(Color(180, 180, 180));
    string emailStr = "Email";
    labelEmail.setString(String::fromUtf8(emailStr.begin(), emailStr.end()));
    labelEmail.setPosition({leftCol, startY});
    
    valueEmail.setFont(font);
    valueEmail.setCharacterSize(20);
    valueEmail.setFillColor(Color::White);
    valueEmail.setPosition({rightCol, startY});
    
    // Phim
    startY += lineHeight + 10.f;
    labelMovie.setFont(font);
    labelMovie.setCharacterSize(20);
    labelMovie.setFillColor(Color(180, 180, 180));
    string movieStr = "Phim";
    labelMovie.setString(String::fromUtf8(movieStr.begin(), movieStr.end()));
    labelMovie.setPosition({leftCol, startY});
    
    valueMovie.setFont(font);
    valueMovie.setCharacterSize(20);
    valueMovie.setFillColor(Color::White);
    valueMovie.setPosition({rightCol, startY});
    
    // Phòng
    startY += lineHeight;
    labelRoom.setFont(font);
    labelRoom.setCharacterSize(20);
    labelRoom.setFillColor(Color(180, 180, 180));
    string roomStr = "Phòng";
    labelRoom.setString(String::fromUtf8(roomStr.begin(), roomStr.end()));
    labelRoom.setPosition({leftCol, startY});
    
    valueRoom.setFont(font);
    valueRoom.setCharacterSize(20);
    valueRoom.setFillColor(Color::White);
    valueRoom.setPosition({rightCol, startY});
    
    // Ngày giờ
    startY += lineHeight;
    labelDateTime.setFont(font);
    labelDateTime.setCharacterSize(20);
    labelDateTime.setFillColor(Color(180, 180, 180));
    string dateTimeStr = "Ngày & Giờ";
    labelDateTime.setString(String::fromUtf8(dateTimeStr.begin(), dateTimeStr.end()));
    labelDateTime.setPosition({leftCol, startY});
    
    valueDateTime.setFont(font);
    valueDateTime.setCharacterSize(20);
    valueDateTime.setFillColor(Color::White);
    valueDateTime.setPosition({rightCol, startY});
    
    // Ghế
    startY += lineHeight + 10.f;
    labelSeats.setFont(font);
    labelSeats.setCharacterSize(20);
    labelSeats.setFillColor(Color(180, 180, 180));
    string seatsStr = "Ghế đơn";
    labelSeats.setString(String::fromUtf8(seatsStr.begin(), seatsStr.end()));
    labelSeats.setPosition({leftCol, startY});
    
    valueSeats.setFont(font);
    valueSeats.setCharacterSize(20);
    valueSeats.setFillColor(Color::White);
    valueSeats.setPosition({rightCol, startY});
    
    // Combo
    startY += lineHeight;
    labelCombo.setFont(font);
    labelCombo.setCharacterSize(20);
    labelCombo.setFillColor(Color(180, 180, 180));
    string comboStr = "Combo";
    labelCombo.setString(String::fromUtf8(comboStr.begin(), comboStr.end()));
    labelCombo.setPosition({leftCol, startY});
    
    valueCombo.setFont(font);
    valueCombo.setCharacterSize(20);
    valueCombo.setFillColor(Color::White);
    valueCombo.setPosition({rightCol, startY});
    
    // Divider line before total
    startY += lineHeight + 20.f;
    
    // Tổng cộng
    labelTotal.setFont(font);
    labelTotal.setCharacterSize(24);
    labelTotal.setFillColor(Color::White);
    string totalStr = "Tổng cộng";
    labelTotal.setString(String::fromUtf8(totalStr.begin(), totalStr.end()));
    labelTotal.setPosition({leftCol, startY});
    
    valueTotal.setFont(font);
    valueTotal.setCharacterSize(24);
    valueTotal.setFillColor(Color(255, 215, 0)); // Gold
    valueTotal.setPosition({rightCol, startY});
    
    // Home button
    homeButton.setPosition({490.f, 700.f});
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
    this->userPhone = userPhone;
    
    // Mã vé
    valueTicketId.setString(String::fromUtf8(ticket.ticketId.begin(), ticket.ticketId.end()));
    
    // Khách hàng
    valueCustomer.setString(String::fromUtf8(ticket.fullName.begin(), ticket.fullName.end()));
    
    // Email
    valueEmail.setString(String::fromUtf8(ticket.email.begin(), ticket.email.end()));
    
    // Số điện thoại
    valuePhone.setString(String::fromUtf8(userPhone.begin(), userPhone.end()));
    
    // Phim
    valueMovie.setString(String::fromUtf8(movieName.begin(), movieName.end()));
    
    // Phòng
    valueRoom.setString(String::fromUtf8(roomName.begin(), roomName.end()));
    
    // Ngày & Giờ
    string dateTime = date + " - " + time;
    valueDateTime.setString(String::fromUtf8(dateTime.begin(), dateTime.end()));
    
    // Ghế
    valueSeats.setString(String::fromUtf8(ticket.booked.begin(), ticket.booked.end()));
    
    // Combo
    string comboDisplay = (ticket.comboName.empty() || ticket.comboName == "Không có") 
                          ? "Không có" 
                          : ticket.comboName;
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
