#pragma once
#include <string>
#include <vector>

enum class BookingState {
    suatchieu,
    ghengoi,
    bapnuoc,
    thanhtoan,
    xacnhan
};

// Struct to store all booking information across states
struct BookingData {
    // Showtime data
    std::string showtimeId;
    std::string movieId;
    std::string movieName;
    std::string roomId;
    std::string roomName;
    std::string date;
    std::string time;
    int ticketPrice = 0;
    
    // Seat data
    std::vector<std::string> selectedSeats;  // e.g., ["A1", "A2"]
    std::string seatsDisplay;  // "A1, A2, A3"
    int totalSeats = 0;
    
    // Combo data
    struct ComboItem {
        std::string comboId;
        std::string comboName;
        int price;
        int quantity;
    };
    std::vector<ComboItem> selectedCombos;
    std::string combosDisplay;  // "Combo A (x2), Combo B (x1)"
    int comboTotalPrice = 0;
    
    // User data
    std::string customerName;
    std::string customerEmail;
    std::string customerPhone;
    
    // Final data
    std::string ticketId;
    int totalPrice = 0;
    
    // Helper method to format combo display with line breaks
    std::string getFormattedComboDisplay(int maxPerLine = 2) const {
        if (selectedCombos.empty()) return "Không có";
        
        std::string result;
        for (size_t i = 0; i < selectedCombos.size(); i++) {
            if (i > 0) {
                // Add line break every maxPerLine items
                if (i % maxPerLine == 0) {
                    result += "\n";
                } else {
                    result += ", ";
                }
            }
            result += selectedCombos[i].comboName;
            if (selectedCombos[i].quantity > 1) {
                result += " (x" + std::to_string(selectedCombos[i].quantity) + ")";
            }
        }
        return result;
    }
    
    // Helper method for purchase history (max 2 combos, then "...")
    std::string getComboDisplayForHistory() const {
        if (selectedCombos.empty()) return "Không có";
        
        std::string result;
        size_t displayCount = std::min(selectedCombos.size(), size_t(2));
        
        for (size_t i = 0; i < displayCount; i++) {
            if (i > 0) result += ", ";
            result += selectedCombos[i].comboName;
            if (selectedCombos[i].quantity > 1) {
                result += " (x" + std::to_string(selectedCombos[i].quantity) + ")";
            }
        }
        
        if (selectedCombos.size() > 2) {
            result += "...";
        }
        
        return result;
    }
    
    void clear() {
        showtimeId.clear();
        movieId.clear();
        movieName.clear();
        roomId.clear();
        roomName.clear();
        date.clear();
        time.clear();
        ticketPrice = 0;
        
        selectedSeats.clear();
        seatsDisplay.clear();
        totalSeats = 0;
        
        selectedCombos.clear();
        combosDisplay.clear();
        comboTotalPrice = 0;
        
        customerName.clear();
        customerEmail.clear();
        customerPhone.clear();
        
        ticketId.clear();
        totalPrice = 0;
    }
};