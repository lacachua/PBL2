#pragma once

#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class StatCard {
private:
    Font& font;
    RectangleShape box;
    Text titleText;
    Text valueText;
    Text subtitleText;

    void centerValue();

public:
    StatCard(Font& font, const Vector2f& size, const Color& outlineColor);

    void setPosition(const Vector2f& position);
    void setSize(const Vector2f& size);
    void setOutlineThickness(float);
    void setOutlineColor(const Color&);

    void setTitle(const string& title);
    void setValue(const string& value);
    void setSubtitle(const string& subtitle);

    Vector2f getPosition() const { return box.getPosition(); }
    Vector2f getSize() const { return box.getSize(); }

    void render(RenderTarget& target) const;
};
