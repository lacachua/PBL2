#pragma once

#include <SFML/Graphics.hpp>
#include "UI/components/Admin/TextBox.h"
#include "UI/components/Admin/DropdownBox.h"
#include "UI/components/Admin/MovieRepository.h"
#include "UI/components/Button.h"
#include "data-structures/DLL.h"
#include <string>
#include <memory>

using namespace sf;
using namespace std;

/**
 * @brief Movie CRUD Panel với layout chính xác theo spec
 * 
 * Layout: (x=340, y=120), size=(1320x680)
 * Table columns: ID(100), Tên phim(400), Thời lượng(140), Ngày chiếu(180), Trạng thái(180)
 */
class MoviePanel {
private:
    Font& font;
    float width, height;
    Vector2f position;
    
    // Repository
    unique_ptr<AdminMovieRepository> repository;
    
    // UI Components
    unique_ptr<Text> titleText;
    RectangleShape background;
    RectangleShape tableHeaderBg;
    DLL<RectangleShape> rowBackgrounds;
    DLL<RectangleShape> rowBorders;
    
    // CRUD Buttons (custom, pixel-perfect)
    RectangleShape btnAddBg;      // Thêm phim
    RectangleShape btnEditBg;     // Sửa thông tin
    RectangleShape btnDeleteBg;   // Xóa phim
    unique_ptr<Text> btnAddText;
    unique_ptr<Text> btnEditText;
    unique_ptr<Text> btnDeleteText;
    bool btnAddHover = false;
    bool btnEditHover = false;
    bool btnDeleteHover = false;
    bool btnAddPressed = false;
    bool btnEditPressed = false;
    bool btnDeletePressed = false;
    
    // Reload icon button
    Texture reloadTexture;
    Sprite reloadSprite;
    RectangleShape reloadButtonBg;
    bool btnReloadHover = false;
    bool btnReloadPressed = false;
    
    // Popup state
    enum PopupType { NONE, ADD, EDIT, DELETE_CONFIRM };
    PopupType currentPopup;
    int selectedRow;
    
    // Popup components
    RectangleShape popupBackground;
    RectangleShape popupOverlay;
    unique_ptr<Text> popupTitle;
    DLL<unique_ptr<TextBox>> inputBoxes;
    unique_ptr<DropdownBox> statusDropdown; // For movie status selection
    unique_ptr<Button> btnPopupSave;
    unique_ptr<Button> btnPopupCancel;
    
    // Notification
    string notificationText;
    Clock notificationClock;
    RectangleShape notificationBg;
    unique_ptr<Text> notificationTextObj;
    bool notificationVisible = false;

    // Selection warning popup
    bool selectionWarningVisible = false;
    RectangleShape selectionWarningOverlay;
    RectangleShape selectionWarningBg;
    unique_ptr<Text> selectionWarningText;
    RectangleShape selectionWarningButton;
    unique_ptr<Text> selectionWarningButtonText;
    bool selectionWarningButtonHover = false;
    string selectionWarningMessage;
    
    // Table data
    int scrollOffset;
    int hoveredRow;
    
    // Constants for layout (adjusted for panel width ~1428px)
    static constexpr float TABLE_X = 40.f;      // Reduced left margin
    static constexpr float TABLE_Y = 120.f;
    static constexpr float TABLE_WIDTH = 1100.f; // Reduced from 1320 to fit in panel
    static constexpr float TABLE_HEIGHT = 680.f;
    static constexpr float HEADER_HEIGHT = 44.f;
    static constexpr float ROW_HEIGHT = 38.f;
    
    static constexpr float COL_ID_X = 60.f;      // Adjusted
    static constexpr float COL_ID_WIDTH = 80.f;
    static constexpr float COL_TITLE_X = 150.f;  // Adjusted
    static constexpr float COL_TITLE_WIDTH = 350.f;
    static constexpr float COL_DURATION_X = 510.f; // Adjusted
    static constexpr float COL_DURATION_WIDTH = 120.f;
    static constexpr float COL_DATE_X = 640.f;   // Adjusted
    static constexpr float COL_DATE_WIDTH = 150.f;
    static constexpr float COL_STATUS_X = 800.f; // Adjusted
    static constexpr float COL_STATUS_WIDTH = 150.f;
    
    // Colors
    Color bgColor = Color(244, 246, 250);      // #F4F6FA
    Color headerColor = Color(20, 118, 172);   // #1476AC
    Color rowColor = Color(255, 255, 255);     // #FFFFFF
    Color hoverColor = Color(229, 241, 251);   // #E5F1FB
    Color selectedColor = Color(204, 228, 247); // #CCE4F7
    Color borderColor = Color(201, 206, 214);  // #C9CED6
    Color textColor = Color(34, 34, 34);       // #222222
    
    // Helper to draw rounded rectangle
    void drawRoundedRect(RenderWindow& window, const Vector2f& pos, const Vector2f& size, float radius, const Color& color);
    
    void setupUI();
    void renderTable(RenderWindow& window);
    void renderPopup(RenderWindow& window);
    void renderNotification(RenderWindow& window);
    void renderSelectionWarning(RenderWindow& window);
    
    void openAddPopup();
    void openEditPopup();
    void openDeleteConfirm();
    void closePopup();
    
    void handleAdd();
    void handleEdit();
    void handleDelete();
    void handleReload();
    
    void showNotification(const string& message);
    void showSelectionWarning(const string& message);
    void updateSelectionWarning(Vector2f mousePos, bool mousePressed);
    void handleSelectionWarningEvent(const Event& event, const RenderWindow& window);
    
    public:
        MoviePanel(Font& font, float width, float height);
    
        void setPosition(Vector2f pos);
        void handleEvent(const Event& event, const RenderWindow& window);
        void update(Vector2f mousePos, bool mousePressed);
        void render(RenderWindow& window);
    };
