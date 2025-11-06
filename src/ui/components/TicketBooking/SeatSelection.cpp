#include "UI/components/TicketBooking/SeatSelection.h"
#include <sstream>
#include <iomanip>

SeatSelection::SeatSelection(Font& f) 
    : font(f), ticketPrice(0) {
    initializeSeats();
}

void SeatSelection::initializeSeats() {
    seatMatrix.clear();
    
    // Tạo ma trận 9x9 ghế
    for (int row = 0; row < kRows; ++row) {
        DLL<Seat> rowSeats;
        for (int col = 0; col < kCols; ++col) {
            // Tạo ID ghế: A1, A2, ..., I9
            char rowLabel = getRowLabel(row);
            string seatId = string(1, rowLabel) + to_string(col + 1);
            
            Seat seat(seatId, SeatStatus::AVAILABLE);
            rowSeats.push_back(seat);
        }
        seatMatrix.push_back(rowSeats);
    }
    
    updateSeatColors();
}

char SeatSelection::getRowLabel(int rowIndex) const {
    return 'A' + rowIndex;  // 0→'A', 1→'B', ..., 8→'I'
}

void SeatSelection::setTicketPrice(int price) {
    ticketPrice = price;
}

void SeatSelection::setOccupiedSeats(const DLL<string>& occupiedIds) {
    // Đánh dấu các ghế đã được đặt
    for (int i = 0; i < occupiedIds.getSize(); ++i) {
        string occupiedId = occupiedIds[i];
        
        // Tìm ghế trong matrix và đổi status
        for (int row = 0; row < seatMatrix.getSize(); ++row) {
            for (int col = 0; col < seatMatrix[row].getSize(); ++col) {
                if (seatMatrix[row][col].id == occupiedId) {
                    seatMatrix[row][col].status = SeatStatus::OCCUPIED;
                }
            }
        }
    }
    
    updateSeatColors();
}

void SeatSelection::updateSeatColors() {
    for (int row = 0; row < seatMatrix.getSize(); ++row) {
        for (int col = 0; col < seatMatrix[row].getSize(); ++col) {
            Seat& seat = seatMatrix[row][col];
            
            switch (seat.status) {
                case SeatStatus::AVAILABLE:
                    seat.shape.setFillColor(Color(60, 60, 70));  // Màu bình thường
                    break;
                case SeatStatus::SELECTED:
                    seat.shape.setFillColor(Color(50, 200, 80));  // Xanh lá cây
                    break;
                case SeatStatus::OCCUPIED:
                    seat.shape.setFillColor(Color(40, 40, 45));   // Xám tối
                    break;
            }
            
            seat.shape.setOutlineThickness(1.f);
            seat.shape.setOutlineColor(Color(100, 100, 110));
        }
    }
}

void SeatSelection::handleClick(Vector2f mousePos, bool mousePressed) {
    if (!mousePressed) return;
    
    // Vị trí bắt đầu của ghế (có khoảng cách cho label hàng)
    float startX = kViewX + 50.f;  // Dành chỗ cho label A, B, C...
    float startY = kViewY + 80.f;   // Dành chỗ cho tiêu đề
    
    for (int row = 0; row < seatMatrix.getSize(); ++row) {
        for (int col = 0; col < seatMatrix[row].getSize(); ++col) {
            Seat& seat = seatMatrix[row][col];
            
            // Tính vị trí ghế
            FloatRect seatBounds({
                startX + col * (kSeatSize + kSeatSpacing),
                startY + row * (kSeatSize + kSeatSpacing)
            }, {kSeatSize, kSeatSize});
            
            if (seatBounds.contains(mousePos)) {
                // KHÔNG cho phép click vào ghế đã đặt
                if (seat.status == SeatStatus::OCCUPIED) return;
                
                // Toggle giữa AVAILABLE và SELECTED
                if (seat.status == SeatStatus::AVAILABLE) {
                    seat.status = SeatStatus::SELECTED;
                    selectedSeatIds.push_back(seat.id);
                } 
                else if (seat.status == SeatStatus::SELECTED) {
                    seat.status = SeatStatus::AVAILABLE;
                    
                    // Xóa khỏi danh sách đã chọn
                    for (int i = 0; i < selectedSeatIds.getSize(); ++i) {
                        if (selectedSeatIds[i] == seat.id) {
                            // Tạo DLL mới không chứa phần tử này
                            DLL<string> newList;
                            for (int j = 0; j < selectedSeatIds.getSize(); ++j) {
                                if (j != i) newList.push_back(selectedSeatIds[j]);
                            }
                            selectedSeatIds = newList;
                            break;
                        }
                    }
                }
                
                updateSeatColors();
                return;
            }
        }
    }
}

