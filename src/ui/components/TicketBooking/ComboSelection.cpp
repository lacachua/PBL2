#include "UI/components/TicketBooking/ComboSelection.h"
#include <sstream>

static string fmtMoney(int v) {
    string s = to_string(v), out;
    for (int i = 0; i < (int)s.size(); ++i) {
        out += s[i];
        if ((int)s.size() - i - 1 > 0 && ((int)s.size() - i - 1) % 3 == 0) out += ".";
    }
    out += " VND";
    return out;
}

ComboSelection::ComboSelection(Font& f, const DLL<Combo>& combos) : font(f) {
    comboItems.clear();
    float y = viewY + 70.f;

    for (int i = 0; i < combos.getSize(); ++i) {
        ComboItem item;
        item.combo = combos[i];
        item.quantity = 0;

        // Nút -
        item.minusBtn.setSize({25.f, 25.f});
        item.minusBtn.setPosition({viewX + 670.f, y});
        item.minusBtn.setFillColor(Color(80, 80, 90));
        item.minusBtn.setOutlineColor(Color::White);
        item.minusBtn.setOutlineThickness(1.f);

        // Nút +
        item.plusBtn.setSize({25.f, 25.f});
        item.plusBtn.setPosition({viewX + 770.f, y});
        item.plusBtn.setFillColor(Color(80, 80, 90));
        item.plusBtn.setOutlineColor(Color::White);
        item.plusBtn.setOutlineThickness(1.f);

        comboItems.push_back(item);
        y += 50.f;
    }
}

void ComboSelection::handleClick(Vector2f mousePos, bool mousePressed) {
    if (!mousePressed) return;
    for (int i = 0; i < comboItems.getSize(); ++i) {
        auto& item = comboItems[i];
        if (item.minusBtn.getGlobalBounds().contains(mousePos)) {
            if (item.quantity > 0) item.quantity--;
        }
        if (item.plusBtn.getGlobalBounds().contains(mousePos)) {
            item.quantity++;
        }
    }
}

void ComboSelection::draw(RenderWindow& window) {
    Text header(font, L"CHỌN COMBO BẮP NƯỚC", 28);
    header.setFillColor(Color::White);
    header.setPosition({viewX + 20.f, viewY + 20.f});
    header.setOutlineThickness(2.f);
    header.setOutlineColor(Color(20, 118, 172));
    window.draw(header);

    float y = viewY + 70.f;
    for (int i = 0; i < comboItems.getSize(); ++i) {
        auto& item = comboItems[i];

        // Tên combo
        Text name(font, item.combo.name, 20);
        name.setFillColor(Color::White);
        name.setPosition({viewX + 40.f, y});
        window.draw(name);

        // Giá
        Text price(font, String(fmtMoney(item.combo.price)), 18);
        price.setFillColor(Color(200, 200, 200));
        price.setPosition({viewX + 480.f, y + 2.f});
        window.draw(price);

        // Số lượng
        Text qty(font, String(to_string(item.quantity)), 20);
        qty.setFillColor(Color::White);
        qty.setPosition({viewX + 720.f, y});
        window.draw(qty);

        // Nút
        window.draw(item.minusBtn);
        window.draw(item.plusBtn);
        Text minus(font, "-", 30);
        minus.setPosition({viewX + 677.f, y - 7.f});
        window.draw(minus);
        Text plus(font, "+", 30);
        plus.setPosition({viewX + 775.f, y - 7.f});
        window.draw(plus);

        y += 50.f;
    }
}

int ComboSelection::getTotalPrice() const {
    int total = 0;
    for (int i = 0; i < comboItems.getSize(); ++i)
        total += comboItems[i].combo.price * comboItems[i].quantity;
    return total;
}

bool ComboSelection::hasSelectedCombos() const {
    for (int i = 0; i < comboItems.getSize(); ++i)
        if (comboItems[i].quantity > 0) return true;
    return false;
}

DLL<ComboOrder> ComboSelection::getSelectedCombos() const {
    DLL<ComboOrder> out;
    for (int i = 0; i < comboItems.getSize(); ++i) {
        const auto& ci = comboItems[i];
        if (ci.quantity > 0) {
            ComboOrder o;
            o.id       = ci.combo.id;
            o.name     = ci.combo.name;
            o.quantity = ci.quantity;
            o.price    = ci.combo.price;
            out.push_back(o);
        }
    }
    return out;
}

void ComboSelection::reset() {
    for (int i = 0; i < comboItems.getSize(); ++i)
        comboItems[i].quantity = 0;
}
