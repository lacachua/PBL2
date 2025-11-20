#pragma once

enum class AppRole {
    Guest,      // Chỉ xem phim, không thể đặt vé
    Customer,   // Đặt vé, xem lịch sử, quản lý tài khoản cá nhân
    Staff,      // Bán vé, tra cứu vé, hủy vé
    Admin       // Toàn quyền: quản lý user, phim, phòng, suất chiếu, thống kê
};

enum class UserStatus {
    Active,     // Tài khoản hoạt động bình thường
    Locked      // Tài khoản bị khóa, không thể đăng nhập
};
