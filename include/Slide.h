#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Slide {
private:
    Sprite sprite;
    RectangleShape buttonBox;
    Text buttonLabel;

public:
    Slide(const Texture&, const Font&);
    void setPosition(Vector2f);
    void draw(RenderWindow&);
    bool isButtonHovered(Vector2f) const;
    bool isButtonClicked(Vector2f, bool) const;
    void highlightButton(bool);
    Sprite& getPosterSprite();
    const Texture& getPosterTexture() const;
};
