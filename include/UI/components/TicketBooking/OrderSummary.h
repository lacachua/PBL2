#pragma once
#include <SFML/Graphics.hpp>
#include "data-structures/DLL.h"
#include "UI/components/TicketBooking/ComboSelection.h"
#include "UI/components/TicketBooking/SeatSelection.h"
using namespace sf;
using namespace std;

// Mỗi dòng trong bảng đơn hàng
struct OrderItem {
    String description;
    int quantity;
    int totalPrice;
};

class OrderSummary {
private:
    Font& font;
    DLL<OrderItem> items;

    String fmtMoney(int v) const;

public:
    OrderSummary(Font& f);

    // clear & add items
    void reset();
    void addItem(const String& desc, int qty, int pricePerUnit);

    // sinh dữ liệu từ ghế + combo
    void generateFromSelections(const SeatSelection* seatSel, const ComboSelection* comboSel);

    // tính tổng
    int getTotal() const;

    // render UI
    void draw(RenderWindow& window);
};
