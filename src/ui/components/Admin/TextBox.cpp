#include "UI/components/Admin/TextBox.h"
#include <algorithm>

TextBox::TextBox(Font& font, const string& label, float x, float y, float width, float height)
    : font(font), x(x), y(y), width(width), height(height), isFocused(false), isActive(true) {
    
    // Background
    background.setSize({width, height});
    background.setPosition({x, y});
    background.setFillColor(bgColor);
    
    // Border
    border.setSize({width, height});
    border.setPosition({x, y});
    border.setFillColor(Color::Transparent);
    border.setOutlineThickness(1.f);
    border.setOutlineColor(borderColor);
    
    // Label text
    labelText = make_unique<Text>(font);
    labelText->setString(String::fromUtf8(label.begin(), label.end()));
    labelText->setCharacterSize(14);
    labelText->setFillColor(Color(80, 80, 80));
    labelText->setPosition({x, y - 22});
    
    // Display text
    displayText = make_unique<Text>(font);
    displayText->setCharacterSize(15);
    displayText->setFillColor(textColor);
    displayText->setPosition({x + horizontalPadding, y});

    // Cursor setup (drawn manually to avoid placeholder flicker)
    cursor.setSize({1.6f, height - 16.f});
    cursor.setFillColor(focusColor);
    cursor.setPosition({x + horizontalPadding, y + (height - cursor.getSize().y) / 2});
}

void TextBox::setPlaceholder(const string& text) {
    placeholder = text;
}

void TextBox::setValue(const string& text) {
    value = text;
}

void TextBox::setPosition(Vector2f pos) {
    x = pos.x;
    y = pos.y;
    background.setPosition({x, y});
    border.setPosition({x, y});
    if (labelText) labelText->setPosition({x, y - 22});
    // displayText position is updated in render()
}

void TextBox::setFocus(bool focus) {
    isFocused = focus;
    if (isFocused) {
        border.setOutlineColor(focusColor);
        border.setOutlineThickness(2.f);
        cursorClock.restart();
    } else {
        border.setOutlineColor(borderColor);
        border.setOutlineThickness(1.f);
        cursorClock.restart();
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
            cursorClock.restart();
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
            cursorClock.restart();
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
    bool showPlaceholder = !isFocused && value.empty();
    string displayStr = showPlaceholder ? placeholder : value;
    Color displayColor = showPlaceholder ? placeholderColor : textColor;

    if (displayText) {
        displayText->setFillColor(displayColor);
        displayText->setString(String::fromUtf8(displayStr.begin(), displayStr.end()));

        // Re-center vertically inside the textbox each frame
        auto bounds = displayText->getLocalBounds();
        float baselineOffset = background.getPosition().y + (height - bounds.size.y) / 2.f - bounds.position.y;
        displayText->setPosition({background.getPosition().x + horizontalPadding, baselineOffset});

        window.draw(*displayText);
    }

    // Blink cursor without altering text or placeholder
    if (isFocused && displayText) {
        float elapsed = cursorClock.getElapsedTime().asSeconds();
        if (elapsed >= 1.f) {
            cursorClock.restart();
            elapsed = cursorClock.getElapsedTime().asSeconds();
        }

        if (elapsed < 0.5f) {
            auto bounds = displayText->getLocalBounds();
            float cursorX = displayText->getPosition().x + bounds.position.x + bounds.size.x;
            float cursorY = background.getPosition().y + (height - cursor.getSize().y) / 2.f;
            cursor.setSize({cursor.getSize().x, height - 16.f});
            cursor.setPosition({cursorX + 2.f, cursorY});
            cursor.setFillColor(focusColor);
            window.draw(cursor);
        }
    }
}
