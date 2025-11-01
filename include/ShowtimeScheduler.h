#pragma once
#include <string>
#include <vector>
#include <set>
#include "models/Showtime.h"

/**
 * Thuật toán phân phòng thông minh cho rạp
 * - 6 phòng: Phòng 1 đến Phòng 6
 * - Khung giờ cố định: 10:00, 12:30, 15:00, 17:30, 20:00, 22:30
 * - Mỗi phòng tại 1 thời điểm chỉ chiếu 1 phim
 * - Phim hot → nhiều suất (nhiều phòng cùng giờ)
 * - Phim ít người → ít suất (1 phòng)
 */
class ShowtimeScheduler {
public:
    /**
     * Tạo lịch chiếu cho tất cả phim trong N ngày tới
     * @param movies Vector of movie IDs
     * @param startDate Starting date (YYYY-MM-DD)
     * @param numDays Number of days to generate
     * @return Vector of Showtime with unique (date, time, room_id)
     */
    static std::vector<Showtime> generateSchedule(
        const std::vector<int>& movieIds,
        const std::string& startDate,
        int numDays
    );

private:
    // Khung giờ cố định
    static const std::vector<std::string> TIME_SLOTS;
    
    // Tên phòng
    static const std::vector<std::string> ROOMS;
    
    // Helper: Add days to date
    static std::string addDays(const std::string& date, int days);
    
    // Helper: Check if slot is taken
    static bool isSlotTaken(
        const std::set<std::string>& usedSlots,
        const std::string& date,
        const std::string& time,
        const std::string& room
    );
    
    // Helper: Make slot key
    static std::string makeSlotKey(
        const std::string& date,
        const std::string& time,
        const std::string& room
    );
};
