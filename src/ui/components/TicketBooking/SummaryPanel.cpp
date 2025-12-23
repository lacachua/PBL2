#include "UI/components/TicketBooking/SummaryPanel.h"
#include <algorithm>
using namespace std;
using namespace SummaryPanelLayout;

namespace {
    string formatCurrency(int value) {
        string digits = to_string(max(0, value));
        string formatted;
        for (int i = 0; i < (int)digits.size(); ++i) {
            formatted += digits[i];
            if ((int)digits.size() - i - 1 > 0 && ((int)digits.size() - i - 1) % 3 == 0) {
                formatted += ".";
            }
        }
        formatted += " VND";
        return formatted;
    }

    float textWidth(sf::Text& text) {
        const auto b = text.getLocalBounds();
        return b.position.x + b.size.x;
    }

    sf::String ellipsizeToWidth(const sf::Font& font, const sf::String& input, unsigned int characterSize, float maxWidth) {
        sf::Text probe(font);
        probe.setCharacterSize(characterSize);
        probe.setString(input);
        if (textWidth(probe) <= maxWidth) {
            return input;
        }

        const sf::String ellipsis = L"...";
        sf::Text withEllipsis(font);
        withEllipsis.setCharacterSize(characterSize);
        withEllipsis.setString(ellipsis);
        if (textWidth(withEllipsis) > maxWidth) {
            return ellipsis;
        }

        sf::String result;
        for (size_t i = 0; i < input.getSize(); ++i) {
            result += input[i];
            sf::Text t(font);
            t.setCharacterSize(characterSize);
            t.setString(result + ellipsis);
            if (textWidth(t) > maxWidth) {
                if (result.getSize() > 0) {
                    result.erase(result.getSize() - 1, 1);
                }
                break;
            }
        }
        return result + ellipsis;
    }
}

SummaryPanel::SummaryPanel(Font& f) : font(f) {}

void SummaryPanel::draw(RenderWindow& window, const String& movieName, const String& room, const String& date, const String& time, int ticketPrice, bool hasSelectedShowtime) {
    (void)ticketPrice;
    float boxX = PanelX;
    float boxY = PanelY;
    float yPos = boxY + 20.f;

    Text title(font, ellipsizeToWidth(font, movieName, 22, 360.f), 22);
    title.setFillColor(Color::White);
    title.setPosition({boxX + 20.f, yPos});
    window.draw(title);

    yPos += 40.f;
    Text cinema(font, L"Rạp CiNeXíNè - Đà Nẵng", 20);
    cinema.setFillColor(Color::White);
    cinema.setPosition({boxX + 20.f, yPos});
    window.draw(cinema);

    yPos += 40.f;
    
    if (!hasSelectedShowtime) {
        Text noSelection(font, L"Chưa chọn suất chiếu!", 20);
        noSelection.setFillColor(Color::Red);
        noSelection.setPosition({boxX + 20.f, yPos});
        window.draw(noSelection);
        return;
    }

    string dateStr = date.toAnsiString();
    int year = 0, month = 0, day = 0;
    char formattedDate[20] = "";
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3) 
        snprintf(formattedDate, sizeof(formattedDate), "%02d/%02d/%04d", day, month, year);
    else
        snprintf(formattedDate, sizeof(formattedDate), "%s", dateStr.c_str());

    Text info(font, L"Suất " + time + L" - " + String(formattedDate), 20);
    info.setFillColor(Color::White);
    info.setPosition({boxX + 20.f, yPos});
    window.draw(info);

    yPos += 30.f;
    Text roomText(font, room + L" - Ghế ...", 20);
    roomText.setFillColor(Color::White);
    roomText.setPosition({boxX + 20.f, yPos});
    window.draw(roomText);

    yPos += 50.f;
    RectangleShape divider({360.f, 2.f});
    divider.setPosition({boxX + 20.f, yPos});
    divider.setFillColor(Color(80, 80, 90));
    window.draw(divider);

    yPos += 20.f;
    Text totalTitle(font, L"TỔNG ĐƠN HÀNG", 20);
    totalTitle.setFillColor(Color(180, 180, 180));
    totalTitle.setPosition({boxX + 20.f, yPos});
    window.draw(totalTitle);

    // MẶC ĐỊNH là 0 VND (chưa chọn ghế/bắp nước)
    char priceStr[64];
    snprintf(priceStr, sizeof(priceStr), "0 VND");
    yPos += 30.f;

    Text price(font, String(priceStr), 28);
    price.setFillColor(Color::White);
    price.setPosition({boxX + 20.f, yPos});
    window.draw(price);
}

