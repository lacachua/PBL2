#pragma once
#include <SFML/Graphics.hpp>
#include "repositories/booking/ComboRepository.h"
#include "data-structures/DLL.h"
using namespace sf;
using namespace std;

struct ComboItem {
    Combo combo;
    int quantity = 0;
    RectangleShape minusBtn, plusBtn;
};

struct ComboOrder {
    String id;
    String name;
    int quantity;
    int price;
};

class ComboSelection {
private:
    Font& font;
    DLL<ComboItem> comboItems;
    float viewX = 174.f, viewY = 220.f;

public:
    ComboSelection(Font& f, const DLL<Combo>& combos);

    void handleClick(Vector2f mousePos, bool mousePressed);
    void draw(RenderWindow& window);
    int  getTotalPrice() const;
    bool hasSelectedCombos() const;

    DLL<ComboOrder> getSelectedCombos() const;

    void reset(); // reset số lượng về 0
};
