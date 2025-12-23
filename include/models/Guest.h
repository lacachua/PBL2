#pragma once
#include "models/IUser.h"

/**
 * @brief Class Guest - Người dùng chưa đăng nhập
 * 
 * Guest có quyền hạn thấp nhất:
 * - Xem danh sách phim
 * - Xem thông tin phim
 * - Không thể đặt vé
 * - Không thể sử dụng voucher
 */
class Guest : public IUser {
public:
    Guest() : IUser() {
        role = AppRole::Guest;
    }
    
    Guest(const string& email, const string& passwordHash,
          const string& fullName, const string& birthDate,
                    const string& phone, time_t registeredAt)
                : IUser(email, passwordHash, fullName, birthDate, phone,
                                registeredAt, AppRole::Guest) {}

    // ===== IMPLEMENT PURE VIRTUAL METHODS =====
    
    string getUserType() const override {
        return "Guest";
    }
    
    bool hasPermission(const string& feature) const override {
        // Guest chỉ có quyền xem phim
        if (feature == "view_movies") return true;
        if (feature == "view_movie_detail") return true;
        if (feature == "search_movies") return true;
        return false;
    }
    
    string getPermissions() const override {
        return "Xem danh sách phim, Xem chi tiết phim, Tìm kiếm phim";
    }
};
