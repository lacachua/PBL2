#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "core/AppState.h"
#include "services/AuthService.h"
#include "UI/screens/BaseScreen.h"
#include "UI/components/Button.h"
#include "UI/components/AccountInfo/PersonalInfoView.h"
#include "UI/components/AccountInfo/PurchaseHistoryView.h"
#include "UI/components/AccountInfo/VoucherListView.h"
using namespace sf;
using namespace std;

class AccountScreen : public BaseScreen {
private:
    // Current active tab (font inherited from BaseScreen)
    AccountTab currentTab = AccountTab::CUSTOMER_INFO;
    
    // Sidebar menu items
    RectangleShape sidebarBg;
    Button menuItem1, menuItem2, menuItem3;
    
    // Main card background
    RectangleShape mainCardBg;
    
    // Component views (modular architecture)
    unique_ptr<PersonalInfoView> personalInfoView;
    unique_ptr<PurchaseHistoryView> purchaseHistoryView;
    unique_ptr<VoucherListView> voucherListView;
    
    // User data
    AuthService* authService;
    string currentUserEmail;
    
    // Debounce for menu clicks
    bool wasMousePressed = false;
    
public:
    AccountScreen(Font& f, AuthService& auth);
    
    void setCurrentUser(const string& email);
    void update(Vector2f mousePos, bool mousePressed, const Event* event, AppState& state);
    void draw(RenderWindow& window) override;
    
private:
    void updatePositions(Vector2u windowSize);
};