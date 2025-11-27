#pragma once
#include <string>

using namespace std;

/**
 * @brief Model đại diện cho một suất chiếu
 * 
 * Một suất chiếu liên kết phim với phòng chiếu tại một thời điểm cụ thể.
 */
struct Showtime {
    string id;          // ID suất chiếu (VD: "SUATCHIEU_20251127_0001")
    string movieId;     // ID phim
    string roomId;      // ID phòng chiếu
    string date;        // Ngày chiếu (YYYY-MM-DD)
    string time;        // Giờ chiếu (HH:MM)
    int price = 0;      // Giá vé (VND)
    
    // ===== CONSTRUCTORS =====
    
    Showtime() = default;
    
    Showtime(const string& id, const string& movieId, const string& roomId,
             const string& date, const string& time, int price)
        : id(id), movieId(movieId), roomId(roomId), 
          date(date), time(time), price(price) {}
    
    // ===== HELPER METHODS =====
    
    /**
     * @brief Lấy giờ chiếu dạng số phút từ 00:00
     */
    int getTimeInMinutes() const {
        int hour = 0, minute = 0;
        size_t colonPos = time.find(':');
        if (colonPos != string::npos) {
            try {
                hour = stoi(time.substr(0, colonPos));
                minute = stoi(time.substr(colonPos + 1));
            } catch (...) {}
        }
        return hour * 60 + minute;
    }
    
    /**
     * @brief Lấy giá vé format có dấu chấm
     */
    string getPriceFormatted() const {
        string priceStr = to_string(price);
        string result;
        int count = 0;
        for (int i = priceStr.length() - 1; i >= 0; i--) {
            if (count > 0 && count % 3 == 0) {
                result = "." + result;
            }
            result = priceStr[i] + result;
            count++;
        }
        return result + " đ";
    }
    
    /**
     * @brief Kiểm tra 2 suất chiếu có cùng ngày không
     */
    bool isSameDate(const Showtime& other) const {
        return date == other.date;
    }
    
    /**
     * @brief So sánh thời gian (dùng cho sort)
     */
    bool operator<(const Showtime& other) const {
        if (date != other.date) return date < other.date;
        return time < other.time;
    }
};

/**
 * @brief Backwards compatible với ShowtimeData (code cũ)
 */
using ShowtimeData = Showtime;
