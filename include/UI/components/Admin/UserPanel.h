#pragma once

#include <SFML/Graphics.hpp>
#include "services/UserRepository.h"
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <optional>

using namespace sf;
using namespace std;

/**
 * @brief User Management Panel
 * 
 * Layout: (x=340, y=120), size=(1320x680)
 * Table columns: Email(250), Tên(250), SĐT(150), Vai trò(150), Trạng thái(150)
 */
class UserPanel {
private:
    Font& font;
    float width, height;
    Vector2f position;
    
    // Repository
    unique_ptr<UserRepository> repository;
    vector<User> userList; // Cache for display
    
    // UI Components
    unique_ptr<Text> titleText;
    RectangleShape background;
    RectangleShape tableHeaderBg;
    RectangleShape tableBodyBg;

    struct ActionButton {
        RectangleShape box;
        unique_ptr<Text> label;
        Color baseColor;
        Color hoverColor;
        bool hovered = false;
    };

    ActionButton btnViewInfo;
    ActionButton btnLock;
    ActionButton btnDelete;
    ActionButton btnRefresh;
    Texture reloadTexture;
    std::optional<Sprite> reloadSprite;

    // Notification
    RectangleShape notificationBg;
    unique_ptr<Text> notificationText;
    string notificationMessage;
    Clock notificationClock;
    bool notificationVisible = false;
    Color notificationColor = Color(20, 118, 172);

    // Table state
    int selectedRow = -1;
    int hoveredRow = -1;
    int scrollOffset = 0;
    
    // Constants for layout
    static constexpr float TABLE_X = 32.f;
    static constexpr float TABLE_Y = 140.f;
    static constexpr float TABLE_WIDTH = 1152.f;
    static constexpr float TABLE_HEIGHT = 560.f;
    static constexpr float HEADER_HEIGHT = 48.f;
    static constexpr float ROW_HEIGHT = 44.f;

    // Colors aligned with Admin theme
    Color bgColor = Color(244, 246, 250);
    Color headerColor = Color(20, 118, 172);
    Color tableHeaderColor = Color(27, 38, 59);
    Color rowColor = Color::White;
    Color hoverColor = Color(235, 244, 252);
    Color selectedColor = Color(214, 232, 248);
    Color borderColor = Color(210, 216, 224);
    Color textColor = Color(33, 37, 41);

    void setupUI();
    void setupButton(ActionButton& button, const string& labelUtf8, const Color& base, const Color& hover, Vector2f size);
    void updateButton(ActionButton& button, Vector2f mousePos);

    void refreshData();
    void renderTable(RenderWindow& window);
    void renderButtons(RenderWindow& window);
    void renderNotification(RenderWindow& window);
    void showNotification(const string& message, const Color& color = Color(20, 118, 172));
    string formatDate(time_t timestamp) const;
    void layoutElements();
    
public:
    UserPanel(Font& font, float width, float height);
    
    void setPosition(Vector2f pos);
    void handleEvent(const Event& event, const RenderWindow& window);
    void update(Vector2f mousePos, bool mousePressed);
    void render(RenderWindow& window);
};
