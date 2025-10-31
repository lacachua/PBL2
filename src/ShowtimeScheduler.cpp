#include "ShowtimeScheduler.h"
#include "RoomsRepository.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <filesystem>

// Khung giờ chiếu (6 suất/ngày)
const std::vector<std::string> ShowtimeScheduler::TIME_SLOTS = {
    "09:00", "11:30", "14:00", "16:30", "19:00", "21:30"
};

// 6 phòng (sẽ load từ rooms.csv)
const std::vector<std::string> ShowtimeScheduler::ROOMS = {
    "R001", "R002", "R003", "R004", "R005", "R006"
};

std::string ShowtimeScheduler::addDays(const std::string& date, int days) {
    // Parse YYYY-MM-DD
    std::istringstream iss(date);
    int year, month, day;
    char dash;
    iss >> year >> dash >> month >> dash >> day;
    
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    
    std::time_t time = std::mktime(&tm);
    time += days * 24 * 3600;
    
    std::tm* newTm = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(newTm, "%Y-%m-%d");
    return oss.str();
}

std::string ShowtimeScheduler::makeSlotKey(
    const std::string& date,
    const std::string& time,
    const std::string& room
) {
    return date + "|" + time + "|" + room;
}

bool ShowtimeScheduler::isSlotTaken(
    const std::set<std::string>& usedSlots,
    const std::string& date,
    const std::string& time,
    const std::string& room
) {
    return usedSlots.find(makeSlotKey(date, time, room)) != usedSlots.end();
}

std::vector<Showtime> ShowtimeScheduler::generateSchedule(
    const std::vector<int>& movieIds,
    const std::string& startDate,
    int numDays
) {
    std::vector<Showtime> showtimes;
    std::set<std::string> usedSlots;
    
    std::cout << "\n[ShowtimeScheduler] ===== PHÂN PHỐI ĐỀU TẤT CẢ PHIM =====" << std::endl;
    std::cout << "[ShowtimeScheduler] Movies: " << movieIds.size() 
              << ", Days: " << numDays 
              << ", Slots/day: " << TIME_SLOTS.size()
              << ", Rooms: " << ROOMS.size() << std::endl;
    
    // Load room info từ CSV
    auto roomInfos = RoomsRepository::loadAll("data/rooms.csv");
    
    // ✅ THUẬT TOÁN ROUND-ROBIN: Phân phối đều từng phim vào slot
    // Mỗi ngày có 6 slots, 6 rooms → 36 slots/ngày
    // Chia đều cho tất cả phim
    
    int totalMovies = movieIds.size();
    int movieIndex = 0;  // Con trỏ vòng quanh danh sách phim
    
    for (int day = 0; day < numDays; ++day) {
        std::string date = addDays(startDate, day);
        
        // Duyệt qua từng time slot
        for (const std::string* timePtr = TIME_SLOTS.data(); 
             timePtr < TIME_SLOTS.data() + TIME_SLOTS.size(); 
             ++timePtr) {
            
            // Duyệt qua từng room
            for (const std::string* roomPtr = ROOMS.data(); 
                 roomPtr < ROOMS.data() + ROOMS.size(); 
                 ++roomPtr) {
                
                const std::string& time = *timePtr;
                const std::string& room_id = *roomPtr;
                
                // Kiểm tra slot đã sử dụng chưa
                if (isSlotTaken(usedSlots, date, time, room_id)) {
                    continue;
                }
                
                // Lấy movie_id theo round-robin
                int movie_id = movieIds[movieIndex % totalMovies];
                movieIndex++;
                
                // Tìm thông tin phòng
                RoomInfo roomInfo;
                bool roomFound = false;
                for (size_t i = 0; i < roomInfos.size(); ++i) {
                    if (roomInfos[i].room_id == room_id) {
                        roomInfo = roomInfos[i];
                        roomFound = true;
                        break;
                    }
                }
                
                if (!roomFound) {
                    // Fallback nếu không tìm thấy room
                    roomInfo.room_id = room_id;
                    roomInfo.room_name = "Phòng " + room_id.substr(3); // R001 → Phòng 1
                    roomInfo.rows = 9;
                    roomInfo.cols = 9;
                    roomInfo.total_seats = 81;
                }
                
                // Tạo showtime
                Showtime st;
                st.movie_id = movie_id;
                st.date = date;
                st.time = time;
                st.room = roomInfo.room_name;  // "Phòng 1", "Phòng 2", ...
                st.available_seats = roomInfo.total_seats;
                st.total_seats = roomInfo.total_seats;
                
                // ✅ Giá vé cố định 80000 VND
                st.price = 80000;
                
                showtimes.push_back(st);
                usedSlots.insert(makeSlotKey(date, time, room_id));
            }
        }
    }
    
    std::cout << "[ShowtimeScheduler] ✅ Generated " << showtimes.size() 
              << " showtimes (round-robin)" << std::endl;
    std::cout << "[ShowtimeScheduler] Used " << usedSlots.size() 
              << " unique slots" << std::endl;
    
    return showtimes;
}
