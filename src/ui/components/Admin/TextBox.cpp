#include "UI/components/Admin/TextBox.h"
#include <algorithm>

TextBox::TextBox(Font& font, const string& label, float x, float y, float width, float height)
    : font(font), x(x), y(y), width(width), height(height), isFocused(false), isActive(true) {
    
    // Background
    background.setSize(Vector2f(width, height));
    background.setPosition(Vector2f(x, y));
    background.setFillColor(bgColor);
    
    // Border
    border.setSize(Vector2f(width, height));
    border.setPosition(Vector2f(x, y));
    border.setFillColor(Color::Transparent);
    border.setOutlineThickness(1.f);
    border.setOutlineColor(borderColor);
    
    // Label text
    labelText = make_unique<Text>(font);
    labelText->setString(String::fromUtf8(label.begin(), label.end()));
    labelText->setCharacterSize(14);
    labelText->setFillColor(Color(80, 80, 80));
    labelText->setPosition(Vector2f(x, y - 22));
    
    // Display text
    displayText = make_unique<Text>(font);
    displayText->setCharacterSize(15);
    displayText->setFillColor(textColor);
    displayText->setPosition(Vector2f(x + 10, y + (height - 20) / 2));
}

void TextBox::setPlaceholder(const string& text) {
    placeholder = text;
}

void TextBox::setValue(const string& text) {
    value = text;
}

void TextBox::setFocus(bool focus) {
    isFocused = focus;
    if (isFocused) {
        border.setOutlineColor(focusColor);
        border.setOutlineThickness(2.f);
    } else {
        border.setOutlineColor(borderColor);
        border.setOutlineThickness(1.f);
    }
}

void TextBox::handleEvent(const Event& event) {
    if (!isFocused) return;
    
    if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
        if (textEvent->unicode == 8) { // Backspace
            if (!value.empty()) {
                // Handle UTF-8 properly
                size_t len = value.length();
                while (len > 0 && (value[len-1] & 0xC0) == 0x80) {
                    len--;
                }
                if (len > 0) len--;
                value = value.substr(0, len);
            }
        } else if (textEvent->unicode == 13) { // Enter
            // Do nothing, handled by parent
        } else if (textEvent->unicode == 27) { // Escape
            setFocus(false);
        } else if (textEvent->unicode >= 32) {
            // Add character (including Vietnamese)
            char32_t unicode = textEvent->unicode;
            if (unicode < 0x80) {
                value += static_cast<char>(unicode);
            } else if (unicode < 0x800) {
                value += static_cast<char>(0xC0 | (unicode >> 6));
                value += static_cast<char>(0x80 | (unicode & 0x3F));
            } else if (unicode < 0x10000) {
                value += static_cast<char>(0xE0 | (unicode >> 12));
                value += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
                value += static_cast<char>(0x80 | (unicode & 0x3F));
            } else {
                value += static_cast<char>(0xF0 | (unicode >> 18));
                value += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
                value += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
                value += static_cast<char>(0x80 | (unicode & 0x3F));
            }
        }
    }
}

void TextBox::update(Vector2f mousePos, bool mousePressed) {
    if (mousePressed) {
        FloatRect bounds = background.getGlobalBounds();
        if (bounds.contains(mousePos)) {
            setFocus(true);
        } else {
            setFocus(false);
        }
    }
}

void TextBox::render(RenderWindow& window) {
    window.draw(background);
    window.draw(border);
    if (labelText) window.draw(*labelText);
    
    // Display text or placeholder
    string displayStr = value.empty() ? placeholder : value;
    Color displayColor = value.empty() ? placeholderColor : textColor;
    
    // Add cursor if focused
    if (isFocused && cursorClock.getElapsedTime().asSeconds() < 0.5f) {
        displayStr = value + "|";
        if (cursorClock.getElapsedTime().asSeconds() >= 1.0f) {
            cursorClock.restart();
        }
    } else if (cursorClock.getElapsedTime().asSeconds() >= 1.0f) {
        cursorClock.restart();
    }
    
    if (displayText) {
        displayText->setString(String::fromUtf8(displayStr.begin(), displayStr.end()));
        displayText->setFillColor(displayColor);
        window.draw(*displayText);
    }
}
