#pragma once
#include <SFML/Graphics.hpp>
#include "data-structures/DLL.h"
#include "UI/components/TicketBooking/ComboSelection.h"
#include "UI/components/TicketBooking/SeatSelection.h"
#include "UI/components/TicketBooking/VoucherDisplayState.h"
using namespace sf;
using namespace std;

namespace OrderSummaryLayout {
    inline constexpr float PanelX = 174.f;
    inline constexpr float PanelY = 270.f;
    inline constexpr float VoucherLabelX = PanelX + 20.f;
    inline constexpr float VoucherSectionY = 600.f;
    inline constexpr float VoucherInputWidth = 360.f;
    inline constexpr float VoucherInputHeight = 48.f;
    inline constexpr float VoucherButtonWidth = 150.f;
    inline constexpr float VoucherButtonHeight = 48.f;
    inline constexpr float VoucherButtonSpacing = 18.f;
    inline constexpr float PriceSectionY = VoucherSectionY + 110.f;
}

struct OrderItem {
    String description;
    int quantity;
    int totalPrice;
};

class OrderSummary {
private:
    Font& font;
    DLL<OrderItem> items;
    String fmtMoney(int) const;
public:
    OrderSummary(Font&);
    void reset();
    void addItem(const String&, int, int);
    void generateFromSelections(const SeatSelection*, const ComboSelection*);
    int getTotal() const;
    void draw(RenderWindow&, const VoucherDisplayState&, int discountAmount);
};
