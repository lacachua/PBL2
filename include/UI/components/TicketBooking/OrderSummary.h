#pragma once
#include <SFML/Graphics.hpp>
#include "data-structures/DLL.h"
#include "UI/components/TicketBooking/ComboSelection.h"
#include "UI/components/TicketBooking/SeatSelection.h"
using namespace sf;
using namespace std;

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
    void draw(RenderWindow&);
};
