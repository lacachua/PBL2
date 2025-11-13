#include "UI/components/TicketBooking/ConfirmationView.h"
#include <sstream>
#include <cstring>

using namespace std;
using namespace sf;

static String fmtMoney(int v) {
    string s = to_string(v), out;
    for (int i = 0; i < (int)s.size(); ++i) {
        out += s[i];
        if ((int)s.size() - i - 1 > 0 && ((int)s.size() - i - 1) % 3 == 0) out += ".";
    }
    out += " đ";
    return String::fromUtf8(out.begin(), out.end());
}

static string formatDateToDDMMYYYY(const string& dateStr) {
    if (dateStr.find('/') != string::npos) 
        return dateStr;
    
    if (dateStr.length() >= 8) {
        string year, month, day;
        
        if (dateStr.find('-') != string::npos) {
            stringstream ss(dateStr);
            getline(ss, year, '-');
            getline(ss, month, '-');
            getline(ss, day, '-');
        } 
        else {
            year = dateStr.substr(0, 4);
            month = dateStr.substr(4, 2);
            day = dateStr.substr(6, 2);
        }
        return day + "/" + month + "/" + year;
    }
    return dateStr;
}

static String utf8(const char* text) {
    size_t len = std::strlen(text);
    return String::fromUtf8(text, text + len);
}

ConfirmationView::ConfirmationView(Font& f) 
        : font(f),
      homeButton(f, utf8("Quay lại trang chủ"), 300.f, 60.f, 20),
      titleText(f, utf8("ĐẶT VÉ THÀNH CÔNG"), 32),
      labelTicketId(f, utf8("Mã vé"), 20), valueTicketId(f, String(), 20),
      labelCustomer(f, utf8("Khách hàng"), 20), valueCustomer(f, String(), 20),
      labelEmail(f, utf8("Email"), 20), valueEmail(f, String(), 20),
      labelMovie(f, utf8("Phim"), 20), valueMovie(f, String(), 20),
      labelRoom(f, utf8("Phòng"), 20), valueRoom(f, String(), 20),
      labelDateTime(f, utf8("Ngày & Giờ"), 20), valueDateTime(f, String(), 20),
      labelSeats(f, utf8("Ghế"), 20), valueSeats(f, String(), 20),
      labelCombo(f, utf8("Combo"), 20), valueCombo(f, String(), 20),
      labelTotal(f, utf8("Tổng cộng"), 24), valueTotal(f, String(), 24)
{
    initializeUI();
}

void ConfirmationView::initializeUI() {
    float leftCol = 200.f;
    float rightCol = 500.f;
    float startY = 220.f;
    float lineHeight = 35.f;
    
    titleText.setFillColor(Color(46, 204, 113));
    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.f, titleBounds.size.y / 2.f});
    titleText.setPosition({640.f, startY + 40.f});

    startY += 70.f;
    labelTicketId.setFillColor(Color(180, 180, 180));
    labelTicketId.setPosition({leftCol, startY});
    
    valueTicketId.setFillColor(Color::White);
    valueTicketId.setPosition({rightCol, startY});
    
    startY += lineHeight;
    labelCustomer.setFillColor(Color(180, 180, 180));
    labelCustomer.setPosition({leftCol, startY});
    
    valueCustomer.setFillColor(Color::White);
    valueCustomer.setPosition({rightCol, startY});
    
    startY += lineHeight;
    labelEmail.setFillColor(Color(180, 180, 180));
    labelEmail.setPosition({leftCol, startY});
    
    valueEmail.setFillColor(Color::White);
    valueEmail.setPosition({rightCol, startY});
    
    startY += lineHeight + 10.f;
    labelMovie.setFillColor(Color(180, 180, 180));
    labelMovie.setPosition({leftCol, startY});
    
    valueMovie.setFillColor(Color::White);
    valueMovie.setPosition({rightCol, startY});
    
    startY += lineHeight;
    labelRoom.setFillColor(Color(180, 180, 180));
    labelRoom.setPosition({leftCol, startY});
    
    valueRoom.setFillColor(Color::White);
    valueRoom.setPosition({rightCol, startY});
    
    startY += lineHeight;
    labelDateTime.setFillColor(Color(180, 180, 180));
    labelDateTime.setPosition({leftCol, startY});
    
    valueDateTime.setFillColor(Color::White);
    valueDateTime.setPosition({rightCol, startY});
    
    startY += lineHeight + 10.f;
    labelSeats.setFillColor(Color(180, 180, 180));
    labelSeats.setPosition({leftCol, startY});
    
    valueSeats.setFillColor(Color::White);
    valueSeats.setPosition({rightCol, startY});
    
    startY += lineHeight;
    labelCombo.setFillColor(Color(180, 180, 180));
    labelCombo.setPosition({leftCol, startY});
    
    valueCombo.setFillColor(Color::White);
    valueCombo.setPosition({rightCol, startY});
    
    startY += lineHeight + 50.f;
    
    labelTotal.setFillColor(Color::White);
    labelTotal.setPosition({leftCol, startY});
    
    valueTotal.setFillColor(Color(255, 215, 0));
    valueTotal.setPosition({rightCol, startY});
    
    homeButton.setPosition({490.f, 720.f});
    homeButton.setFillColor(Color(20, 118, 172));
}

void ConfirmationView::setBookingData(const BookingData& data) {
    valueTicketId.setString(String::fromUtf8(data.ticketId.begin(), data.ticketId.end()));
    valueCustomer.setString(String::fromUtf8(data.customerName.begin(), data.customerName.end()));
    valueEmail.setString(String::fromUtf8(data.customerEmail.begin(), data.customerEmail.end()));
    valueMovie.setString(String::fromUtf8(data.movieName.begin(), data.movieName.end()));
    valueRoom.setString(String::fromUtf8(data.roomName.begin(), data.roomName.end()));
    
    string formattedDate = formatDateToDDMMYYYY(data.date);
    string dateTime = formattedDate + " - " + data.time;
    valueDateTime.setString(String::fromUtf8(dateTime.begin(), dateTime.end()));
    
    valueSeats.setString(String::fromUtf8(data.seatsDisplay.begin(), data.seatsDisplay.end()));
    
    string comboDisplay = data.getFormattedComboDisplay(2);
    valueCombo.setString(String::fromUtf8(comboDisplay.begin(), comboDisplay.end()));
    
    valueTotal.setString(fmtMoney(data.totalPrice));
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
    window.draw(titleText);
    
    window.draw(labelTicketId);
    window.draw(labelCustomer);
    window.draw(labelEmail);
    window.draw(labelMovie);
    window.draw(labelRoom);
    window.draw(labelDateTime);
    window.draw(labelSeats);
    window.draw(labelCombo);
    window.draw(labelTotal);
    
    window.draw(valueTicketId);
    window.draw(valueCustomer);
    window.draw(valueEmail);
    window.draw(valueMovie);
    window.draw(valueRoom);
    window.draw(valueDateTime);
    window.draw(valueSeats);
    window.draw(valueCombo);
    window.draw(valueTotal);
    
    homeButton.draw(window);
}
