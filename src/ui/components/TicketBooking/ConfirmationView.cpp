#include "UI/components/TicketBooking/ConfirmationView.h"
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper function: Convert UTF-8 string to wstring
static wstring utf8_to_wstring(const string& str) {
    if (str.empty()) return wstring();
#ifdef _WIN32
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
#else
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
#endif
}

// Helper function: Format combo string với line break sau mỗi 2 combo
static wstring formatComboDisplay(const string& comboStr) {
    if (comboStr.empty() || comboStr == "Không có") {
        return L"Không có";
    }
    
    wstring wCombo = utf8_to_wstring(comboStr);
    wstringstream result;
    wstring current;
    int count = 0;
    
    // Split by comma
    size_t pos = 0;
    size_t found;
    wstring temp = wCombo;
    
    while ((found = temp.find(L',', pos)) != wstring::npos) {
        wstring item = temp.substr(pos, found - pos);
        // Trim whitespace
        size_t start = item.find_first_not_of(L" \t");
        size_t end = item.find_last_not_of(L" \t");
        if (start != wstring::npos) {
            item = item.substr(start, end - start + 1);
        }
        
        if (count > 0 && count % 2 == 0) {
            result << L"\n" << item;
        } else {
            if (count > 0) result << L", ";
            result << item;
        }
        count++;
        pos = found + 1;
    }
    
    // Last item
    if (pos < temp.length()) {
        wstring item = temp.substr(pos);
        size_t start = item.find_first_not_of(L" \t");
        size_t end = item.find_last_not_of(L" \t");
        if (start != wstring::npos) {
            item = item.substr(start, end - start + 1);
            if (count > 0 && count % 2 == 0) {
                result << L"\n" << item;
            } else {
                if (count > 0) result << L", ";
                result << item;
            }
        }
    }
    
    return result.str();
}

// Helper function: Convert date format (YYYY-MM-DD or any format) to DD/MM/YYYY
static string formatDateToDDMMYYYY(const string& dateStr) {
    // Nếu đã đúng format DD/MM/YYYY (có chứa '/'), giữ nguyên
    if (dateStr.find('/') != string::npos) {
        return dateStr;
    }
    
    // Nếu format YYYY-MM-DD hoặc YYYYMMDD, chuyển đổi
    if (dateStr.length() >= 8) {
        string year, month, day;
        
        if (dateStr.find('-') != string::npos) {
            // Format: YYYY-MM-DD
            stringstream ss(dateStr);
            getline(ss, year, '-');
            getline(ss, month, '-');
            getline(ss, day, '-');
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
      labelTicketId(f, L"Mã vé", 20), valueTicketId(f, L"", 20),
      labelCustomer(f, L"Khách hàng", 20), valueCustomer(f, L"", 20),
      labelEmail(f, L"Email", 20), valueEmail(f, L"", 20),
      labelMovie(f, L"Phim", 20), valueMovie(f, L"", 20),
      labelRoom(f, L"Phòng", 20), valueRoom(f, L"", 20),
      labelDateTime(f, L"Ngày & Giờ", 20), valueDateTime(f, L"", 20),
      labelSeats(f, L"Ghế", 20), valueSeats(f, L"", 20),
      labelCombo(f, L"Combo", 20), valueCombo(f, L"", 20),
      labelTotal(f, L"Tổng cộng", 24), valueTotal(f, L"", 24)
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
    
    valueTicketId.setFillColor(Color::White);
    valueTicketId.setPosition({rightCol, startY});
    
    // Khách hàng
    startY += lineHeight;
    labelCustomer.setFillColor(Color(180, 180, 180));
    labelCustomer.setPosition({leftCol, startY});
    
    valueCustomer.setFillColor(Color::White);
    valueCustomer.setPosition({rightCol, startY});
    
    // Email
    startY += lineHeight;
    labelEmail.setFillColor(Color(180, 180, 180));
    labelEmail.setPosition({leftCol, startY});
    
    valueEmail.setFillColor(Color::White);
    valueEmail.setPosition({rightCol, startY});
    
    // Phim
    startY += lineHeight + 10.f;
    labelMovie.setFillColor(Color(180, 180, 180));
    labelMovie.setPosition({leftCol, startY});
    
    valueMovie.setFillColor(Color::White);
    valueMovie.setPosition({rightCol, startY});
    
    // Phòng
    startY += lineHeight;
    labelRoom.setFillColor(Color(180, 180, 180));
    labelRoom.setPosition({leftCol, startY});
    
    valueRoom.setFillColor(Color::White);
    valueRoom.setPosition({rightCol, startY});
    
    // Ngày giờ
    startY += lineHeight;
    labelDateTime.setFillColor(Color(180, 180, 180));
    labelDateTime.setPosition({leftCol, startY});
    
    valueDateTime.setFillColor(Color::White);
    valueDateTime.setPosition({rightCol, startY});
    
    // Ghế
    startY += lineHeight + 10.f;
    labelSeats.setFillColor(Color(180, 180, 180));
    labelSeats.setPosition({leftCol, startY});
    
    valueSeats.setFillColor(Color::White);
    valueSeats.setPosition({rightCol, startY});
    
    // Combo
    startY += lineHeight;
    labelCombo.setFillColor(Color(180, 180, 180));
    labelCombo.setPosition({leftCol, startY});
    
    valueCombo.setFillColor(Color::White);
    valueCombo.setPosition({rightCol, startY});
    
    // Divider line before total
    startY += lineHeight + 50.f;
    
    // Tổng cộng
    labelTotal.setFillColor(Color::White);
    labelTotal.setPosition({leftCol, startY});
    
    valueTotal.setFillColor(Color(255, 215, 0));
    valueTotal.setPosition({rightCol, startY});
    
    // Home button
    homeButton.setPosition({490.f, 720.f});
    homeButton.setFillColor(Color(20, 118, 172));
}

void ConfirmationView::setBookingData(const BookingData& data) {
    // Mã vé
    valueTicketId.setString(String::fromUtf8(data.ticketId.begin(), data.ticketId.end()));
    
    // Khách hàng
    valueCustomer.setString(String::fromUtf8(data.customerName.begin(), data.customerName.end()));
    
    // Email
    valueEmail.setString(String::fromUtf8(data.customerEmail.begin(), data.customerEmail.end()));
    
    // Phim
    valueMovie.setString(String::fromUtf8(data.movieName.begin(), data.movieName.end()));
    
    // Phòng
    valueRoom.setString(String::fromUtf8(data.roomName.begin(), data.roomName.end()));
    
    // Ngày & Giờ - Format ngày sang DD/MM/YYYY
    string formattedDate = formatDateToDDMMYYYY(data.date);
    string dateTime = formattedDate + " - " + data.time;
    valueDateTime.setString(String::fromUtf8(dateTime.begin(), dateTime.end()));
    
    // Ghế
    valueSeats.setString(String::fromUtf8(data.seatsDisplay.begin(), data.seatsDisplay.end()));
    
    // Combo - ✅ Format với line break mỗi 2 combo
    string comboDisplay = data.getFormattedComboDisplay(2);  // Max 2 combos per line
    valueCombo.setString(String::fromUtf8(comboDisplay.begin(), comboDisplay.end()));
    
    // Tổng tiền - Format with thousand separators
    stringstream ss;
    ss << data.totalPrice;
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
