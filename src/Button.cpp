#include "Button.h"

Button::Button(const Font& font, const wstring& label, float width, float height, unsigned int charSize)
    : text(font, label, charSize), isHovered(false), isDisabled(false), isSelected(false) {
    
    box.setSize({width, height});
    
    // ✅ Default colors
    normalColor = Color(60, 60, 70);
    hoverColor = Color(100, 100, 120);
    disabledColor = Color(40, 40, 45);
    
    box.setFillColor(normalColor);
    box.setOutlineThickness(2.f);
    box.setOutlineColor(Color(100, 100, 110));
    
    text.setFillColor(Color::White);
}

void Button::setPosition(float x, float y) {
    setPosition({x, y});
}

void Button::setPosition(Vector2f pos) {
    box.setPosition(pos);
    
    // Center text in box - Căn giữa chính xác
    FloatRect textBounds = text.getLocalBounds();
    float textX = pos.x + (box.getSize().x - textBounds.size.x) / 2.f - textBounds.position.x;
    float textY = pos.y + (box.getSize().y - textBounds.size.y) / 2.f - textBounds.position.y;
    text.setPosition({textX, textY});
}

void Button::setText(const wstring& label) {
    text.setString(label);
    // Re-center text sau khi đổi nội dung
    Vector2f boxPos = box.getPosition();
    FloatRect textBounds = text.getLocalBounds();
    float textX = boxPos.x + (box.getSize().x - textBounds.size.x) / 2.f - textBounds.position.x;
    float textY = boxPos.y + (box.getSize().y - textBounds.size.y) / 2.f - textBounds.position.y;
    text.setPosition({textX, textY});
}

void Button::setNormalColor(const Color& color) {
    normalColor = color;
    if (!isHovered && !isDisabled) {
        box.setFillColor(normalColor);
    }
}

void Button::setHoverColor(const Color& color) {
    hoverColor = color;
}

void Button::setDisabledColor(const Color& color) {
    disabledColor = color;
    if (isDisabled) {
        box.setFillColor(disabledColor);
    }
}

void Button::setOutlineThickness(float thickness) {
    box.setOutlineThickness(thickness);
}

void Button::setOutlineColor(const Color& color) {
    box.setOutlineColor(color);
}

void Button::setTextFillColor(const Color& color) {
    text.setFillColor(color);
}

void Button::setTextPosition(float offsetX, float offsetY) {
    Vector2f boxPos = box.getPosition();
    text.setPosition({boxPos.x + offsetX, boxPos.y + offsetY});
}

void Button::setDisabled(bool disabled) {
    isDisabled = disabled;
    if (isDisabled) {
        box.setFillColor(disabledColor);
        text.setFillColor(Color(100, 100, 100));
    } else {
        box.setFillColor(isHovered ? hoverColor : normalColor);
        text.setFillColor(Color::White);
    }
}

void Button::setSelected(bool selected) {
    isSelected = selected;
    if (isSelected) {
        box.setFillColor(Color(52, 62, 209)); // Selected color
    } else {
        box.setFillColor(isHovered && !isDisabled ? hoverColor : normalColor);
    }
}

Vector2f Button::getPosition() const {
    return box.getPosition();
}

FloatRect Button::getGlobalBounds() const {
    return box.getGlobalBounds();
}

void Button::update(Vector2f mousePos) {
    if (isDisabled) return;
    
    isHovered = box.getGlobalBounds().contains(mousePos);
    
    if (isSelected) {
        box.setFillColor(Color(52, 62, 209));
    } else if (isHovered) {
        box.setFillColor(hoverColor);
    } else {
        box.setFillColor(normalColor);
    }
}

void Button::draw(RenderWindow& window) {
    window.draw(box);
    window.draw(text);
}

bool Button::isClicked(Vector2f mousePos, bool mousePressed) const {
    return !isDisabled && mousePressed && box.getGlobalBounds().contains(mousePos);
}
