#pragma once
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Model đại diện cho một phòng chiếu
 */
struct Room {
    string id;              // ID phòng (VD: "R01")
    string name;            // Tên phòng (VD: "Phòng chiếu 1")
    int capacity = 0;       // Sức chứa (số ghế)
    string type;            // Loại phòng (VD: "2D", "3D", "IMAX")
    string status;          // Trạng thái ("Hoạt động", "Bảo trì")
    
    // ===== CONSTRUCTORS =====
    
    Room() = default;
    
    Room(const string& id, const string& name, int capacity = 0, 
         const string& type = "2D", const string& status = "Hoạt động")
        : id(id), name(name), capacity(capacity), type(type), status(status) {}
    
    // ===== HELPER METHODS =====
    
    bool isActive() const {
        return status == "Hoạt động" || status == "active";
    }
};

/**
 * @brief Model đại diện cho một vé đã đặt
 */
struct Ticket {
    string ticketId;        // ID vé (VD: "T0001")
    string showtimeId;      // ID suất chiếu
    string movieTitle;      // Tên phim (lưu để hiển thị)
    string date;            // Ngày chiếu
    string time;            // Giờ chiếu
    string roomName;        // Tên phòng
    string seats;           // Danh sách ghế (VD: "A1, A2, A3")
    string combos;          // Combo đã chọn (VD: "Combo A x2, Combo B x1")
    int totalPrice = 0;     // Tổng giá (VND)
    
    // Customer info
    string customerEmail;   // Email khách hàng
    string customerName;    // Tên khách hàng
    
    // Booking metadata
    string bookedDate;      // Ngày đặt vé
    string bookedTime;      // Giờ đặt vé
    
    // ===== CONSTRUCTORS =====
    
    Ticket() = default;
    
    // ===== HELPER METHODS =====
    
    /**
     * @brief Đếm số ghế đã đặt
     */
    int getSeatCount() const {
        if (seats.empty()) return 0;
        int count = 1;
        for (char c : seats) {
            if (c == ',') count++;
        }
        return count;
    }
    
    /**
     * @brief Lấy giá format có dấu chấm
     */
    string getPriceFormatted() const {
        string priceStr = to_string(totalPrice);
        string result;
        int count = 0;
        for (int i = priceStr.length() - 1; i >= 0; i--) {
            if (count > 0 && count % 3 == 0) {
                result = "." + result;
            }
            result = priceStr[i] + result;
            count++;
        }
        return result + " đ";
    }
};

/**
 * @brief Model đại diện cho một Combo bắp nước
 */
struct Combo {
    string id;              // ID combo (VD: "C01")
    string name;            // Tên combo (VD: "Combo Đôi")
    string description;     // Mô tả
    int price = 0;          // Giá (VND)
    string imagePath;       // Đường dẫn ảnh (optional)
    
    // ===== CONSTRUCTORS =====
    
    Combo() = default;
    
    Combo(const string& id, const string& name, int price, const string& desc = "")
        : id(id), name(name), description(desc), price(price) {}
    
    /**
     * @brief Lấy giá format có dấu chấm
     */
    string getPriceFormatted() const {
        string priceStr = to_string(price);
        string result;
        int count = 0;
        for (int i = priceStr.length() - 1; i >= 0; i--) {
            if (count > 0 && count % 3 == 0) {
                result = "." + result;
            }
            result = priceStr[i] + result;
            count++;
        }
        return result + " đ";
    }
};

/**
 * @brief Model đại diện cho một Voucher
 */
struct Voucher {
    string code;            // Mã voucher (VD: "WELCOME", "SALE50")
    int type = 1;           // Loại: 1 = fixed amount, 2 = percentage
    double value = 0.0;     // Giá trị giảm (VND hoặc %)
    double minBill = 0.0;   // Hóa đơn tối thiểu để áp dụng
    string description;     // Mô tả
    
    // ===== CONSTRUCTORS =====
    
    Voucher() = default;
    
    Voucher(const string& code, int type, double value, double minBill, const string& desc = "")
        : code(code), type(type), value(value), minBill(minBill), description(desc) {}
    
    // ===== HELPER METHODS =====
    
    /**
     * @brief Tính discount cho một hóa đơn
     * @param billAmount Số tiền hóa đơn
     * @return Số tiền được giảm
     */
    double calculateDiscount(double billAmount) const {
        if (billAmount < minBill) return 0.0;
        
        if (type == 1) {
            return min(value, billAmount); // Fixed amount
        } else {
            return billAmount * value / 100.0; // Percentage
        }
    }
    
    /**
     * @brief Lấy mô tả dạng đọc được
     */
    string getValueDescription() const {
        if (type == 1) {
            return "Giảm " + to_string(static_cast<int>(value)) + "đ";
        } else {
            return "Giảm " + to_string(static_cast<int>(value)) + "%";
        }
    }
    
    /**
     * @brief Kiểm tra voucher có áp dụng được cho hóa đơn không
     */
    bool isApplicable(double billAmount) const {
        return billAmount >= minBill;
    }
};

/**
 * @brief Model cho voucher đã cấp cho user
 */
struct UserVoucher {
    string email;           // Email người nhận
    string code;            // Mã voucher
    int status = 1;         // 1: usable, 0: used/expired
    string expiryDate;      // Ngày hết hạn (YYYYMMDD)
    int quantity = 1;       // Số lượng còn lại
    
    UserVoucher() = default;
    
    UserVoucher(const string& email, const string& code, int status, 
                const string& expiry, int qty = 1)
        : email(email), code(code), status(status), expiryDate(expiry), quantity(qty) {}
    
    bool isUsable() const { return status == 1 && quantity > 0; }
};
