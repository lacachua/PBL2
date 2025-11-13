#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>

using namespace sf;
using namespace std;

/**
 * @brief TextBox component for input fields in popups
 */
class TextBox {
private:
    Font& font;
    float x, y, width, height;
    
    RectangleShape background;
    RectangleShape border;
    unique_ptr<Text> displayText;
    unique_ptr<Text> labelText;
    RectangleShape cursor;
    
    string value;
    string placeholder;
    bool isFocused;
    bool isActive;
    
    Clock cursorClock;
    
    Color bgColor = Color(255, 255, 255);
    Color borderColor = Color(201, 206, 214);  // #C9CED6
    Color focusColor = Color(20, 118, 172);    // #1476AC
    Color textColor = Color(34, 34, 34);
    Color placeholderColor = Color(160, 160, 160);
    float horizontalPadding = 10.f;
    
public:
    TextBox(Font& font, const string& label, float x, float y, float width, float height);
    
    void setPlaceholder(const string& text);
    void setValue(const string& text);
    string getValue() const { return value; }
    
    void handleEvent(const Event& event);
    void update(Vector2f mousePos, bool mousePressed);
    void render(RenderWindow& window);
    
    void setFocus(bool focus);
    bool getFocus() const { return isFocused; }
};

#endif
