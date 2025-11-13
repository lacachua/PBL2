#include "UI/components/Admin/DropdownBox.h"
#include <iostream>

DropdownBox::DropdownBox(Font& font, const string& label, float x, float y, float width, float height)
    : font(font), x(x), y(y), width(width), height(height), label(label),
      selectedIndex(0), isOpen(false),
      bgColor(255, 255, 255), hoverColor(230, 230, 230), 
      selectedColor(100, 149, 237), borderColor(200, 200, 200),
      textColor(50, 50, 50) {
    
    // Label text
    labelText = make_unique<Text>(font, String::fromUtf8(label.begin(), label.end()), 14);
    labelText->setFillColor(Color(80, 80, 80));
    labelText->setPosition(Vector2f(x, y - 22));
    
    // Main background
    background.setSize(Vector2f(width, height));
    background.setPosition(Vector2f(x, y));
    background.setFillColor(bgColor);
    background.setOutlineThickness(1.f);
    background.setOutlineColor(borderColor);
    
    // Selected text (shown in main box)
    selectedText = make_unique<Text>(font, "", 16);
    selectedText->setFillColor(textColor);
    selectedText->setPosition(Vector2f(x + 10, y + (height - 20) / 2));
    
    // Arrow (small triangle)
    arrow.setSize(Vector2f(8, 8));
    arrow.setFillColor(Color(100, 100, 100));
    arrow.setPosition(Vector2f(x + width - 20, y + height / 2 - 4));
}

void DropdownBox::setOptions(const vector<string>& opts) {
    options = opts;
    optionTexts.clear();
    optionBackgrounds.clear();
    
    // Create dropdown panel
    float panelHeight = opts.size() * 40.f;
    dropdownPanel.setSize(Vector2f(width, panelHeight));
    dropdownPanel.setPosition(Vector2f(x, y + height));
    dropdownPanel.setFillColor(Color::White);
    dropdownPanel.setOutlineThickness(1.f);
    dropdownPanel.setOutlineColor(borderColor);
    
    // Create option items
    for (size_t i = 0; i < opts.size(); i++) {
        // Background
        RectangleShape optBg;
        optBg.setSize(Vector2f(width, 40.f));
        optBg.setPosition(Vector2f(x, y + height + i * 40.f));
        optBg.setFillColor(bgColor);
        optionBackgrounds.push_back(optBg);
        
        // Text
        auto text = make_unique<Text>(font, String::fromUtf8(opts[i].begin(), opts[i].end()), 16);
        text->setFillColor(textColor);
        text->setPosition(Vector2f(x + 10, y + height + i * 40.f + 10));
        optionTexts.push_back(move(text));
    }
    
    // Set initial selected text
    if (!options.empty()) {
        selectedText->setString(String::fromUtf8(options[0].begin(), options[0].end()));
    }
}

void DropdownBox::setSelectedIndex(int index) {
    if (index >= 0 && index < (int)options.size()) {
        selectedIndex = index;
        selectedText->setString(String::fromUtf8(options[index].begin(), options[index].end()));
    }
}

void DropdownBox::setSelectedValue(const string& value) {
    for (size_t i = 0; i < options.size(); i++) {
        if (options[i] == value) {
            setSelectedIndex(i);
            break;
        }
    }
}

string DropdownBox::getSelectedValue() const {
    if (selectedIndex >= 0 && selectedIndex < (int)options.size()) {
        return options[selectedIndex];
    }
    return "";
}

int DropdownBox::getSelectedIndex() const {
    return selectedIndex;
}

bool DropdownBox::isMouseOver(const Vector2f& mousePos) const {
    FloatRect bounds({x, y}, {width, height});
    return bounds.contains(mousePos);
}

void DropdownBox::handleEvent(const Event& event, const Vector2f& mousePos) {
    if (auto* mouseButtonPressed = event.getIf<Event::MouseButtonPressed>()) {
        if (mouseButtonPressed->button == Mouse::Button::Left) {
            // Click on main box - toggle dropdown
            if (isMouseOver(mousePos)) {
                isOpen = !isOpen;
                return;
            }
            
            // Click on option in dropdown
            if (isOpen) {
                for (size_t i = 0; i < optionBackgrounds.size(); i++) {
                    if (optionBackgrounds[i].getGlobalBounds().contains(mousePos)) {
                        setSelectedIndex(i);
                        isOpen = false;
                        return;
                    }
                }
                // Click outside - close
                isOpen = false;
            }
        }
    }
}

void DropdownBox::update() {
    // Update hover effects for dropdown options
    if (isOpen) {
        Vector2i mousePixelPos = Mouse::getPosition();
        // Note: This needs window reference to be accurate, simplified for now
    }
}

void DropdownBox::draw(RenderWindow& window) {
    // Draw label
    if (labelText) {
        window.draw(*labelText);
    }
    
    // Draw main box
    window.draw(background);
    
    // Draw selected text
    if (selectedText) {
        window.draw(*selectedText);
    }
    
    // Draw arrow
    window.draw(arrow);
    
    // Draw dropdown panel if open
    if (isOpen) {
        window.draw(dropdownPanel);
        
        // Draw options
        for (size_t i = 0; i < optionBackgrounds.size(); i++) {
            // Highlight hovered option
            Vector2i mousePos = Mouse::getPosition(window);
            Vector2f worldPos = window.mapPixelToCoords(mousePos);
            
            if (optionBackgrounds[i].getGlobalBounds().contains(worldPos)) {
                optionBackgrounds[i].setFillColor(hoverColor);
            } else if ((int)i == selectedIndex) {
                optionBackgrounds[i].setFillColor(Color(220, 235, 255)); // Light blue for selected
            } else {
                optionBackgrounds[i].setFillColor(bgColor);
            }
            
            window.draw(optionBackgrounds[i]);
            
            if (optionTexts[i]) {
                window.draw(*optionTexts[i]);
            }
        }
    }
}
