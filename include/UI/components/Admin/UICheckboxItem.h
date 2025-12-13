#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>

/**
 * @brief Reusable Checkbox Item Component
 * 
 * Features:
 * - Self-managed checked/unchecked state
 * - Hover and click visual feedback
 * - Customizable colors and sizes
 * - Callback on state change
 * 
 * Usage:
 *   UICheckboxItem checkbox(font, "Option 1", 200.f);
 *   checkbox.setPosition({100, 100});
 *   checkbox.setOnToggle([](bool checked) { ... });
 */
class UICheckboxItem {
public:
    struct Config {
        float width;
        float height;
        float checkboxSize;
        float padding;
        unsigned int fontSize;
        
        sf::Color bgColor;
        sf::Color bgHoverColor;
        sf::Color bgSelectedColor;
        sf::Color checkboxColor;
        sf::Color checkboxCheckedColor;
        sf::Color borderColor;
        sf::Color textColor;
        sf::Color secondaryTextColor;
        
        Config();
    };

    explicit UICheckboxItem(sf::Font& font, const Config& config = Config{});
    
    // Data setters
    void setPrimaryText(const std::string& text);
    void setSecondaryText(const std::string& text);
    void setData(const std::string& primary, const std::string& secondary = "");
    
    // State management
    void setChecked(bool checked);
    bool isChecked() const;
    void toggle();
    
    // Position & Layout
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    float getHeight() const;
    
    // Interaction
    bool handleClick(sf::Vector2f mousePos);  // Returns true if state changed
    void updateHover(sf::Vector2f mousePos);
    bool isHovered() const;
    
    // Callback
    using ToggleCallback = std::function<void(bool checked)>;
    void setOnToggle(ToggleCallback callback);
    
    // Rendering
    void render(sf::RenderTarget& target);
    
    // Visual state for external styling (e.g., alternating rows)
    void setAlternateBackground(bool alternate);

private:
    sf::Font& font_;
    Config config_;
    
    sf::Vector2f position_;
    bool checked_ = false;
    bool hovered_ = false;
    bool alternateBackground_ = false;
    
    std::string primaryText_;
    std::string secondaryText_;
    
    // Cached renderables
    sf::RectangleShape background_;
    sf::RectangleShape checkbox_;
    std::unique_ptr<sf::Text> primaryTextObj_;
    std::unique_ptr<sf::Text> secondaryTextObj_;
    std::unique_ptr<sf::Text> checkmark_;
    
    ToggleCallback onToggle_;
    
    void updateVisuals();
    static sf::String toUtf8(const std::string& text);
};
