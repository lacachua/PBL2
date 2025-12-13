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

    // Voucher data
    std::string voucherCode;
    int voucherDiscount = 0;
    
    // User data
    std::string customerName;
    std::string customerEmail;
    std::string customerPhone;
    
    // Final data
    std::string ticketId;
    int totalPrice = 0;
    
    // Helper method to format combo display with line breaks
    std::string getFormattedComboDisplay(int maxPerLine = 2) const;
    
    // Helper method for purchase history (max 2 combos, then "...")
    std::string getComboDisplayForHistory() const;
    
    void clear();
};