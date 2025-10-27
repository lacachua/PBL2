#include "Showtime.h"
#include "CSVReader.h"
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
        if (row.size() < 7) continue; // Skip invalid rows
        
        Showtime show;
        show.movie_id = stoi(row[0]);
        show.date = row[1];
        show.time = row[2];
        show.room = row[3];
        show.available_seats = stoi(row[4]);
        show.total_seats = stoi(row[5]);
        show.price = stoi(row[6]);
        
        // ✅ Load seat map từ seats_status.csv
        show.seat_map = loadSeatMap(show.movie_id, show.date, show.time, show.room);
        
        showtimes.push_back(show);
    }
    
    return showtimes;
}

// ✅ Đọc trạng thái ghế từ file seats_status.csv
string loadSeatMap(int movie_id, const string& date, const string& time, const string& room) {
    string filename = "data/seats_status.csv";
    
    if (!CSVReader::fileExists(filename)) {
        // Nếu file không tồn tại, trả về tất cả ghế trống (81 ghế = 9x9)
        return string(81, '1');
    }
    
    auto data = CSVReader::readCSV(filename, true);
    
    for (const auto& row : data) {
        if (row.size() < 5) continue;
        
        if (stoi(row[0]) == movie_id && 
            row[1] == date && 
            row[2] == time && 
            row[3] == room) {
            return row[4]; // Trả về seat_map
        }
    }
    
    // Nếu không tìm thấy, trả về tất cả ghế trống
    return string(81, '1');
}

// ✅ Lưu trạng thái ghế vào file seats_status.csv
void saveSeatMap(int movie_id, const string& date, const string& time, const string& room, const string& seat_map) {
    string filename = "data/seats_status.csv";
    vector<vector<string>> data;
    bool found = false;
    
    // Đọc dữ liệu hiện tại
    if (CSVReader::fileExists(filename)) {
        data = CSVReader::readCSV(filename, false); // Bao gồm header
        
        // Tìm và cập nhật dòng tương ứng
        for (size_t i = 1; i < data.size(); i++) {
            if (data[i].size() < 5) continue;
            
            if (stoi(data[i][0]) == movie_id && 
                data[i][1] == date && 
                data[i][2] == time && 
                data[i][3] == room) {
                data[i][4] = seat_map;
                found = true;
                break;
            }
        }
    } else {
        // Tạo file mới với header
        data.push_back({"movie_id", "date", "time", "room", "seat_map"});
    }
    
    // Nếu chưa có, thêm dòng mới
    if (!found) {
        data.push_back({
            to_string(movie_id),
            date,
            time,
            room,
            seat_map
        });
    }
    
    // Ghi lại file
    ofstream file(filename);
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}
