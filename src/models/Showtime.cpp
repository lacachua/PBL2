#include "models/Showtime.h"
#include "utils/CSVReader.h"
#include "SeatStatusRepository.h"
#include "RoomsRepository.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>

vector<Showtime> loadShowtimesFromCSV(const string& filename) {
    vector<Showtime> showtimes;
    
    if (!CSVReader::fileExists(filename)) {
        return showtimes;
    }
    
    auto data = CSVReader::readCSV(filename, true); // skip header
    
    for (const auto& row : data) {
        if (row.size() < 7) continue; // ✅ NEW FORMAT: showtime_id,film_id,room_id,date,time,price_vnd,status
        
        Showtime show;
        show.showtime_id = row[0];      // S0001, S0002, ...
        
        // ✅ Parse film_id: F0001 → movie_id = 1
        std::string film_id = row[1];
        if (film_id.length() >= 5 && film_id[0] == 'F') {
            show.movie_id = std::stoi(film_id.substr(1));  // F0001 → 1
        } else {
            show.movie_id = std::stoi(film_id); // Fallback if already numeric
        }
        
        std::string room_id = row[2];  // R001, R002, ...
        
        // ✅ Convert date format: DD-MM-YYYY → YYYY-MM-DD
        std::string date_dmy = row[3];  // "31-10-2025"
        int day, month, year;
        if (sscanf(date_dmy.c_str(), "%d-%d-%d", &day, &month, &year) == 3) {
            char date_ymd[11];
            snprintf(date_ymd, sizeof(date_ymd), "%04d-%02d-%02d", year, month, day);
            show.date = date_ymd;  // "2025-10-31"
        } else {
            show.date = date_dmy;  // Fallback
        }
        
        show.time = row[4];  // "09:30", "14:00", ...
        show.price = std::stoi(row[5]);  // 95000, 85000, ...
        
        // ✅ Convert room_id to room name: R001 → "Phòng 1"
        if (room_id.length() >= 4 && room_id[0] == 'R') {
            int room_num = std::stoi(room_id.substr(1));  // R001 → 1
            show.room = "Phòng " + std::to_string(room_num);
        } else {
            show.room = room_id;
        }
        
        // ✅ Load room layout to get total_seats
        auto roomInfos = RoomsRepository::loadAll("data/rooms.csv");
        show.total_seats = 81;  // Default
        for (const auto& r : roomInfos) {
            if (r.room_id == room_id) {
                show.total_seats = r.total_seats;
                break;
            }
        }
        show.available_seats = show.total_seats;  // Initially all seats available
        
        // ✅ Generate showtime key: YYYY-MM-DD|HH:MM|ROOM_ID
        std::string showtime_key = SeatStatusRepository::makeShowtimeKey(
            show.date, show.time, room_id
        );
        
        // ✅ Load seat status from seats_status.csv
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
        
        // ✅ Calculate available_seats from seat_map
        int occupied_count = 0;
        for (char c : show.seat_map) {
            if (c == '0') occupied_count++;  // '0' = booked, '1' = available
        }
        show.available_seats = show.total_seats - occupied_count;
        
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
