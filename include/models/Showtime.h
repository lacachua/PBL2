#pragma once
#include <string>

namespace models {

/**
 * @brief Showtime entity - Represents a movie screening schedule
 * POCO - No dependencies
 */
class Showtime {
private:
    std::string showtimeId;   // e.g., "S0001"
    int movieId;
    std::string date;         // YYYY-MM-DD
    std::string time;         // HH:MM
    std::string room;         // e.g., "Phòng 1", "Phòng 2"
    int availableSeats;
    int totalSeats;
    int priceVnd;
    std::string status;       // "Sắp chiếu", "Đang chiếu", "Đã chiếu"
    
public:
    Showtime() : movieId(0), availableSeats(0), totalSeats(81), priceVnd(0) {}
    
    Showtime(const std::string& id, int movieId, 
             const std::string& date, const std::string& time,
             const std::string& room, int price)
        : showtimeId(id), movieId(movieId), date(date), 
          time(time), room(room), availableSeats(81), 
          totalSeats(81), priceVnd(price), status("Sắp chiếu") {}
    
    // Getters
    std::string getShowtimeId() const { return showtimeId; }
    int getMovieId() const { return movieId; }
    std::string getDate() const { return date; }
    std::string getTime() const { return time; }
    std::string getRoom() const { return room; }
    int getAvailableSeats() const { return availableSeats; }
    int getTotalSeats() const { return totalSeats; }
    int getPriceVnd() const { return priceVnd; }
    std::string getStatus() const { return status; }
    
    // Setters
    void setShowtimeId(const std::string& id) { showtimeId = id; }
    void setMovieId(int id) { movieId = id; }
    void setDate(const std::string& d) { date = d; }
    void setTime(const std::string& t) { time = t; }
    void setRoom(const std::string& r) { room = r; }
    void setAvailableSeats(int seats) { availableSeats = seats; }
    void setTotalSeats(int seats) { totalSeats = seats; }
    void setPriceVnd(int price) { priceVnd = price; }
    void setStatus(const std::string& s) { status = s; }
    
    // Business logic
    void decrementAvailableSeats(int count = 1) {
        availableSeats -= count;
        if (availableSeats < 0) availableSeats = 0;
    }
    
    void incrementAvailableSeats(int count = 1) {
        availableSeats += count;
        if (availableSeats > totalSeats) availableSeats = totalSeats;
    }
    
    bool hasAvailableSeats() const {
        return availableSeats > 0;
    }
    
    int getOccupiedSeats() const {
        return totalSeats - availableSeats;
    }
    
    // Generate showtime key for seat status lookup
    std::string getShowtimeKey() const {
        return date + "_" + time + "_" + getRoomId();
    }
    
    // Get room ID (e.g., "Phòng 1" → "R001")
    std::string getRoomId() const {
        if (room.find("Phòng ") == 0) {
            int roomNum = std::stoi(room.substr(6));
            char buffer[5];
            snprintf(buffer, sizeof(buffer), "R%03d", roomNum);
            return std::string(buffer);
        }
        return room;
    }
    
    // Get film ID string from numeric ID (e.g., 1 → "F0001")
    std::string getFilmId() const {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "F%04d", movieId);
        return std::string(buffer);
    }
    
    // Validation
    bool isValid() const {
        return !showtimeId.empty() &&
               movieId > 0 &&
               !date.empty() &&
               !time.empty() &&
               !room.empty() &&
               priceVnd >= 0;
    }
};

} // namespace models
