#pragma once

/**
 * @brief Định nghĩa các trạng thái màn hình của ứng dụng
 * 
 * Trạng thái public (Guest/Customer):
 * - HOME, LOGIN, REGISTER, BOOKING, MOVIE_DETAILS, ACCOUNT
 * 
 * Trạng thái Admin:
 * - ADMIN_DASHBOARD: Trang chủ admin
 * - ADMIN_USER_MANAGEMENT: Quản lý tài khoản
 * - ADMIN_MOVIE_MANAGEMENT: Quản lý phim
 * - ADMIN_ROOM_MANAGEMENT: Quản lý phòng chiếu
 * - ADMIN_SHOWTIME_MANAGEMENT: Quản lý suất chiếu
 * - ADMIN_STATISTICS: Thống kê doanh thu
 * 
 * Trạng thái Staff:
 * - STAFF_DASHBOARD: Trang chủ nhân viên
 * - STAFF_TICKET_BOOKING: Bán vé trực tiếp
 * - STAFF_TICKET_SEARCH: Tra cứu vé
 */
enum class AppState {
    // Public states
    HOME, 
    LOGIN,
    REGISTER,
    BOOKING,
    MOVIE_DETAILS,
    ACCOUNT,
    
    // Admin states
    ADMIN_DASHBOARD,
    ADMIN_MOVIES,           // Quản lý phim
    ADMIN_ROOMS,            // Quản lý phòng chiếu
    ADMIN_SHOWTIMES,        // Quản lý suất chiếu
    ADMIN_TICKETS,          // Quản lý vé
    ADMIN_COMBOS,           // Quản lý combo bắp nước
    ADMIN_STAFF,            // Quản lý nhân viên
    ADMIN_CUSTOMERS,        // Quản lý khách hàng
    ADMIN_REVENUE,          // Thống kê doanh thu
    ADMIN_SOLD_TICKETS,     // Thống kê vé đã bán
    ADMIN_CHANGE_PASSWORD,  // Đổi mật khẩu
    
    // Staff states
    STAFF_DASHBOARD,
    STAFF_TICKET_BOOKING,
    STAFF_TICKET_SEARCH
};

enum class AccountTab {
    CUSTOMER_INFO,
    PURCHASE_HISTORY,
    MY_GIFTS
};