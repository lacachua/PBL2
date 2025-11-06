#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Button {
    private:
        const Font& font;
        RectangleShape box;
        Text text;
        bool hovered = false;
        bool pressed = false;
    public:
        Button(const Font&, const String&, float, float, int);

        void setPosition(Vector2f);
        void setText(const String&);
        void setFillColor(Color);
        void setTextColor(Color);
        void setOutlineThickness(float);
        void setOutlineColor(Color);

        Vector2f getPosition() const { return box.getPosition(); }
        Vector2f getSize() const { return box.getSize(); }
        FloatRect getGlobalBounds() const { return box.getGlobalBounds(); }
        String getString() const { return text.getString(); }
        bool isHovered() const { return hovered; }
        bool isPressed() const { return pressed; }

        void update(Vector2f, bool, Color, Color);
        bool isClicked(Vector2f, bool);
        void draw(RenderWindow&);
};
