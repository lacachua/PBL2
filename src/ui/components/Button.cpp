#include "UI/components/Button.h"

Button::Button(const Font& font, const String& label, float width, float height, int text_size) 
    :   font(font), box({width, height}), text(font)
{
    box.setOutlineThickness(2.f);
    box.setOutlineColor(Color(136, 136, 136));

    text.setString(label);
    text.setFillColor(Color::Black);
    text.setCharacterSize(text_size);
}

void Button::setPosition(Vector2f pos) {
    box.setPosition(pos);
    FloatRect bounds = text.getLocalBounds();
    text.setPosition({
        pos.x + (box.getSize().x - bounds.size.x) / 2.f - bounds.position.x,
        pos.y + (box.getSize().y - bounds.size.y) / 2.f - bounds.position.y,
    });
}

void Button::setText(const String& label) {
    text.setString(label);
    setPosition(box.getPosition());
}

void Button::setFillColor(Color color) {
    box.setFillColor(color);
}

void Button::setTextColor(Color color) {
    text.setFillColor(color);
}

void Button::setOutlineThickness(float thickness) {
    box.setOutlineThickness(thickness);
}


void Button::update(Vector2f mousePos, bool mousePressed) {
    hovered = box.getGlobalBounds().contains(mousePos);

    if (hovered) {
        box.setFillColor(Color(30, 41, 202));
        text.setFillColor(Color::White);
    }
    else {
        box.setFillColor(Color::White);
        text.setFillColor(Color::Black);
    }
}

bool Button::isClicked(Vector2f mousePos, bool mousePressed) {
    return box.getGlobalBounds().contains(mousePos) && mousePressed;
}

void Button::draw(RenderWindow& window) {
    window.draw(box);
    window.draw(text);
}
