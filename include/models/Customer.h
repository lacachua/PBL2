#pragma once
#include "models/IUser.h"
#include <vector>

/**
 * @brief Class Customer - Khách hàng đã đăng ký tài khoản
 * 
 * Customer có các quyền:
 * - Tất cả quyền của Guest
 * - Đặt vé xem phim
 * - Quản lý voucher cá nhân
 * - Xem lịch sử đặt vé
 * - Cập nhật thông tin cá nhân
 * 
 * Thuộc tính riêng:
 * - bookingHistory: Lịch sử đặt vé
 * - voucherCodes: Danh sách mã voucher đang có
 */
class Customer : public IUser {
private:
    vector<string> bookingHistory;  // Danh sách ticketId đã đặt
    vector<string> voucherCodes;    // Danh sách voucher đang có

public:
    Customer() : IUser() {
        role = AppRole::Customer;
    }
    
    Customer(const string& email, const string& passwordHash,
             const string& fullName, const string& birthDate,
             const string& phone, time_t registeredAt,
             UserStatus status = UserStatus::Active)
        : IUser(email, passwordHash, fullName, birthDate, phone, 
                registeredAt, AppRole::Customer, status) {}

    // ===== IMPLEMENT PURE VIRTUAL METHODS =====
    
    string getUserType() const override {
        return "Customer";
    }
    
    bool hasPermission(const string& feature) const override {
        // Guest permissions
        if (feature == "view_movies") return true;
        if (feature == "view_movie_detail") return true;
        if (feature == "search_movies") return true;
        
        // Customer-specific permissions
        if (feature == "book_ticket") return true;
        if (feature == "view_booking_history") return true;
        if (feature == "use_voucher") return true;
        if (feature == "view_vouchers") return true;
        if (feature == "update_profile") return true;
        if (feature == "view_account") return true;
        
        return false;
    }
    
    string getPermissions() const override {
        return "Xem phim, Đặt vé, Xem lịch sử đặt vé, Sử dụng voucher, Cập nhật thông tin cá nhân";
    }

    // ===== CUSTOMER-SPECIFIC METHODS =====
    
    /**
     * @brief Thêm ticketId vào lịch sử đặt vé
     */
    void addBooking(const string& ticketId) {
        bookingHistory.push_back(ticketId);
    }
    
    /**
     * @brief Lấy lịch sử đặt vé
     */
    const vector<string>& getBookingHistory() const {
        return bookingHistory;
    }
    
    /**
     * @brief Thêm voucher vào danh sách
     */
    void addVoucher(const string& code) {
        voucherCodes.push_back(code);
    }
    
    /**
     * @brief Xóa voucher sau khi sử dụng
     */
    bool removeVoucher(const string& code) {
        for (auto it = voucherCodes.begin(); it != voucherCodes.end(); ++it) {
            if (*it == code) {
                voucherCodes.erase(it);
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Lấy danh sách voucher
     */
    const vector<string>& getVoucherCodes() const {
        return voucherCodes;
    }
    
    /**
     * @brief Đếm số vé đã đặt
     */
    int getTotalBookings() const {
        return static_cast<int>(bookingHistory.size());
    }
};
