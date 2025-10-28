#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "AppState.h"
#include "AuthService.h"
using namespace sf;
using namespace std;

class AccountScreen {
private:
    // Title
    Text titleText;
    
    // Personal Info Section
    Text personalInfoTitle;
    Text usernameLabel, fullNameLabel, birthDateLabel, phoneLabel, emailLabel;
    RectangleShape usernameBox, fullNameBox, birthDateBox, phoneBox, emailBox;
    Text usernameText, fullNameText, birthDateText, phoneText, emailText;
    
    // Editable data
    wstring fullNameInput, birthDateInput, phoneInput;
    
    // Change Password Section
    Text changePasswordTitle;
    Text oldPasswordLabel, newPasswordLabel, confirmPasswordLabel;
    RectangleShape oldPasswordBox, newPasswordBox, confirmPasswordBox;
    Text oldPasswordText, newPasswordText, confirmPasswordText;
    string oldPasswordInput, newPasswordInput, confirmPasswordInput;
    
    // Buttons
    RectangleShape saveInfoBtn, changePasswordBtn;
    Text saveInfoBtnText, changePasswordBtnText;
    
    // Message texts
    Text infoMessage, passwordMessage;
    Clock messageTimerInfo, messageTimerPassword;
    bool showInfoMessage = false;
    bool showPasswordMessage = false;
    
    // User data
    AuthService* authService;
    string currentUserEmail;
    User* currentUser;
    
    // Hover states
    bool isSaveInfoHovered = false;
    bool isChangePasswordHovered = false;
    
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