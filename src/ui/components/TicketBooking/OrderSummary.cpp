#include "UI/components/TicketBooking/OrderSummary.h"
#include <sstream>

OrderSummary::OrderSummary(Font& f) : font(f) {}

String OrderSummary::fmtMoney(int v) const {
    string s = to_string(v), out;
    for (int i = 0; i < (int)s.size(); i++) {
        out += s[i];
        if ((int)s.size() - i - 1 > 0 && ((int)s.size() - i - 1) % 3 == 0) out += ".";
    }
    out += " đ";
    return String::fromUtf8(out.begin(), out.end());
}

void OrderSummary::reset() {
    items.clear();
}

void OrderSummary::addItem(const String& desc, int qty, int pricePerUnit) {
    if (qty <= 0) return;
    OrderItem item;
    item.description = desc;
    item.quantity = qty;
    item.totalPrice = qty * pricePerUnit;
    items.push_back(item);
}

void OrderSummary::generateFromSelections(const SeatSelection* seatSel, const ComboSelection* comboSel) {
    reset();

    if (seatSel) {
        int seatCount = seatSel->getSelectedCount();
        if (seatCount > 0) addItem(L"Ghế đơn", seatCount, seatSel->getUnitPrice());
    }

    if (comboSel) {
        DLL<ComboOrder> chosen = comboSel->getSelectedCombos();
        for (int i = 0; i < chosen.getSize(); i++) {
            const auto& ci = chosen[i];
            addItem(ci.name, ci.quantity, ci.price);
        }
    }
}

int OrderSummary::getTotal() const {
    int total = 0;
    for (int i = 0; i < items.getSize(); ++i)
        total += items[i].totalPrice;
    return total;
}

void OrderSummary::draw(RenderWindow& window) {
    float x = 174.f, y = 270.f;

    Text title(font, L"Tóm tắt đơn hàng", 28);
    title.setFillColor(Color::White);
    title.setPosition({x + 20.f, y - 40.f});
    title.setOutlineThickness(2.f);
    title.setOutlineColor(Color(20, 118, 172));
    window.draw(title);

    Text h1(font, L"MÔ TẢ", 20), h2(font, L"SỐ LƯỢNG", 20), h3(font, L"THÀNH TIỀN", 20);
    h1.setPosition({x + 20.f, y});
    h2.setPosition({x + 360.f, y});
    h3.setPosition({x + 560.f, y});
    h1.setFillColor(Color(180, 180, 180));
    h2.setFillColor(Color(180, 180, 180));
    h3.setFillColor(Color(180, 180, 180));
    window.draw(h1); window.draw(h2); window.draw(h3);

    y += 30.f;
    for (int i = 0; i < items.getSize(); i++) {
        auto& it = items[i];

        Text desc(font, it.description, 20);
        desc.setFillColor(Color::White);
        desc.setPosition({x + 20.f, y});

        Text qty(font, String(to_string(it.quantity)), 20);
        qty.setFillColor(Color::White);
        qty.setPosition({x + 400.f, y});

        Text total(font, fmtMoney(it.totalPrice), 20);
        total.setFillColor(Color::White);
        total.setPosition({x + 560.f, y});

        window.draw(desc);
        window.draw(qty);
        window.draw(total);

        y += 40.f;
    }

    RectangleShape line({720.f, 2.f});
    line.setPosition({x + 20.f, y});
    line.setFillColor(Color(60, 60, 70));
    window.draw(line);

    y += 10.f;
    Text totalT(font, L"Tổng cộng", 22);
    totalT.setFillColor(Color(200, 200, 200));
    totalT.setPosition({x + 20.f, y});
    window.draw(totalT);

    Text totalVal(font, fmtMoney(getTotal()), 24);
    totalVal.setFillColor(Color::White);
    totalVal.setPosition({x + 560.f, y});
    window.draw(totalVal);
}
