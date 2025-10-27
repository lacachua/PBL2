#include "Slide.h"
using namespace std;

Slide::Slide(const Texture& texture, const Font& font) : 
buttonLabel(font, L"Xem chi tiết", 22),
sprite(texture)
{
    sprite.setScale({0.36, 0.36});

    buttonBox.setSize({200, 45});
    buttonBox.setFillColor(Color(255, 255, 255, 220));
    buttonBox.setOutlineThickness(2.f);
    buttonBox.setOutlineColor(Color(200, 200, 200));
}

void Slide::setPosition(Vector2f pos) {
    sprite.setPosition(pos);

    FloatRect bounds = sprite.getGlobalBounds();

    Vector2f buttonPos = {
        bounds.position.x + bounds.size.x / 2 - buttonBox.getSize().x / 2,
        bounds.position.y + bounds.size.y + 25 
    };
    buttonBox.setPosition(buttonPos);

    FloatRect txt = buttonLabel.getGlobalBounds();
    buttonLabel.setPosition({
        buttonPos.x + (buttonBox.getSize().x - txt.size.x) / 2,
        buttonPos.y + (buttonBox.getSize().y - txt.size.y) / 2 - 5
    }); 
}

void Slide::draw(RenderWindow& window) {
    window.draw(sprite);
    window.draw(buttonBox);
    window.draw(buttonLabel);
}

bool Slide::isButtonHovered(Vector2f mousePos) const {
    return buttonBox.getGlobalBounds().contains(mousePos);
}

bool Slide::isButtonClicked(Vector2f mousePos, bool mousePressed) const {
    return mousePressed && isButtonHovered(mousePos);
}

void Slide::highlightButton(bool hovered) {
    if (hovered) {
        buttonBox.setFillColor(Color(52, 62, 209, 255));
        buttonLabel.setFillColor(Color::White);
    } else {
        buttonBox.setFillColor(Color(255, 255, 255, 235));
        buttonLabel.setFillColor(Color::Black);
    }
}

Sprite& Slide::getPosterSprite() { 
    return sprite;
}
    
const Texture& Slide::getPosterTexture() const { 
    return sprite.getTexture(); 
}