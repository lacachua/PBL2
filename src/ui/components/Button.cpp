#include "UI/components/Button.h"

Button::Button(const Font& font, const String& label, float width, float height, int text_size) 
    :   font(font), box({width, height})
{
    box.setOutlineThickness(2.f);
    box.setOutlineColor(Color(136, 136, 136));

    text = make_unique<Text>(font);
    text->setString(label);
    text->setFillColor(Color::Black);
    text->setCharacterSize(text_size);
}

// Copy constructor - deep copy
Button::Button(const Button& other) 
    : font(other.font), box(other.box), hovered(other.hovered), pressed(other.pressed)
{
    if (other.text) {
        text = make_unique<Text>(font);
        text->setString(other.text->getString());
        text->setFillColor(other.text->getFillColor());
        text->setCharacterSize(other.text->getCharacterSize());
        text->setPosition(other.text->getPosition());
    }
}

// Copy assignment
Button& Button::operator=(const Button& other) {
    if (this != &other) {
        box = other.box;
        hovered = other.hovered;
        pressed = other.pressed;
        
        if (other.text) {
            text = make_unique<Text>(font);
            text->setString(other.text->getString());
            text->setFillColor(other.text->getFillColor());
            text->setCharacterSize(other.text->getCharacterSize());
            text->setPosition(other.text->getPosition());
        } else {
            text.reset();
        }
    }
    return *this;
}

void Button::setPosition(Vector2f pos) {
    box.setPosition(pos);
    if (text) {
        FloatRect bounds = text->getLocalBounds();
        text->setPosition({
            pos.x + (box.getSize().x - bounds.size.x) / 2.f - bounds.position.x,
            pos.y + (box.getSize().y - bounds.size.y) / 2.f - bounds.position.y,
        });
    }
}

void Button::setText(const String& label) {
    if (text) {
        text->setString(label);
        setPosition(box.getPosition());
    }
}

void Button::setFillColor(Color color) {
    box.setFillColor(color);
}

void Button::setTextColor(Color color) {
    if (text) text->setFillColor(color);
}

void Button::setOutlineThickness(float thickness) {
    box.setOutlineThickness(thickness);
}

void Button::setOutlineColor(Color color) {
    box.setOutlineColor(color);
}

void Button::update(Vector2f mousePos, bool mousePressed, Color hover, Color normal) {
    hovered = box.getGlobalBounds().contains(mousePos);

    if (hovered) {
        box.setFillColor(hover);
        // Keep existing text color, don't override it
    }
    else {
        box.setFillColor(normal);
        // Keep existing text color, don't override it
    }
}

bool Button::isClicked(Vector2f mousePos, bool mousePressed) {
    return box.getGlobalBounds().contains(mousePos) && mousePressed;
}

void Button::draw(RenderWindow& window) {
    window.draw(box);
    if (text) window.draw(*text);
}
