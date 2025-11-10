#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "services/AuthService.h"
#include "UI/components/Button.h"
using namespace sf;
using namespace std;

class PersonalInfoView {
private:
    const Font& font;
    AuthService* authService;
    User* currentUser;
    string currentUserEmail;
    bool userDataLoaded = false;
    
    // Personal Info Section
    Text personalInfoTitle;
    Text fullNameLabel, birthDateLabel, phoneLabel, emailLabel;
    RectangleShape fullNameBox, birthDateBox, phoneBox, emailBox;
    Text fullNameText, birthDateText, phoneText, emailText;
    wstring fullNameInput, birthDateInput, phoneInput;
    
    // Change Password Section
    Text changePasswordTitle;
    Text oldPasswordLabel, newPasswordLabel, confirmPasswordLabel;
    RectangleShape oldPasswordBox, newPasswordBox, confirmPasswordBox;
    Text oldPasswordText, newPasswordText, confirmPasswordText;
    string oldPasswordInput, newPasswordInput, confirmPasswordInput;
    
    // Buttons (using Button class)
    Button saveInfoBtn, changePasswordBtn;
    
    // Messages
    Text infoMessage, passwordMessage;
    Clock messageTimerInfo, messageTimerPassword;
    bool showInfoMessage = false;
    bool showPasswordMessage = false;
    
    // Active fields
    int activeField = -1; // -1: none, 0: old, 1: new, 2: confirm
    int activeInfoField = -1; // -1: none, 0: fullName, 1: birthDate, 2: phone
    
    // Cursor
    RectangleShape cursor;
    Clock cursorClock;
    bool showCursor = true;
    
    // Debounce
    bool wasMousePressed = false;
    
public:
    PersonalInfoView(const Font& f, AuthService& auth);
    
    void setUser(const string& email);
    void update(Vector2f mousePos, bool mousePressed, const Event* event, Vector2f cardPos);
    void draw(RenderWindow& window);
    
private:
    void handlePasswordInput(const Event* event);
    void handleInfoInput(const Event* event);
    void savePasswordChange();
    void saveInfoChange();
};
