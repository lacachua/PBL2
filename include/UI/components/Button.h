#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
using namespace sf;
using namespace std;

class Button {
    private:
        const Font& font;
        RectangleShape box;
        unique_ptr<Text> text;
        bool hovered = false;
        bool pressed = false;
        
        Color normalColor = Color(200, 200, 200);
        Color hoverColor = Color(180, 180, 180);
        
    public:
        Button(const Font&, const String&, float, float, int);
        Button(const Font&, const String&, Vector2f size, int textSize);
        
        Button(const Button&);
        Button& operator=(const Button&);
        Button(Button&&) = default;
        Button& operator=(Button&&) = default;

        void setPosition(Vector2f);
        void setText(const String&);
        void setFillColor(Color);
        void setTextColor(Color);
        void setOutlineThickness(float);
        void setOutlineColor(Color);
        void setColors(Color normal, Color hover, Color textColor);

        Vector2f getPosition() const { return box.getPosition(); }
        Vector2f getSize() const { return box.getSize(); }
        FloatRect getGlobalBounds() const { return box.getGlobalBounds(); }
        String getString() const { return text ? text->getString() : ""; }
        bool isHovered() const { return hovered; }
        bool isPressed() const { return pressed; }

        void update(Vector2f mousePos, bool mousePressed, Color hover, Color normal);
        void update(Vector2f mousePos);
        bool isClicked(Vector2f mousePos, bool mousePressed);
        bool isClicked(Vector2f mousePos);
        void draw(RenderWindow&);
};
