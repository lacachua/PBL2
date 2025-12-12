#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <set>
#include "repositories/admin/AdminVoucherRepository.h"
#include "UI/components/Admin/TextBox.h"
#include "UI/components/Admin/UserSelectPopup.h"
#include "UI/components/Button.h"

using namespace sf;
using namespace std;

/**
 * @brief Voucher Management Panel for Admin
 * 
 * Features:
 * - Hiển thị danh sách voucher definitions
 * - Thêm/Sửa/Xóa voucher definition
 * - Cấp voucher đồng loạt cho tất cả active users
 * - Xem danh sách users đang có voucher
 * - Thêm/Xóa voucher cho từng user cụ thể
 * 
 * Layout:
 * [Left Panel - Voucher Definitions] [Right Panel - User Vouchers]
 */
class VoucherPanel {
public:
    VoucherPanel(Font& font, float width, float height);
    
    void setPosition(Vector2f pos);
    void handleEvent(const Event& event, const RenderWindow& window);
    void update(Vector2f mousePos, bool mousePressed);
    void render(RenderWindow& window);
    
private:
    Font& font;
    float width, height;
    Vector2f position;
    
    // Repository
    unique_ptr<AdminVoucherRepository> repository;
    
    // Current state
    enum class ViewMode { DEFINITIONS, USER_VOUCHERS };
    ViewMode currentMode = ViewMode::DEFINITIONS;
    int selectedVoucherIndex = -1;
    int hoveredVoucherIndex = -1;
    string selectedVoucherCode = "";
    
    // User voucher view state
    int selectedUserIndex = -1;
    int hoveredUserIndex = -1;
    vector<VoucherUserInfo> currentVoucherUsers;
    
    // Scroll states
    int defScrollOffset = 0;
    int userScrollOffset = 0;
    
    // UI Components
    RectangleShape background;
    RectangleShape leftPanelBg;
    RectangleShape rightPanelBg;
    unique_ptr<Text> titleText;
    unique_ptr<Text> leftPanelTitle;
    unique_ptr<Text> rightPanelTitle;
    
    // Table headers
    RectangleShape defTableHeader;
    RectangleShape userTableHeader;
    
    // Action buttons - Voucher Definition
    struct ActionButton {
        RectangleShape box;
        unique_ptr<Text> label;
        Color baseColor;
        Color hoverColor;
        bool hovered = false;
    };
    
    ActionButton btnAddVoucher;
    ActionButton btnEditVoucher;
    ActionButton btnDeleteVoucher;
    ActionButton btnDistribute;  // Cấp voucher đồng loạt
    ActionButton btnViewUsers;   // Xem users có voucher
    ActionButton btnRefresh;
    
    // Action buttons - User Voucher
    ActionButton btnAddToUser;
    ActionButton btnRemoveFromUser;
    ActionButton btnBackToList;
    
    Texture reloadTexture;
    optional<Sprite> reloadSprite;
    
    // Popup
    enum class PopupType { NONE, ADD_VOUCHER, EDIT_VOUCHER, DELETE_VOUCHER, 
                          DISTRIBUTE_VOUCHER, ADD_TO_USER };
    PopupType currentPopup = PopupType::NONE;
    
    RectangleShape popupOverlay;
    RectangleShape popupBackground;
    unique_ptr<Text> popupTitle;
    vector<unique_ptr<TextBox>> inputBoxes;
    vector<unique_ptr<Text>> inputLabels;
    unique_ptr<Button> btnPopupSave;
    unique_ptr<Button> btnPopupCancel;
    
    // User selection popup
    unique_ptr<UserSelectPopup> userSelectPopup_;
    
    // Legacy user selection (to be removed after migration)
    vector<pair<string, string>> availableUsers;  // All available users (email, name)
    vector<int> filteredAvailableUsers;           // Indices into availableUsers (after search filter)
    set<int> selectedUserSet;                     // Set of selected user indices for multi-select
    int selectedAvailableUserIndex = -1;
    int hoveredAvailableUserIndex = -1;
    int availableUserScrollOffset = 0;
    string userSearchFilter;                      // Search filter for users
    RectangleShape userListBg;
    unique_ptr<Button> btnSelectAll;              // Select all / Deselect all button
    unique_ptr<TextBox> searchBox;                // Search box for filtering users
    
    // Helper method to filter users by search query
    void filterUsers(const string& query);
    
    // Notification
    string notificationMessage;
    Clock notificationClock;
    bool notificationVisible = false;
    Color notificationColor;
    RectangleShape notificationBg;
    unique_ptr<Text> notificationText;
    
    // Layout constants
    static constexpr float LEFT_PANEL_WIDTH = 580.f;
    static constexpr float RIGHT_PANEL_WIDTH = 540.f;
    static constexpr float PANEL_MARGIN = 20.f;
    static constexpr float HEADER_HEIGHT = 48.f;
    static constexpr float ROW_HEIGHT = 52.f;
    static constexpr float BUTTON_HEIGHT = 44.f;
    static constexpr float BUTTON_RADIUS = 8.f;
    
    // Colors
    Color bgColor = Color(244, 246, 250);
    Color panelBgColor = Color::White;
    Color headerColor = Color(20, 118, 172);
    Color tableHeaderColor = Color(27, 38, 59);
    Color rowColor = Color::White;
    Color rowAltColor = Color(248, 250, 252);
    Color hoverColor = Color(235, 244, 252);
    Color selectedColor = Color(214, 232, 248);
    Color borderColor = Color(210, 216, 224);
    Color textColor = Color(33, 37, 41);
    Color accentColor = Color(20, 118, 172);
    Color successColor = Color(40, 167, 69);
    Color warningColor = Color(255, 193, 7);
    Color dangerColor = Color(220, 53, 69);
    
    // Setup methods
    void setupUI();
    void setupButton(ActionButton& button, const string& labelUtf8, 
                     const Color& base, const Color& hover, Vector2f size);
    void layoutElements();
    
    // Render methods
    void renderLeftPanel(RenderWindow& window);
    void renderRightPanel(RenderWindow& window);
    void renderVoucherDefinitions(RenderWindow& window);
    void renderUserVouchers(RenderWindow& window);
    void renderButtons(RenderWindow& window);
    void renderPopup(RenderWindow& window);
    void renderNotification(RenderWindow& window);
    
    // Update methods
    void updateButton(ActionButton& button, Vector2f mousePos);
    void refreshData();
    void loadUsersForVoucher(const string& code);
    
    // Popup methods
    void openAddVoucherPopup();
    void openEditVoucherPopup();
    void openDeleteVoucherPopup();
    void openDistributePopup();
    void openAddToUserPopup();
    void closePopup();
    
    // Action methods
    void handleAddVoucher();
    void handleEditVoucher();
    void handleDeleteVoucher();
    void handleDistribute();
    void handleAddToUser();
    void handleRemoveFromUser();
    
    // Helper methods
    void showNotification(const string& message, const Color& color = Color(20, 118, 172));
    string formatCurrency(double amount) const;
    string formatDate(const string& raw) const;
    string getTypeString(int type) const;
};
