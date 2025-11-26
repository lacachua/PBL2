#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <array>
#include "services/AuthService.h"
#include "UI/components/Button.h"

class AdminProfilePanel {
private:
    sf::Font& font;
    float width;
    float height;
    sf::Vector2f position;
    AuthService* authService;
    User* currentUser = nullptr;
    std::string currentEmail;
    bool userDataLoaded = false;

    // Layout containers
    sf::RectangleShape panelBackground;
    sf::RectangleShape infoCard;
    sf::RectangleShape passwordCard;
    sf::RectangleShape heroBanner;
    sf::CircleShape avatarCircle;

    // Titles
    sf::Text personalInfoTitle;
    sf::Text personalInfoHint;
    sf::Text passwordTitle;
    sf::Text passwordHint;
    sf::Text heroNameText;      // "HỒ SƠ CỦA ADMIN"
    sf::Text heroMetaText;      // "email - role"
    sf::Text avatarInitialsText;

    // Info labels and inputs
    sf::Text fullNameLabel;
    sf::Text birthDateLabel;
    sf::Text phoneLabel;
    sf::Text emailLabel;

    sf::RectangleShape fullNameBox;
    sf::RectangleShape birthDateBox;
    sf::RectangleShape phoneBox;
    sf::RectangleShape emailBox;

    sf::Text fullNameText;
    sf::Text birthDateText;
    sf::Text phoneText;
    sf::Text emailText;
    std::array<sf::RectangleShape*, 3> infoBoxes;
    std::array<sf::Text*, 3> infoTexts;

    std::wstring fullNameInput;
    std::wstring birthDateInput;
    std::wstring phoneInput;

    // Password inputs
    sf::Text oldPasswordLabel;
    sf::Text newPasswordLabel;
    sf::Text confirmPasswordLabel;

    sf::RectangleShape oldPasswordBox;
    sf::RectangleShape newPasswordBox;
    sf::RectangleShape confirmPasswordBox;

    sf::Text oldPasswordText;
    sf::Text newPasswordText;
    sf::Text confirmPasswordText;
    std::array<sf::RectangleShape*, 3> passwordBoxes;

    std::string oldPasswordInput;
    std::string newPasswordInput;
    std::string confirmPasswordInput;

    // Actions
    Button saveInfoBtn;
    Button changePasswordBtn;

    // Feedback
    sf::Text infoMessage;
    sf::Text passwordMessage;
    sf::Clock infoMessageClock;
    sf::Clock passwordMessageClock;
    bool showInfoMessage = false;
    bool showPasswordMessage = false;

    // Cursor state
    sf::RectangleShape cursor;
    sf::Clock cursorClock;
    bool showCursor = true;
    int activeInfoField = -1;
    int activePasswordField = -1;

    bool wasMousePressed = false;

    void layoutElements();
    void refreshUser();
    void updateHeroContents();
    std::wstring getRoleLabel(AppRole role) const;
    std::wstring buildInitials(const std::string& source) const;
    void handleInfoInput(std::uint32_t unicode);
    void handlePasswordInput(std::uint32_t unicode);
    void saveInfoChange();
    void savePasswordChange();
    void resetPasswordInputs();
    void persistUser();

public:
    AdminProfilePanel(sf::Font& font, float width, float height, AuthService* authService);

    void setPosition(sf::Vector2f pos);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(sf::Vector2f mousePos, bool mousePressed);
    void render(sf::RenderWindow& window);
    void reload();
};
