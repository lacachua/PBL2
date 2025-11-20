#include "UI/components/TicketBooking/LoginRequiredPopup.h"

LoginRequiredPopup::LoginRequiredPopup(Font& f, const String& customMessage) 
        : font(f),
            overlay({1728.f, 972.f}),
            popup({600.f, 300.f}),
            messageText(font, customMessage, 24),
            backButton(font, L"Quay lại", 200.f, 60.f, 22),
            loginButton(font, L"Đăng nhập", 200.f, 60.f, 22)
{
    // Overlay mờ
    overlay.setFillColor(Color(0, 0, 0, 180));
    
    // Popup chính giữa màn hình
    popup.setPosition({(1728.f - 600.f) / 2.f, (972.f - 300.f) / 2.f});
    popup.setFillColor(Color(30, 30, 40));
    popup.setOutlineThickness(3.f);
    popup.setOutlineColor(Color(20, 118, 172));
    
    // Message text
    messageText.setFillColor(Color::White);
    FloatRect textBounds = messageText.getLocalBounds();
    messageText.setPosition({
        popup.getPosition().x + (600.f - textBounds.size.x) / 2.f,
        popup.getPosition().y + 80.f
    });
    
    // Nút "Quay lại"
    backButton.setPosition({
        popup.getPosition().x + 50.f,
        popup.getPosition().y + 200.f
    });
    backButton.setFillColor(Color(150, 50, 50));
    backButton.setTextColor(Color::Black);
    backButton.setOutlineThickness(2.f);
    backButton.setOutlineColor(Color::White);
    
    // Nút "Đăng nhập"
    loginButton.setPosition({
        popup.getPosition().x + 350.f,
        popup.getPosition().y + 200.f
    });
    loginButton.setFillColor(Color(20, 118, 172));
    loginButton.setTextColor(Color::Black);
    loginButton.setOutlineThickness(2.f);
    loginButton.setOutlineColor(Color::White);
}

void LoginRequiredPopup::update(Vector2f mousePos, bool mousePressed) {
    backButton.update(mousePos, mousePressed, Color(180, 70, 70), Color::White);
    loginButton.update(mousePos, mousePressed, Color(50, 150, 200), Color::White);
}

int LoginRequiredPopup::handleClick(Vector2f mousePos, bool mousePressed) {
    if (!mousePressed) return 0;
    
    // Click "Quay lại"
    if (backButton.getGlobalBounds().contains(mousePos)) {
        return 1;  // Signal to close popup
    }
    
    // Click "Đăng nhập"
    if (loginButton.getGlobalBounds().contains(mousePos)) {
        return 2;  // Signal to go to LOGIN state
    }
    
    return 0;
}

void LoginRequiredPopup::draw(RenderWindow& window) {
    window.draw(overlay);
    window.draw(popup);
    window.draw(messageText);
    backButton.draw(window);
    loginButton.draw(window);
}
