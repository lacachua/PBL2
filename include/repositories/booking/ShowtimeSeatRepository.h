#pragma once

#include <SFML/Graphics.hpp>

#include <string>

#include "data-structures/DLL.h"

using namespace sf;
using namespace std;

// Cấu trúc lưu trạng thái ghế của 1 suất chiếu
struct ShowtimeSeatStatus {
    string room_id;          // PHONG_001, PHONG_002, ...
    string showtime_id;      // SUATCHIEU_20251106_0001, ...
    DLL<string> bookedSeats; // A1, B2, C3, ...

    ShowtimeSeatStatus() {}
    ShowtimeSeatStatus(const string& rId, const string& stId)
        : room_id(rId), showtime_id(stId) {}
};

class ShowtimeSeatRepository {
private:
    string filePath;
    DLL<ShowtimeSeatStatus> statusList;

    // Helper: Parse booked seats string "A1, B2, C3" -> DLL<string>
    DLL<string> parseBookedSeats(const string& bookedStr);

    // Helper: Convert DLL<string> -> "A1, B2, C3"
    string formatBookedSeats(const DLL<string>& seats);

public:
    ShowtimeSeatRepository(const string& path = "../data/RoomStatusAtShowtime.txt");

    // Load dữ liệu từ file
    void loadFromFile();

    // Lấy danh sách ghế đã đặt cho 1 suất chiếu cụ thể
    DLL<string> getBookedSeats(const string& showtimeId, const string& roomId);

    // Thêm ghế đã đặt mới (khi user xác nhận đặt vé)
    void addBookedSeats(const string& showtimeId, const string& roomId, const DLL<string>& newSeats);

    // Lưu toàn bộ dữ liệu về file
    void saveToFile();
};
