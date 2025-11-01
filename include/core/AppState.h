#pragma once
enum class AppState {
    HOME, 
    LOGIN,
    REGISTER,
    BOOKING,
    MOVIE_DETAILS,
    ACCOUNT
};

enum class AccountTab {
    CUSTOMER_INFO,      // Thông tin khách hàng
    PURCHASE_HISTORY,   // Lịch sử mua hàng
    MY_GIFTS            // Quà tặng của tôi
};