#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class Button {
private:
    RectangleShape box;
    Text text;
    Color normalColor;
    Color hoverColor;
    Color disabledColor;
    bool isHovered;
    bool isDisabled;
    bool isSelected;

public:
    Button(const Font& font, const wstring& label, float width, float height, 
           unsigned int charSize = 20);
    
    // ✅ Setters
    void setPosition(float x, float y);
    void setPosition(Vector2f pos);
    void setText(const wstring& label); // ✅ Thêm setText
    void setNormalColor(const Color& color);
    void setHoverColor(const Color& color);
    void setDisabledColor(const Color& color);
    void setOutlineThickness(float thickness);
    void setOutlineColor(const Color& color);
    void setTextFillColor(const Color& color);
    void setTextPosition(float offsetX, float offsetY);
    void setDisabled(bool disabled);
    void setSelected(bool selected);
    
    // ✅ Getters
    Vector2f getPosition() const;
    FloatRect getGlobalBounds() const;
    bool getHovered() const { return isHovered; }
    bool getDisabled() const { return isDisabled; }
    bool getSelected() const { return isSelected; }
    
    // ✅ Interactions
    void update(Vector2f mousePos);
    void draw(RenderWindow& window);
    bool isClicked(Vector2f mousePos, bool mousePressed) const;
};