void SeatSelection::draw(RenderWindow& window) {
    // Tiêu đề
    Text title(font, L"CHỌN GHẾ NGỒI", 28);
    title.setFillColor(Color::White);
    title.setOutlineColor(Color(20, 118, 172));
    title.setOutlineThickness(2.f);
    title.setPosition({kViewX + 20.f, kViewY + 20.f});
    window.draw(title);
    
    // Vẽ label cột (1, 2, 3, ..., 9)
    float startX = kViewX + 50.f;
    float startY = kViewY + 80.f;
    
    for (int col = 0; col < kCols; ++col) {
        Text colLabel(font, to_string(col + 1), 16);
        colLabel.setFillColor(Color(180, 180, 180));
        float labelX = startX + col * (kSeatSize + kSeatSpacing) + kSeatSize/2.f - 5.f;
        colLabel.setPosition({labelX, startY - 25.f});
        window.draw(colLabel);
    }
    
    // Vẽ ghế và label hàng (A, B, C, ...)
    for (int row = 0; row < seatMatrix.getSize(); ++row) {
        // Label hàng
        char rowLabel = getRowLabel(row);
        Text rowText(font, string(1, rowLabel), 18);
        rowText.setFillColor(Color(180, 180, 180));
        rowText.setPosition({kViewX + 20.f, startY + row * (kSeatSize + kSeatSpacing) + 8.f});
        window.draw(rowText);
        
        // Vẽ ghế
        for (int col = 0; col < seatMatrix[row].getSize(); ++col) {
            Seat& seat = seatMatrix[row][col];
            seat.shape.setPosition({
                startX + col * (kSeatSize + kSeatSpacing),
                startY + row * (kSeatSize + kSeatSpacing)
            });
            window.draw(seat.shape);
        }
    }
    
    // Chú thích (Legend)
    float legendY = kViewY + 500.f;
    
    // Ghế có thể chọn
    RectangleShape availableBox({25.f, 25.f});
    availableBox.setPosition({kViewX + 20.f, legendY});
    availableBox.setFillColor(Color(60, 60, 70));
    availableBox.setOutlineThickness(1.f);
    availableBox.setOutlineColor(Color(100, 100, 110));
    window.draw(availableBox);
    Text availableText(font, L"Ghế trống", 16);
    availableText.setFillColor(Color::White);
    availableText.setPosition({kViewX + 55.f, legendY + 3.f});
    window.draw(availableText);
    
    // Ghế đang chọn
    RectangleShape selectedBox({25.f, 25.f});
    selectedBox.setPosition({kViewX + 170.f, legendY});
    selectedBox.setFillColor(Color(50, 200, 80));
    selectedBox.setOutlineThickness(1.f);
    selectedBox.setOutlineColor(Color(100, 100, 110));
    window.draw(selectedBox);
    Text selectedText(font, L"Đang chọn", 16);
    selectedText.setFillColor(Color::White);
    selectedText.setPosition({kViewX + 205.f, legendY + 3.f});
    window.draw(selectedText);
    
    // Ghế đã đặt
    RectangleShape occupiedBox({25.f, 25.f});
    occupiedBox.setPosition({kViewX + 330.f, legendY});
    occupiedBox.setFillColor(Color(40, 40, 45));
    occupiedBox.setOutlineThickness(1.f);
    occupiedBox.setOutlineColor(Color(100, 100, 110));
    window.draw(occupiedBox);
    Text occupiedText(font, L"Đã đặt", 16);
    occupiedText.setFillColor(Color::White);
    occupiedText.setPosition({kViewX + 365.f, legendY + 3.f});
    window.draw(occupiedText);
}

string SeatSelection::getSelectedSeatsDisplay() const {
    if (selectedSeatIds.getSize() == 0) return "";
    
    stringstream ss;
    int displayCount = min(3, selectedSeatIds.getSize());
    
    for (int i = 0; i < displayCount; ++i) {
        ss << selectedSeatIds[i];
        if (i < displayCount - 1) ss << ", ";
    }
    
    // Nếu có hơn 3 ghế, thêm "..."
    if (selectedSeatIds.getSize() > 3) {
        ss << ", ...";
    }
    
    return ss.str();
}

string SeatSelection::getFormattedPrice() const {
    int total = getTotalPrice();
    
    // Format: x0.000 VND (dấu chấm cách 3 số)
    stringstream ss;
    string numStr = to_string(total);
    int len = numStr.length();
    
    for (int i = 0; i < len; ++i) {
        ss << numStr[i];
        // Thêm dấu chấm sau mỗi 3 số (từ phải sang)
        if ((len - i - 1) % 3 == 0 && i != len - 1) {
            ss << ".";
        }
    }
    
    ss << " VND";
    return ss.str();
}
