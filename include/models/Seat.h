#pragma once
#include <string>

namespace models {

/**
 * @brief Seat entity - Represents a cinema seat
 * POCO - No dependencies, pure data + validation
 */
class Seat {
private:
    std::string seatId;      // e.g., "A1", "B5"
    int row;                 // 0-8 (A-I)
    int column;              // 0-8 (1-9)
    bool occupied;           // Already booked
    bool selected;           // Currently selecting
    
public:
    Seat() : row(0), column(0), occupied(false), selected(false) {}
    
    Seat(const std::string& id, int r, int c, bool occ = false)
        : seatId(id), row(r), column(c), occupied(occ), selected(false) {}
    
    // Factory method: Create from seatId string
    static Seat fromId(const std::string& id) {
        if (id.length() < 2) {
            return Seat(); // Invalid
        }
        
        char rowChar = id[0];
        int row = rowChar - 'A';
        int col = std::stoi(id.substr(1)) - 1;
        
        return Seat(id, row, col, false);
    }
    
    // Getters
    std::string getSeatId() const { return seatId; }
    int getRow() const { return row; }
    int getColumn() const { return column; }
    bool isOccupied() const { return occupied; }
    bool isSelected() const { return selected; }
    bool isAvailable() const { return !occupied && !selected; }
    
    // Actions
    void setOccupied(bool occ) { occupied = occ; }
    void setSelected(bool sel) { 
        if (!occupied) {
            selected = sel; 
        }
    }
    void toggleSelection() { 
        if (!occupied) {
            selected = !selected; 
        }
    }
    
    // Validation
    bool isValid() const {
        return row >= 0 && row < 9 &&
               column >= 0 && column < 9 &&
               !seatId.empty();
    }
    
    // Index in 81-seat bitmap
    int getIndex() const {
        return row * 9 + column;
    }
};

} // namespace models
