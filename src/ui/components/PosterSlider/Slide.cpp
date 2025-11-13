#include "UI/components/PosterSlider/Slide.h"
using namespace std;

Slide::Slide(const Texture& texture, const Font& font) 
    :   detail_button(font, L"Xem chi tiết", 150.f, 40.f, 18),
        poster_sprite(texture)
{
    poster_sprite.setScale({0.32f, 0.32f});
}

void Slide::setPosition(Vector2f pos) {
    poster_sprite.setPosition(pos);
    FloatRect bounds = poster_sprite.getGlobalBounds();
    Vector2f detailButton_pos = {
        bounds.position.x + bounds.size.x / 2.f - detail_button.getSize().x / 2.f,
        bounds.position.y + bounds.size.y + 25
    };
    detail_button.setPosition(detailButton_pos);
}

void Slide::draw(RenderWindow& window) {
    window.draw(poster_sprite);
    detail_button.draw(window);
}

void Slide::update(Vector2f mousePos, bool mousePressed) {
    detail_button.update(mousePos, mousePressed, Color(30, 41, 202), Color::White);
    detail_button.setTextColor(detail_button.isHovered() ? Color::White : Color::Black);
}

bool Slide::isDetailButtonClicked(Vector2f mousePos, bool mousePressed) {
    return detail_button.getGlobalBounds().contains(mousePos) && mousePressed;
}

Sprite& Slide::getPosterSprite() {
    return poster_sprite;
}
    
const Texture& Slide::getPosterTexture() const { 
    return poster_sprite.getTexture(); 
}