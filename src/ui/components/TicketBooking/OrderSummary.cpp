#include "UI/components/TicketBooking/OrderSummary.h"
#include <sstream>
#include <algorithm>

using namespace OrderSummaryLayout;

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

void OrderSummary::draw(RenderWindow& window, const VoucherDisplayState& voucherState, int discountAmount) {
    float x = PanelX, y = PanelY;

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

    int subtotal = getTotal();
    int appliedDiscount = std::clamp(discountAmount, 0, subtotal);
    int finalTotal = std::max(0, subtotal - appliedDiscount);

    float voucherLabelY = VoucherSectionY - 10.f;
    Text voucherLabel(font, L"Voucher giảm giá", 22);
    voucherLabel.setFillColor(Color(190, 190, 200));
    voucherLabel.setPosition({VoucherLabelX, voucherLabelY});
    window.draw(voucherLabel);

    Vector2f inputPos(VoucherLabelX, VoucherSectionY + 25.f);
    RectangleShape inputBox({VoucherInputWidth, VoucherInputHeight});
    inputBox.setPosition(inputPos);
    inputBox.setFillColor(voucherState.userLoggedIn ? Color(22, 40, 70, 230) : Color(35, 35, 40, 170));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(voucherState.inputActive ? Color(64, 156, 255) : Color(70, 90, 120));
    window.draw(inputBox);

    string upperText = voucherState.inputText;
    for (auto& ch : upperText)
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));

    Text inputText(font, String::fromUtf8(upperText.begin(), upperText.end()), 20);
    inputText.setFillColor(Color::White);
    inputText.setPosition(inputBox.getPosition() + Vector2f(14.f, 10.f));

    bool showPlaceholder = upperText.empty();
    if (showPlaceholder) {
        inputText.setString(L"Nhập mã voucher");
        inputText.setFillColor(Color(150, 150, 160));
    }
    window.draw(inputText);

    if (voucherState.inputActive && voucherState.caretVisible) {
        float caretX = inputText.getPosition().x + (showPlaceholder ? 0.f : inputText.getLocalBounds().size.x + 2.f);
        RectangleShape caret({2.f, 26.f});
        caret.setPosition({caretX, inputBox.getPosition().y + 8.f});
        caret.setFillColor(Color::White);
        window.draw(caret);
    }

    Vector2f buttonPos(inputBox.getPosition().x + VoucherInputWidth + VoucherButtonSpacing, inputBox.getPosition().y);
    RectangleShape applyButton({VoucherButtonWidth, VoucherButtonHeight});
    applyButton.setPosition(buttonPos);
    bool buttonEnabled = voucherState.userLoggedIn && !voucherState.inputText.empty();
    applyButton.setFillColor(buttonEnabled ? Color(64, 156, 255) : Color(80, 80, 90));
    window.draw(applyButton);

    Text applyText(font, L"Áp dụng", 20);
    applyText.setFillColor(Color::White);
    FloatRect btnBounds = applyText.getLocalBounds();
    applyText.setPosition({buttonPos.x + (VoucherButtonWidth - btnBounds.size.x) / 2.f,
                           buttonPos.y + (VoucherButtonHeight - btnBounds.size.y) / 2.f});
    window.draw(applyText);

    float statusY = inputBox.getPosition().y + VoucherInputHeight + 14.f;
    if (!voucherState.statusMessage.empty()) {
        String statusStr = String::fromUtf8(voucherState.statusMessage.begin(), voucherState.statusMessage.end());
        Text status(font, statusStr, 18);
        status.setFillColor(voucherState.statusIsError ? Color(230, 120, 120) : Color(85, 204, 137));
        status.setPosition({VoucherLabelX, statusY});
        window.draw(status);
        statusY += 26.f;
    }

    float priceY = PriceSectionY + 30.f;
    RectangleShape priceDivider({720.f, 2.f});
    priceDivider.setPosition({x + 20.f, priceY});
    priceDivider.setFillColor(Color(60, 60, 70));
    window.draw(priceDivider);

    priceY += 20.f;
    Text subtotalLabel(font, L"Tạm tính", 22);
    subtotalLabel.setFillColor(Color(200, 200, 210));
    subtotalLabel.setPosition({x + 20.f, priceY});
    window.draw(subtotalLabel);

    Text subtotalValue(font, fmtMoney(subtotal), 24);
    subtotalValue.setFillColor(Color::White);
    subtotalValue.setPosition({x + 540.f, priceY});
    window.draw(subtotalValue);

    priceY += 36.f;
    Text discountLabel(font, L"Giảm giá", 22);
    discountLabel.setFillColor(Color(200, 200, 210));
    discountLabel.setPosition({x + 20.f, priceY});
    window.draw(discountLabel);

    Text discountValueText(font, fmtMoney(appliedDiscount), 24);
    discountValueText.setFillColor(appliedDiscount > 0 ? Color(85, 204, 137) : Color(180, 180, 190));
    discountValueText.setPosition({x + 540.f, priceY});
    window.draw(discountValueText);

    priceY += 44.f;
    Text grandLabel(font, L"TỔNG CỘNG", 26);
    grandLabel.setFillColor(Color::White);
    grandLabel.setPosition({x + 20.f, priceY});
    window.draw(grandLabel);

    Text grandValue(font, fmtMoney(finalTotal), 30);
    grandValue.setFillColor(Color(255, 215, 0));
    grandValue.setPosition({x + 520.f, priceY});
    window.draw(grandValue);
}
