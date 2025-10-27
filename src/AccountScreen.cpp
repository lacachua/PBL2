#include "AccountScreen.h"

AccountScreen::AccountScreen(const Font& font) 
    : welcomeText(font, L"Xin chào!", 32),
      logoutBtn({120.f, 40.f}),
      logoutBtnText(font, L"Đăng xuất", 18)
{
    // Setup welcome text
    welcomeText.setFillColor(Color::White);
    
    // Setup logout button
    logoutBtn.setFillColor(Color(20, 118, 172)); // #1476AC
    logoutBtnText.setFillColor(Color::White);
    
    // Center logout button text
    FloatRect textBounds = logoutBtnText.getLocalBounds();
    logoutBtnText.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
}

void AccountScreen::setCurrentUser(const string& username) {
    currentUser = username;
    
    // Tạo text chào mừng
    wstring welcomeMsg = L"Xin chào, " + wstring(username.begin(), username.end()) + L"!";
    welcomeText.setString(welcomeMsg);
    
    // Center welcome text
    FloatRect bounds = welcomeText.getLocalBounds();
    welcomeText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
}

void AccountScreen::update(Vector2f mouse, bool mousePressed, AppState& state) {
    // Check hover on logout button
    isLogoutHovered = logoutBtn.getGlobalBounds().contains(mouse);
    
    // Update button color based on hover
    if (isLogoutHovered) {
        logoutBtn.setFillColor(Color(45, 156, 219)); // #2D9CDB - hover color
    } else {
        logoutBtn.setFillColor(Color(20, 118, 172)); // #1476AC - normal color
    }
    
    // Handle logout click
    if (mousePressed && isLogoutHovered) {
        state = AppState::HOME; // Quay về HOME thay vì LOGIN
    }
}

void AccountScreen::draw(RenderWindow& window) {
    // Clear with dark background similar to HomeScreen
    window.clear(Color(13, 27, 42)); // #0D1B2A
    
    auto size = window.getSize();
    Vector2f center(size.x / 2.f, size.y / 2.f);
    
    // Position welcome text at center
    welcomeText.setPosition(center);
    
    // Position logout button at top-right corner
    logoutBtn.setPosition({size.x - 140.f, 20.f});
    logoutBtnText.setPosition({
        logoutBtn.getPosition().x + logoutBtn.getSize().x / 2.f,
        logoutBtn.getPosition().y + logoutBtn.getSize().y / 2.f
    });
    
    // Draw components
    window.draw(welcomeText);
    window.draw(logoutBtn);
    window.draw(logoutBtnText);
}