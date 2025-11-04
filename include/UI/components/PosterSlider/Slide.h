#pragma once
#include <SFML/Graphics.hpp>
#include "UI/components/Button.h"
using namespace sf;

class Slide {
private:
    Sprite poster_sprite;
    Button detail_button;

public:
    Slide(const Texture&, const Font&);
    void setPosition(Vector2f);
    void update(Vector2f, bool);
    void draw(RenderWindow&);
    bool isDetailButtonClicked(Vector2f, bool);
    Sprite& getPosterSprite();
    const Texture& getPosterTexture() const;
};
