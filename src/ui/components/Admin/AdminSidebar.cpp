#include "UI/components/Admin/AdminSidebar.h"
#include <iostream>

AdminSidebar::AdminSidebar(float w, float h, const string& uName)
    : width(w), height(h), userName(uName), hoveredIndex(-1), activeIndex(0),
    font("../assets/fonts/quicksand_medium.ttf") {
    
    // Colors
    bgColor = Color(13, 27, 42);         // #0D1B2A
    textColor = Color(233, 238, 245);    // #E9EEF5
    hoverColor = Color(27, 38, 59);      // #1B263B
    activeColor = Color(20, 118, 172);   // #1476AC
    groupHeaderColor = Color(50, 60, 80);
    
    initializeMenuItems();
    setupGraphics();
}

void AdminSidebar::initializeMenuItems() {
    menuItems.clear();
    
    // Header (user info area - will be handled separately)
    
    // Group 1: QUẢN LÝ HỆ THỐNG
    menuItems.push_back(MenuItem("QUẢN LÝ HỆ THỐNG", "", AppState::ADMIN_DASHBOARD, true, false));
    menuItems.push_back(MenuItem("Quản lý phim", "🎬", AppState::ADMIN_MOVIES));
    menuItems.push_back(MenuItem("Quản lý phòng chiếu", "🏠", AppState::ADMIN_ROOMS));
    menuItems.push_back(MenuItem("Quản lý suất chiếu", "⏰", AppState::ADMIN_SHOWTIMES));
    menuItems.push_back(MenuItem("Quản lý vé / đặt vé", "💺", AppState::ADMIN_TICKETS));
    menuItems.push_back(MenuItem("Quản lý combo bắp nước", "🍿", AppState::ADMIN_COMBOS));
    
    // Group 2: THỐNG KÊ & BÁO CÁO
    menuItems.push_back(MenuItem("THỐNG KÊ & BÁO CÁO", "", AppState::ADMIN_DASHBOARD, true, false));
    menuItems.push_back(MenuItem("Doanh thu", "💰", AppState::ADMIN_REVENUE));
    menuItems.push_back(MenuItem("Vé đã bán / lượng khách", "📈", AppState::ADMIN_SOLD_TICKETS));
    
    // Group 3: TÀI KHOẢN
    menuItems.push_back(MenuItem("TÀI KHOẢN", "", AppState::ADMIN_DASHBOARD, true, false));
    menuItems.push_back(MenuItem("Quản lý nhân viên (Staff)", "👨‍💼", AppState::ADMIN_STAFF));
    menuItems.push_back(MenuItem("Quản lý khách hàng (Customer)", "👤", AppState::ADMIN_CUSTOMERS));
    
    // Group 4: HỆ THỐNG
    menuItems.push_back(MenuItem("HỆ THỐNG", "", AppState::ADMIN_DASHBOARD, true, false));
    menuItems.push_back(MenuItem("Đổi mật khẩu", "🔑", AppState::ADMIN_CHANGE_PASSWORD));
    menuItems.push_back(MenuItem("Đăng xuất", "🚪", AppState::LOGIN, false, true));
}

void AdminSidebar::setupGraphics() {
    // Background
    background.setSize(Vector2f(width, height));
    background.setFillColor(bgColor);
    
    // User info background
    userInfoBg.setSize(Vector2f(width, 80));
    userInfoBg.setFillColor(Color(10, 20, 35)); // Slightly darker
    
    // User name text - SFML 3 requires font in constructor, use unique_ptr
    userNameText = make_unique<Text>(font);
    userNameText->setCharacterSize(16);
    userNameText->setFillColor(textColor);
    userNameText->setString(String::fromUtf8(userName.begin(), userName.end()));
    
    // Header text - SFML 3 requires font in constructor, use unique_ptr
    headerText = make_unique<Text>(font);
    headerText->setCharacterSize(20);
    headerText->setFillColor(textColor);
    headerText->setString("CineXine - Admin Panel");
    headerText->setStyle(Text::Bold);
    
    // Create item backgrounds and texts
    itemBackgrounds.clear();
    itemTexts.clear();
    
    float yOffset = 100; // Start after user info
    float itemHeight = 45;
    float groupHeaderHeight = 35;
    
    for (size_t i = 0; i < menuItems.size(); i++) {
        const auto& item = menuItems[i];
        
        float currentHeight = item.isGroupHeader ? groupHeaderHeight : itemHeight;
        
        // Background
        RectangleShape itemBg;
        itemBg.setSize(Vector2f(width - 20, currentHeight - 5));
        itemBg.setPosition(Vector2f(10, yOffset)); // SFML 3: use Vector2f
        itemBg.setFillColor(Color::Transparent);
        
        if (item.isGroupHeader) {
            itemBg.setFillColor(groupHeaderColor);
        }
        
        itemBackgrounds.push_back(itemBg);
        
        // Text - SFML 3 requires font in constructor, use unique_ptr
        auto itemText = make_unique<Text>(font);
        itemText->setCharacterSize(item.isGroupHeader ? 13 : 15);
        itemText->setFillColor(textColor);
        
        string displayText = item.icon.empty() ? item.label : item.icon + " " + item.label;
        itemText->setString(String::fromUtf8(displayText.begin(), displayText.end()));
        
        // SFML 3: getLocalBounds() returns FloatRect, use .size.y instead of .height
        itemText->setPosition(Vector2f(
            item.isGroupHeader ? 15.f : 25.f,
            yOffset + (currentHeight - itemText->getLocalBounds().size.y) / 2 - 5
        ));
        
        if (item.isGroupHeader) {
            itemText->setStyle(Text::Bold);
        }
        
        itemTexts.push_back(move(itemText));
        
        yOffset += currentHeight;
    }
}

