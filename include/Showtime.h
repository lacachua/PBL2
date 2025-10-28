#pragma once
#include <string>
#include <vector>
using namespace std;

struct Showtime {
    int movie_id;
    string date;
    string time;
    string room;
    int available_seats;
    int total_seats;
    int price;
    string seat_map; // ✅ Bitmap 81 ký tự (9x9): 1=trống, 0=đã đặt
    
    Showtime() : movie_id(0), available_seats(0), total_seats(0), price(0), seat_map("") {}
};

// Function to load showtimes from CSV
vector<Showtime> loadShowtimesFromCSV(const string& filename);

// ✅ Hàm đọc/ghi trạng thái ghế
string loadSeatMap(int movie_id, const string& date, const string& time, const string& room);
void saveSeatMap(int movie_id, const string& date, const string& time, const string& room, const string& seat_map);
