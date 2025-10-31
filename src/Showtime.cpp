#include "Showtime.h"
#include "CSVReader.h"
#include "SeatStatusRepository.h"
#include <fstream>
#include <sstream>
#include <algorithm>

vector<Showtime> loadShowtimesFromCSV(const string& filename) {
    vector<Showtime> showtimes;
    
    if (!CSVReader::fileExists(filename)) {
        return showtimes;
    }
    
    auto data = CSVReader::readCSV(filename, true); // skip header
    
    for (const auto& row : data) {
        if (row.size() < 8) continue; // Skip invalid rows (now includes showtime_id)
        
        Showtime show;
        show.showtime_id = row[0];      // ✅ NEW: Read showtime_id
        show.movie_id = stoi(row[1]);
        show.date = row[2];
        show.time = row[3];
        show.room = row[4];
        show.available_seats = stoi(row[5]);
        show.total_seats = stoi(row[6]);
        show.price = stoi(row[7]);
        
        // ✅ Extract room_id from room name
        std::string room_id = show.room;
        room_id.erase(std::remove(room_id.begin(), room_id.end(), ' '), room_id.end());
        room_id.erase(std::remove(room_id.begin(), room_id.end(), 'ò'), room_id.end());
        
        // ✅ Generate showtime key UNIQUE by (date, time, room_id)
        std::string showtime_key = SeatStatusRepository::makeShowtimeKey(
            show.date, show.time, room_id
        );
        show.showtime_id = showtime_key;  // Update showtime_id with generated key
        
        auto layout = SeatStatusRepository::loadOrCreate(
            "data/seats_status.csv",
            showtime_key,
            room_id,
            show.movie_id,
            show.date,
            show.time,
            9, 9
        );
        show.seat_map = layout.seat_bits;
        
        showtimes.push_back(show);
    }
    
    return showtimes;
}

// ✅ DEPRECATED: These functions are replaced by SeatStatusRepository
// Kept for backward compatibility, but now use showtime_id-based system

string loadSeatMap(int movie_id, const string& date, const string& time, const string& room) {
    // Return default empty map - should not be used anymore
    // Use SeatStatusRepository::loadOrCreate with showtime_id instead
    return string(81, '1');
}

void saveSeatMap(int movie_id, const string& date, const string& time, const string& room, const string& seat_map) {
    // DEPRECATED - Use SeatStatusRepository::save with showtime_id instead
    // This function is kept for backward compatibility but does nothing
}