void AdminSidebar::setPosition(Vector2f pos) {
    position = pos;
    background.setPosition(pos);
    userInfoBg.setPosition(Vector2f(pos.x, pos.y + 10)); // SFML 3: use Vector2f
    if (headerText) headerText->setPosition(Vector2f(pos.x + 15, pos.y + 20));
    if (userNameText) userNameText->setPosition(Vector2f(pos.x + 15, pos.y + 55));
    
    // Update item positions
    for (size_t i = 0; i < itemBackgrounds.size(); i++) {
        Vector2f itemPos = itemBackgrounds[i].getPosition();
        itemBackgrounds[i].setPosition(Vector2f(pos.x + itemPos.x, pos.y + itemPos.y));
        
        if (i < itemTexts.size() && itemTexts[i]) {
            Vector2f textPos = itemTexts[i]->getPosition();
            itemTexts[i]->setPosition(Vector2f(pos.x + textPos.x, pos.y + textPos.y));
        }
    }
}

void AdminSidebar::setOnStateChange(function<void(AppState)> callback) {
    onStateChange = callback;
}

void AdminSidebar::setOnLogout(function<void()> callback) {
    onLogout = callback;
}

void AdminSidebar::setActiveState(AppState state) {
    for (size_t i = 0; i < menuItems.size(); i++) {
        if (menuItems[i].targetState == state && !menuItems[i].isGroupHeader) {
            activeIndex = i;
            break;
        }
    }
}

int AdminSidebar::getItemAtPosition(Vector2f mousePos) {
    for (size_t i = 0; i < itemBackgrounds.size(); i++) {
        if (menuItems[i].isGroupHeader) continue;
        
        FloatRect bounds = itemBackgrounds[i].getGlobalBounds();
        if (bounds.contains(mousePos)) {
            return i;
        }
    }
    return -1;
}

void AdminSidebar::handleEvent(const Event& event, const RenderWindow& window) {
    // SFML 3: Use event.is<Event::Type>() instead of event.type
    if (const auto* mouseButtonEvent = event.getIf<Event::MouseButtonPressed>()) {
        if (mouseButtonEvent->button == Mouse::Button::Left) { // SFML 3: Mouse::Button::Left
            Vector2f mousePos = window.mapPixelToCoords(
                Mouse::getPosition(window)
            );
            
            int clickedIndex = getItemAtPosition(mousePos);
            
            if (clickedIndex >= 0) {
                const auto& item = menuItems[clickedIndex];
                
                if (item.isLogout) {
                    // Logout
                    if (onLogout) {
                        onLogout();
                    }
                } else {
                    // Change state
                    activeIndex = clickedIndex;
                    if (onStateChange) {
                        onStateChange(item.targetState);
                    }
                }
            }
        }
    }
}

void AdminSidebar::update(const RenderWindow& window) {
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    hoveredIndex = getItemAtPosition(mousePos);
    
    // Update item backgrounds based on hover/active state
    for (size_t i = 0; i < itemBackgrounds.size(); i++) {
        if (menuItems[i].isGroupHeader) {
            itemBackgrounds[i].setFillColor(groupHeaderColor);
        } else if (i == activeIndex) {
            itemBackgrounds[i].setFillColor(activeColor);
        } else if (i == hoveredIndex) {
            itemBackgrounds[i].setFillColor(hoverColor);
        } else {
            itemBackgrounds[i].setFillColor(Color::Transparent);
        }
    }
}

void AdminSidebar::render(RenderWindow& window) {
    // Draw background
    window.draw(background);
    
    // Draw user info section
    window.draw(userInfoBg);
    if (headerText) window.draw(*headerText);
    if (userNameText) window.draw(*userNameText);
    
    // Draw menu items
    for (size_t i = 0; i < itemBackgrounds.size(); i++) {
        window.draw(itemBackgrounds[i]);
        if (i < itemTexts.size() && itemTexts[i]) {
            window.draw(*itemTexts[i]);
        }
    }
}
