#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "AppState.h"
#include "AuthService.h"
#include "UI/components/Button.h"
using namespace sf;
using namespace std;

class AccountScreen {
private:
    // Font reference
    const Font& font;
    
    // Current active tab
    AccountTab currentTab = AccountTab::CUSTOMER_INFO;
    
    // Sidebar menu items
    RectangleShape sidebarBg;
    Button menuItem1, menuItem2, menuItem3;
    
    // Title
    Text titleText;
    
    // Main card background (chứa cả 2 sections)
    RectangleShape mainCardBg;
    
    // Personal Info Section (section trên)
    Text personalInfoTitle;
    Text fullNameLabel, birthDateLabel, phoneLabel, emailLabel;
    RectangleShape fullNameBox, birthDateBox, phoneBox, emailBox;
    Text fullNameText, birthDateText, phoneText, emailText;
    
    // Editable data
    wstring fullNameInput, birthDateInput, phoneInput;
    
    // Change Password Section (section dưới)
    Text changePasswordTitle;
    Text oldPasswordLabel, newPasswordLabel, confirmPasswordLabel;
    RectangleShape oldPasswordBox, newPasswordBox, confirmPasswordBox;
    Text oldPasswordText, newPasswordText, confirmPasswordText;
    string oldPasswordInput, newPasswordInput, confirmPasswordInput;
    
    // Buttons - using Button class
    Button saveInfoBtn;
    Button changePasswordBtn;
    
    // Message texts
    Text infoMessage, passwordMessage;
    Clock messageTimerInfo, messageTimerPassword;
    bool showInfoMessage = false;
    bool showPasswordMessage = false;
    
    // User data
    AuthService* authService;
    string currentUserEmail;
    User* currentUser;
    
    // Active input field (for password change)
    int activeField = -1; // -1: none, 0: old, 1: new, 2: confirm
    int activeInfoField = -1; // -1: none, 0: fullName, 1: birthDate, 2: phone
    
    // Cursor for input fields
    RectangleShape cursor;
    Clock cursorClock;
    bool showCursor = true;
    
public:
    AccountScreen(const Font&, AuthService&);
    
    void setCurrentUser(const string& email);
    void update(Vector2f mousePos, bool mousePressed, const Event* event, AppState& state);
    void draw(RenderWindow&);
    
private:
    void updatePositions(Vector2u windowSize);
    void handlePasswordInput(const Event* event);
    void handleInfoInput(const Event* event);
    void savePasswordChange();
    void saveInfoChange();
};