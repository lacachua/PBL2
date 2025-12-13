#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>
#include <cstdint>

/**
 * @brief Reusable Input Box Component
 * 
 * Features:
 * - Self-managed focus handling
 * - Placeholder text support
 * - Visual feedback (border color on focus)
 * - Search icon option
 * - Debounced text change callback
 * 
 * Usage:
 *   UIInputBox searchBox(font);
 *   searchBox.setPlaceholder("Search users...");
 *   searchBox.setOnTextChange([](const std::string& text) { ... });
 *   searchBox.handleEvent(event);
 *   searchBox.render(target);
 */
class UIInputBox {
public:
    struct Config {
        float width;
        float height;
        float padding;
        unsigned int fontSize;
        bool showSearchIcon;
        float borderRadius;
        
        sf::Color backgroundColor;
        sf::Color borderColor;
        sf::Color borderFocusColor;
        sf::Color textColor;
        sf::Color placeholderColor;
        sf::Color disabledBackgroundColor;
        
        Config();
    };

    using TextChangeCallback = std::function<void(const std::string& text)>;

    UIInputBox(sf::Font& font, const Config& config = Config{});
    
    // Text management
    void setText(const std::string& text);
    const std::string& getText() const;
    void clear();
    
    void setPlaceholder(const std::string& placeholder);
    const std::string& getPlaceholder() const;
    
    // State
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setFocus(bool focus);
    bool isFocused() const;
    
    // Layout
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    void setWidth(float width);
    float getWidth() const;
    float getHeight() const;
    
    // Event handling
    void handleEvent(const sf::Event& event);
    void update(sf::Vector2f mousePos, bool mousePressed = false);
    
    // Callback
    void setOnTextChange(TextChangeCallback callback);
    
    // Rendering
    void render(sf::RenderTarget& target);

private:
    sf::Font& font_;
    Config config_;
    
    sf::Vector2f position_;
    std::string text_;
    std::string placeholder_;
    bool focused_ = false;
    bool enabled_ = true;
    
    // Cursor blinking
    sf::Clock cursorClock_;
    bool cursorVisible_ = true;
    
    // Cached renderables
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    std::unique_ptr<sf::Text> textObj_;
    std::unique_ptr<sf::Text> placeholderObj_;
    sf::RectangleShape cursor_;
    std::unique_ptr<sf::Text> searchIcon_;
    
    TextChangeCallback onTextChange_;
    
    void updateVisuals();
    void processTextInput(uint32_t unicode);
    static sf::String toUtf8(const std::string& text);
};
