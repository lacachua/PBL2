#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class PasswordField {
public:
    PasswordField(const sf::Font& font, float width = 280.f, float height = 42.f);
    
    // Core methods
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void handleEvent(const sf::Event& event, sf::Vector2f mousePos, bool mousePressed);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window);
    
    // State
    void setActive(bool active);
    bool isActive() const;
    bool containsPoint(sf::Vector2f point) const;
    
    // Value
    const std::string& getValue() const;
    void setValue(const std::string& value);
    void clear();
    
    // Styling
    void setFillColor(sf::Color color);
    void setOutlineColor(sf::Color color);
    void setOutlineThickness(float thickness);
    void setActiveOutlineColor(sf::Color color);
    void setTextColor(sf::Color color);
    void setPlaceholder(const std::wstring& text);
    void setCharacterSize(unsigned int size);
    
    // Bounds
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;

private:
    void updateDisplay();
    void updateEyeIcon();
    void loadTextures();
    std::wstring getMaskedText() const;
    
    const sf::Font& font_;
    
    // Visual elements
    sf::RectangleShape box_;
    sf::Text displayText_;
    sf::Text placeholderText_;
    sf::RectangleShape cursor_;
    
    // Eye toggle
    sf::Texture eyeOpenTexture_;
    sf::Texture eyeClosedTexture_;
    std::unique_ptr<sf::Sprite> eyeSprite_;
    sf::RectangleShape eyeToggleArea_;
    
    // State
    std::string input_;
    bool active_ = false;
    bool showPassword_ = false;
    bool texturesLoaded_ = false;
    
    // Cursor blinking
    sf::Clock cursorClock_;
    bool cursorVisible_ = true;
    
    // Mouse state for edge detection
    bool wasMousePressed_ = false;
    
    // Colors
    sf::Color normalOutline_{201, 214, 226};
    sf::Color activeOutline_{20, 118, 172};
    sf::Color textColor_{0, 0, 0};
    
    // Layout constants
    static constexpr float EYE_ICON_SIZE = 18.f;
    static constexpr float EYE_PADDING = 10.f;
    static constexpr float TEXT_PADDING = 10.f;
};
