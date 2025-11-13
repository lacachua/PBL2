#pragma once

/**
 * @brief Định nghĩa các vai trò người dùng trong hệ thống CineXine
 * 
 * Phân quyền theo hệ thống phân cấp:
 * - Guest: Khách vãng lai (chưa đăng nhập)
 * - Customer: Khách hàng đã đăng ký
 * - Staff: Nhân viên bán vé
 * - Admin: Quản trị viên hệ thống
 */
enum class AppRole {
    Guest,      // Chỉ xem phim, không thể đặt vé
    Customer,   // Đặt vé, xem lịch sử, quản lý tài khoản cá nhân
    Staff,      // Bán vé, tra cứu vé, hủy vé
    Admin       // Toàn quyền: quản lý user, phim, phòng, suất chiếu, thống kê
};

/**
 * @brief Trạng thái tài khoản người dùng
 */
enum class UserStatus {
    Active,     // Tài khoản hoạt động bình thường
    Locked      // Tài khoản bị khóa, không thể đăng nhập
};
