#pragma once
#include <SFML/Graphics.hpp>
#include "data-structures/DLL.h"
#include <string>
using namespace sf;
using namespace std;

enum class SeatStatus {
    AVAILABLE,   // Ghế có thể chọn (màu bình thường)
    SELECTED,    // Ghế đang chọn (màu xanh lá)
    OCCUPIED     // Ghế đã đặt (màu xám tối)
};

// Thông tin 1 ghế
struct Seat {
    string id;
    SeatStatus status;
    RectangleShape shape;
    
    Seat(const string& seatId, SeatStatus st = SeatStatus::AVAILABLE);
};

class SeatSelection {
private:
    Font& font;
    DLL<DLL<Seat>> seatMatrix;
    DLL<string> selectedSeatIds;
    int ticketPrice;
    
    // Hằng số bố cục
    static constexpr float kViewX = 174.f;
    static constexpr float kViewY = 220.f;
    static constexpr float kSeatSize = 30.f;
    static constexpr float kSeatSpacing = 8.f;
    static constexpr int kRows = 9;     // A-I
    static constexpr int kCols = 9;     // 1-9
    
    // Các hàm hỗ trợ
    void initializeSeats();
    void updateSeatColors();
    char getRowLabel(int rowIndex) const;  // 0→'A', 1→'B', ..., 8→'I'
    
public:
    SeatSelection(Font&);
    
    void setTicketPrice(int price);
    void setOccupiedSeats(const DLL<string>& occupiedIds);  // Từ database
    void resetSeats();  // Reset toàn bộ ghế về trạng thái ban đầu
    void handleClick(Vector2f mousePos, bool mousePressed);
    void draw(RenderWindow&);
    
    // Các hàm getter
    DLL<string> getSelectedSeats() const;
    int getTotalPrice() const;
    bool hasSelectedSeats() const;
    int  getSelectedCount() const;   // số ghế đang chọn
    int  getUnitPrice()     const;   // giá 1 ghế
    
    // Định dạng hiển thị
    string getSelectedSeatsDisplay() const;  // "A1, B2, C3, ..."
    string getFormattedPrice() const;        // "120.000 VND"
};
