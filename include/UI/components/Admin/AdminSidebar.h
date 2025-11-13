#ifndef ADMIN_SIDEBAR_H
#define ADMIN_SIDEBAR_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "core/AppState.h"

using namespace sf;
using namespace std;

/**
 * @brief Sidebar cho Admin Panel
 * 
 * Layout: 260px width, theme tối #0D1B2A
 * Chia thành 4 nhóm:
 * - QUẢN LÝ HỆ THỐNG (movies, rooms, showtimes, tickets, combos)
 * - THỐNG KÊ & BÁO CÁO (revenue, sold tickets)
 * - TÀI KHOẢN (staff, customers)
 * - HỆ THỐNG (change password, logout)
 */
class AdminSidebar {
private:
    struct MenuItem {
        string label;
        string icon;
        AppState targetState;
        bool isGroupHeader;
        bool isLogout;
        
        MenuItem(const string& l, const string& i, AppState state, bool header = false, bool logout = false)
            : label(l), icon(i), targetState(state), isGroupHeader(header), isLogout(logout) {}
    };
    
    float width;
    float height;
    Vector2f position;
    
    // Colors
    Color bgColor;          // #0D1B2A
    Color textColor;        // #E9EEF5
    Color hoverColor;       // #1B263B
    Color activeColor;      // #1476AC
    Color groupHeaderColor; // Slightly lighter
    
    // Menu items
    vector<MenuItem> menuItems;
    int hoveredIndex;
    int activeIndex;
    
    // Shapes
    RectangleShape background;
    vector<RectangleShape> itemBackgrounds;
    
    // Text
    Font font;
    unique_ptr<Text> headerText;
    unique_ptr<Text> userNameText;
    vector<unique_ptr<Text>> itemTexts;
    
    // Callback for state change
    function<void(AppState)> onStateChange;
    function<void()> onLogout;
    
    // User info
    string userName;
    RectangleShape userInfoBg;
    
    void initializeMenuItems();
    void setupGraphics();
    int getItemAtPosition(Vector2f mousePos);
    
public:
    AdminSidebar(float width, float height, const string& userName);
    
    void setPosition(Vector2f pos);
    void setOnStateChange(function<void(AppState)> callback);
    void setOnLogout(function<void()> callback);
    void setActiveState(AppState state);
    
    void handleEvent(const Event& event, const RenderWindow& window);
    void update(const RenderWindow& window);
    void render(RenderWindow& window);
    
    float getWidth() const { return width; }
};

#endif