void SummaryPanel::drawWithSeats(RenderWindow& window, const String& movieName, const String& room, const String& date, const String& time, const String& selectedSeats, int totalPrice) {
    float boxX = PanelX;
    float boxY = PanelY;
    float yPos = boxY + 20.f;

    Text title(font, ellipsizeToWidth(font, movieName, 22, 360.f), 22);
    title.setFillColor(Color::White);
    title.setPosition({boxX + 20.f, yPos});
    window.draw(title);

    yPos += 40.f;
    Text cinema(font, L"Rạp CiNeXíNè - Đà Nẵng", 20);
    cinema.setFillColor(Color::White);
    cinema.setPosition({boxX + 20.f, yPos});
    window.draw(cinema);

    string dateStr = date.toAnsiString();
    int year = 0, month = 0, day = 0;
    char formattedDate[20] = "";
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3)
        snprintf(formattedDate, sizeof(formattedDate), "%02d/%02d/%04d", day, month, year);
    else
        snprintf(formattedDate, sizeof(formattedDate), "%s", dateStr.c_str());

    yPos += 40.f;
    Text info(font, L"Suất " + time + L" - " + String(formattedDate), 20);
    info.setFillColor(Color::White);
    info.setPosition({boxX + 20.f, yPos});
    window.draw(info);

    yPos += 30.f;
    String seatDisplay = selectedSeats.isEmpty() ? L"..." : String(selectedSeats);
    Text roomText(font, room + L" - Ghế " + seatDisplay, 20);
    roomText.setFillColor(Color::White);
    roomText.setPosition({boxX + 20.f, yPos});
    window.draw(roomText);

    yPos += 50.f;
    RectangleShape divider({360.f, 2.f});
    divider.setPosition({boxX + 20.f, yPos});
    divider.setFillColor(Color(80, 80, 90));
    window.draw(divider);

    yPos += 20.f;
    Text totalTitle(font, L"TỔNG ĐƠN HÀNG", 20);
    totalTitle.setFillColor(Color(180, 180, 180));
    totalTitle.setPosition({boxX + 20.f, yPos});
    window.draw(totalTitle);

    // Hiển thị tổng tiền với format x0.000 VND
    char priceStr[64];
    snprintf(priceStr, sizeof(priceStr), "%d VND", totalPrice);
    
    // Format số với dấu chấm
    string numStr = to_string(totalPrice);
    string formatted = "";
    int len = numStr.length();
    for (int i = 0; i < len; i++) {
        formatted += numStr[i];
        if ((len - i - 1) % 3 == 0 && i != len - 1) {
            formatted += ".";
        }
    }
    formatted += " VND";
    
    yPos += 30.f;
    Text price(font, String(formatted), 28);
    price.setFillColor(Color::White);
    price.setPosition({boxX + 20.f, yPos});
    window.draw(price);
}

void SummaryPanel::drawPayment(RenderWindow& window, const String& movieName, const String& room,
                               const String& date, const String& time, int finalTotal) {
    float boxX = PanelX;
    float boxY = PanelY;
    float yPos = boxY + 20.f;

    Text title(font, ellipsizeToWidth(font, movieName, 22, 360.f), 22);
    title.setFillColor(Color::White);
    title.setPosition({boxX + 20.f, yPos});
    window.draw(title);

    yPos += 40.f;
    Text cinema(font, L"Rạp CiNeXíNè - Đà Nẵng", 20);
    cinema.setFillColor(Color::White);
    cinema.setPosition({boxX + 20.f, yPos});
    window.draw(cinema);

    string dateStr = date.toAnsiString();
    int year = 0, month = 0, day = 0;
    char formattedDate[20] = "";
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3)
        snprintf(formattedDate, sizeof(formattedDate), "%02d/%02d/%04d", day, month, year);
    else
        snprintf(formattedDate, sizeof(formattedDate), "%s", dateStr.c_str());

    yPos += 40.f;
    Text info(font, L"Suất " + time + L" - " + String(formattedDate), 20);
    info.setFillColor(Color::White);
    info.setPosition({boxX + 20.f, yPos});
    window.draw(info);

    yPos += 30.f;
    Text roomText(font, room, 20);
    roomText.setFillColor(Color::White);
    roomText.setPosition({boxX + 20.f, yPos});
    window.draw(roomText);

    yPos += 50.f;
    RectangleShape divider({360.f, 2.f});
    divider.setPosition({boxX + 20.f, yPos});
    divider.setFillColor(Color(80, 80, 90));
    window.draw(divider);

    yPos += 20.f;
    Text totalTitle(font, L"TỔNG CỘNG", 24);
    totalTitle.setFillColor(Color(200, 200, 200));
    totalTitle.setPosition({boxX + 20.f, yPos});
    window.draw(totalTitle);

    string finalStr = formatCurrency(finalTotal);
    yPos += 35.f;
    Text price(font, String::fromUtf8(finalStr.begin(), finalStr.end()), 30);
    price.setFillColor(Color::White);
    price.setPosition({boxX + 20.f, yPos});
    window.draw(price);
}
