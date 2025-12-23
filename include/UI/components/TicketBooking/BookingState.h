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

// Cau truc giu thong tin dat ve xuyen suot cac buoc
struct BookingData {
    // Thong tin suat chieu
    std::string showtimeId;
    std::string movieId;
    std::string movieName;
    std::string roomId;
    std::string roomName;
    std::string date;
    std::string time;
    int ticketPrice = 0;
    
    // Thong tin ghe
    std::vector<std::string> selectedSeats;  // vi du ["A1", "A2"]
    std::string seatsDisplay;  // "A1, A2, A3"
    int totalSeats = 0;
    
    // Thong tin combo
    struct ComboItem {
        std::string comboId;
        std::string comboName;
        int price;
        int quantity;
    };
    std::vector<ComboItem> selectedCombos;
    std::string combosDisplay;  // "Combo A (x2), Combo B (x1)"
    int comboTotalPrice = 0;

    // Thong tin voucher
    std::string voucherCode;
    int voucherDiscount = 0;
    
    // Thong tin khach hang
    std::string customerName;
    std::string customerEmail;
    std::string customerPhone;
    
    // Tong ket
    std::string ticketId;
    int totalPrice = 0;
    
    std::string getFormattedComboDisplay(int maxPerLine = 2) const;
    std::string getComboDisplayForHistory() const;
    
    void clear();
};