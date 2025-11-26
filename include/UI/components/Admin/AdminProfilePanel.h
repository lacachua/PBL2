#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include "services/AuthService.h"
#include "UI/components/ProfileInfoCard.h"

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
    sf::RectangleShape contentCard;
    sf::RectangleShape heroBanner;
    sf::CircleShape avatarCircle;

    // Hero section
    sf::Text heroNameText;
    sf::Text heroMetaText;
    sf::Text avatarInitialsText;

    // Profile info card (reusable component)
    ProfileInfoCard profileCard_;

    void layoutElements();
    void refreshUser();
    void updateHeroContents();
    std::wstring getRoleLabel(AppRole role) const;
    std::wstring buildInitials(const std::string& source) const;

public:
    AdminProfilePanel(sf::Font& font, float width, float height, AuthService* authService);

    void setPosition(sf::Vector2f pos);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(sf::Vector2f mousePos, bool mousePressed);
    void render(sf::RenderWindow& window);
    void reload();
};
