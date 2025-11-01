#include "ui/components/SnackMenu.h"
#include <locale>
#include <codecvt>

SnackItem::SnackItem(const string& n, int p, const string& img) 
    : name(n), price(p), imagePath(img), quantity(0) {}

void SnackItem::draw(RenderWindow& window, float x, float y, float size) {
    Sprite sprite(texture);
    sprite.setPosition({x, y});
    float scaleX = size / texture.getSize().x;
    float scaleY = size / texture.getSize().y;
    sprite.setScale({scaleX, scaleY});
    window.draw(sprite);
}

SnackMenu::SnackMenu(Font& f) : font(f) {}

void SnackMenu::initialize() {
    items.clear();
    plusButtons.clear();
    minusButtons.clear();
    
    items.emplace_back("Combo 1", 65000, "../assets/elements/combo1.png");
    items.emplace_back("Combo 2", 105000, "../assets/elements/combo2.png");
    items.emplace_back("Bắp & Nước", 55000, "../assets/elements/pop_and_drink.png");
    items.emplace_back("Bắp ngô", 35000, "../assets/elements/one_pop.png");
    items.emplace_back("Nước uống", 25000, "../assets/elements/one_drink.png");
    
    for (auto& item : items) {
        if (!item.texture.loadFromFile(item.imagePath)) {
            // Handle error
        }
    }
    
    for (size_t i = 0; i < items.size(); i++) {
        minusButtons.emplace_back(font, L"-", 40.f, 40.f, 32);
        minusButtons.back().setNormalColor(Color(150, 52, 52));
        minusButtons.back().setHoverColor(Color(180, 70, 70));
        minusButtons.back().setDisabledColor(Color(60, 60, 60));
        minusButtons.back().setOutlineThickness(2.f);
        
        plusButtons.emplace_back(font, L"+", 40.f, 40.f, 32);
        plusButtons.back().setNormalColor(Color(52, 150, 52));
        plusButtons.back().setHoverColor(Color(70, 180, 70));
        plusButtons.back().setOutlineThickness(2.f);
    }
}

void SnackMenu::handleClick(const Vector2f& mousePos) {
    for (size_t i = 0; i < items.size(); i++) {
        if (plusButtons[i].isClicked(mousePos, true)) {
            items[i].quantity++;
            return;
        }
        
        if (minusButtons[i].isClicked(mousePos, true) && !minusButtons[i].getDisabled()) {
            if (items[i].quantity > 0) {
                items[i].quantity--;
            }
            return;
        }
    }
}

void SnackMenu::draw(RenderWindow& window, const FloatRect& contentArea) {
    Text title(font, L"CHỌN ĐỒ ĂN & THỨC UỐNG", 32);
    title.setPosition({contentArea.position.x + 30.f, contentArea.position.y + 30.f});
    title.setFillColor(Color::White);
    window.draw(title);
    
    float startX = contentArea.position.x + 30.f;
    float startY = contentArea.position.y + 90.f;
    float itemWidth = 180.f;
    float itemHeight = 220.f;
    float spacing = 20.f;
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    Font detailFont("../assets/quicksand_medium.ttf");
    
    for (size_t i = 0; i < items.size(); i++) {
        float x = startX + (i % 5) * (itemWidth + spacing);
        float y = startY + (i / 5) * (itemHeight + spacing);
        
        float imageSize = 120.f;
        items[i].draw(window, x, y, imageSize);
        
        Text nameText(detailFont, conv.from_bytes(items[i].name), 16);
        nameText.setPosition({x, y + imageSize + 5});
        nameText.setFillColor(Color::White);
        window.draw(nameText);
        
        char priceStr[32];
        snprintf(priceStr, sizeof(priceStr), "%d VNĐ", items[i].price);
        Text priceText(detailFont, conv.from_bytes(priceStr), 14);
        priceText.setPosition({x, y + imageSize + 28});
        priceText.setFillColor(Color(255, 200, 100));
        window.draw(priceText);
        
        minusButtons[i].setPosition(x, y + imageSize + 55);
        minusButtons[i].setDisabled(items[i].quantity == 0);
        minusButtons[i].draw(window);
        
        char qtyStr[8];
        snprintf(qtyStr, sizeof(qtyStr), "%d", items[i].quantity);
        Text qtyText(font, conv.from_bytes(qtyStr), 24);
        
        FloatRect textBounds = qtyText.getLocalBounds();
        float centerX = x + 65;
        qtyText.setPosition({
            centerX - textBounds.size.x / 2.f - textBounds.position.x,
            y + imageSize + 60
        });
        qtyText.setFillColor(Color::White);
        window.draw(qtyText);
        
        plusButtons[i].setPosition(x + 90, y + imageSize + 55);
        plusButtons[i].draw(window);
    }
    
    int totalSnackPrice = getTotalPrice();
    
    RectangleShape separator({900.f, 2.f});
    separator.setPosition({startX, startY + itemHeight + 80});
    separator.setFillColor(Color(100, 100, 100));
    window.draw(separator);
    
    Text totalLabel(font, L"TỔNG TIỀN ĐỒ ĂN:", 36);
    totalLabel.setPosition({startX, startY + itemHeight + 100});
    totalLabel.setFillColor(Color::White);
    window.draw(totalLabel);
    
    char totalStr[32];
    snprintf(totalStr, sizeof(totalStr), "%d VNĐ", totalSnackPrice);
    Text totalText(font, conv.from_bytes(totalStr), 36);
    totalText.setPosition({startX + 300, startY + itemHeight + 100});
    totalText.setFillColor(Color(255, 200, 100));
    window.draw(totalText);
}

void SnackMenu::updateButtons(const Vector2f& mousePos) {
    for (auto& btn : plusButtons) btn.update(mousePos);
    for (auto& btn : minusButtons) btn.update(mousePos);
}

int SnackMenu::getTotalPrice() const {
    int total = 0;
    for (const auto& item : items) {
        total += item.price * item.quantity;
    }
    return total;
}

void SnackMenu::reset() {
    for (auto& item : items) {
        item.quantity = 0;
    }
}