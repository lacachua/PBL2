#pragma once
enum class AppState {
    HOME, 
    LOGIN,
    REGISTER,
    BOOKING,
    MOVIE_DETAILS,
    ACCOUNT,
    
    ADMIN_DASHBOARD,
    ADMIN_MOVIES,           // Quản lý phim
    ADMIN_ROOMS,            // Quản lý phòng chiếu
    ADMIN_SHOWTIMES,        // Quản lý suất chiếu
    ADMIN_TICKETS,          // Quản lý vé
    ADMIN_COMBOS,           // Quản lý combo bắp nước
    ADMIN_VOUCHERS,         // Quản lý voucher
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