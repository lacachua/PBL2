#pragma once
#include <string>
#include <ctime>
#include "core/AppRole.h"
#include "models/IUser.h"

using namespace std;

/**
 * @brief Concrete class User - Backwards compatible với code cũ
 * 
 * Kế thừa từ IUser và implement các phương thức virtual.
 * Class này giữ lại để tương thích với code hiện có.
 * 
 * Cho code mới, nên sử dụng:
 * - Guest, Customer, Staff, Admin classes
 * - UserFactory để tạo user
 * 
 * Note: Email là khóa chính, KHÔNG ĐƯỢC PHÉP THAY ĐỔI
 */
class User : public IUser {
public:
    /**
     * @brief Constructor mặc định
     */
    User();

    /**
     * @brief Constructor đầy đủ
     */
    User(const string& email, 
         const string& passwordHash,
         const string& fullName,
         const string& birthDate,
         const string& phone,
         time_t registeredAt,
         AppRole role = AppRole::Customer,
         UserStatus status = UserStatus::Active);

    // ===== IMPLEMENT PURE VIRTUAL METHODS từ IUser =====
    
    string getUserType() const override {
        switch (role) {
            case AppRole::Admin: return "Admin";
            case AppRole::Customer: return "Customer";
            default: return "Guest";
        }
    }
    
    bool hasPermission(const string& feature) const override {
        // Admin có tất cả quyền
        if (role == AppRole::Admin) return true;
        
        // Customer có quyền đặt vé, xem voucher
        if (role == AppRole::Customer || role == AppRole::Admin) {
            if (feature == "book_ticket") return true;
            if (feature == "view_booking_history") return true;
            if (feature == "use_voucher") return true;
            if (feature == "view_vouchers") return true;
            if (feature == "update_profile") return true;
            if (feature == "view_account") return true;
        }
        
        // Guest chỉ có quyền xem
        if (feature == "view_movies") return true;
        if (feature == "view_movie_detail") return true;
        if (feature == "search_movies") return true;
        
        return false;
    }
    
    string getPermissions() const override {
        switch (role) {
            case AppRole::Admin:
                return "Toàn quyền quản trị";
            case AppRole::Customer:
                return "Đặt vé, Xem lịch sử, Sử dụng voucher";
            default:
                return "Xem phim";
        }
    }

    // ===== SETTER cho role (User class cần setRole) =====
    void setRole(AppRole r) { role = r; }
};
