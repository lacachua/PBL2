#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>
#include "core/AppState.h"
#include "data-structures/DLL.h"

using namespace sf;
using namespace std;

class RoundRectButton {
private:
    Font& font;
    Vector2f size;
    Vector2f position;
    float radius;
    Color baseColor;
    Color hoverColor;
    bool hovered;
    unique_ptr<Text> label;

public:
    RoundRectButton(Font& font, const string& text, Vector2f size, float radius);

    void setPosition(Vector2f pos);
    void setColors(const Color& base, const Color& hover);
    void setTextSize(unsigned int size);
    void update(Vector2f mousePos);
    bool contains(Vector2f point) const;
    void draw(RenderWindow& window) const;
    Vector2f getSize() const { return size; }
};

class AdminSidebar {
private:
    struct MenuItem {
        string label;
        AppState targetState;
        bool isGroupHeader;
        bool isLogout;
        
        MenuItem(const string& l, AppState state, bool header = false, bool logout = false)
            : label(l), targetState(state), isGroupHeader(header), isLogout(logout) {}
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
    DLL<MenuItem> menuItems;
    int hoveredIndex;
    int activeIndex;
    
    // Shapes
    RectangleShape background;
    DLL<RectangleShape> itemBackgrounds;
    
    // Text
    Font font;
    Font montserratFont;
    unique_ptr<Text> headerText;
    unique_ptr<Text> userNameText;
    DLL<unique_ptr<Text>> itemTexts;
    
    // Callback for state change
    function<void(AppState)> onStateChange;
    function<void()> onLogout;
    
    // User info
    string userName;
    RectangleShape userInfoBg;
    unique_ptr<RoundRectButton> logoutButton;
    
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