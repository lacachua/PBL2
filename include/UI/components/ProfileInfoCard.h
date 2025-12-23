#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "services/AuthService.h"
#include "UI/components/Button.h"
#include "UI/components/PasswordField.h"

class ProfileInfoCard {
public:
    // Configuration struct for flexible customization
    struct Config {
        float width;
        float cardPadding;
        float fieldWidth;
        float fieldHeight;
        float passwordFieldWidth;
        float rowGap;
        float labelInputGap;
        float sectionGap;
        bool showPasswordSection;
        bool darkMode;
        
        // Colors
        sf::Color backgroundColor;
        sf::Color labelColor;
        sf::Color titleColor;
        sf::Color inputBgColor;
        sf::Color inputTextColor;
        sf::Color outlineColor;
        sf::Color accentColor;
        sf::Color readOnlyBgColor;
        sf::Color readOnlyTextColor;
        sf::Color successColor;
        sf::Color errorColor;
        
        // Default constructor with default values
        Config() :
            width(656.f),
            cardPadding(26.f),
            fieldWidth(280.f),
            fieldHeight(42.f),
            passwordFieldWidth(604.f),
            rowGap(18.f),
            labelInputGap(10.f),
            sectionGap(24.f),
            showPasswordSection(true),
            darkMode(true),
            backgroundColor(30, 41, 59),
            labelColor(191, 215, 234),
            titleColor(238, 238, 238),
            inputBgColor(255, 255, 255),
            inputTextColor(13, 27, 42),
            outlineColor(201, 214, 226),
            accentColor(0, 153, 255),
            readOnlyBgColor(233, 236, 239),
            readOnlyTextColor(107, 114, 128),
            successColor(60, 160, 90),
            errorColor(200, 60, 60)
        {}
    };

    ProfileInfoCard(const sf::Font& font, AuthService& auth, const Config& config = Config());
    
    // Setup
    void setUser(const std::string& email);
    void setPosition(sf::Vector2f pos);
    
    // Core loop
    void handleEvent(const sf::Event& event, sf::Vector2f mousePos, bool mousePressed);
    void update(sf::Vector2f mousePos, bool mousePressed);
    void draw(sf::RenderWindow& window);
    
    // Getters
    sf::Vector2f getSize() const;
    float getTotalHeight() const;

private:
    void setupStyles();
    void layoutElements();
    void handleInfoInput(const sf::Event* event);
    void saveInfoChange();
    void savePasswordChange();
    void deactivateAllPasswordFields();
    void showInfoMessage(const std::wstring& msg, bool isError);
    void showPwdMessage(const std::wstring& msg, bool isError);

    const sf::Font& font_;
    AuthService* authService_;
    User* currentUser_ = nullptr;
    std::string currentEmail_;
    bool userDataLoaded_ = false;
    Config config_;
    sf::Vector2f position_;

    // Section 1: Personal Info
    sf::Text personalInfoTitle_;
    sf::Text fullNameLabel_, birthDateLabel_, phoneLabel_, emailLabel_;
    sf::RectangleShape fullNameBox_, birthDateBox_, phoneBox_, emailBox_;
    sf::Text fullNameText_, birthDateText_, phoneText_, emailText_;
    std::wstring fullNameInput_, birthDateInput_, phoneInput_;
    Button saveInfoBtn_;
    sf::Text infoMessage_;
    sf::Clock infoMsgClock_;
    bool showInfoMsg_ = false;

    // Section 2: Password Change
    sf::Text passwordTitle_;
    sf::Text oldPwdLabel_, newPwdLabel_, confirmPwdLabel_;
    PasswordField oldPwdField_, newPwdField_, confirmPwdField_;
    Button changePwdBtn_;
    sf::Text pwdMessage_;
    sf::Clock pwdMsgClock_;
    bool showPwdMsg_ = false;

    // State
    int activeInfoField_ = -1;  // 0: fullName, 1: birthDate, 2: phone
    sf::RectangleShape cursor_;
    sf::Clock cursorClock_;
    bool showCursor_ = true;
    bool wasMousePressed_ = false;
};
