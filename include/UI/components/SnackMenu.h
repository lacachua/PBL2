#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ui/components/Button.h"

using namespace sf;
using namespace std;

struct SnackItem {
    string name;
    int price;
    string imagePath;
    Texture texture;
    int quantity;
    
    SnackItem(const string& n, int p, const string& img);
    void draw(RenderWindow& window, float x, float y, float size);
};

class SnackMenu {
private:
    vector<SnackItem> items;
    vector<Button> plusButtons;
    vector<Button> minusButtons;
    Font& font;
    
public:
    SnackMenu(Font& f);
    
    void initialize();
    void handleClick(const Vector2f& mousePos);
    void draw(RenderWindow& window, const FloatRect& contentArea);
    void updateButtons(const Vector2f& mousePos);
    
    int getTotalPrice() const;
    void reset();
    
    const vector<SnackItem>& getItems() const { return items; }
};