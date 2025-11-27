#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "data-structures/DLL.h"
#include "utils/StringUtils.h"
#include "utils/FileUtils.h"

/**
 * @brief Model Room (Phòng chiếu) thống nhất cho toàn hệ thống
 * Format: room_id|room_name|seat_rows|seat_cols|total_seats
 */
struct Room {
    std::string id;             // ID phòng (VD: "PHONG_001")
    std::string name;           // Tên phòng (VD: "Phòng Chiếu 1")
    int rows = 0;               // Số hàng ghế
    int cols = 0;               // Số ghế mỗi hàng
    int totalSeats = 0;         // Tổng số ghế
    
    Room() = default;
    
    Room(const std::string& _id, const std::string& _name,
         int _rows, int _cols)
        : id(_id), name(_name), rows(_rows), cols(_cols) {
        totalSeats = _rows * _cols;
    }
    
    /**
     * @brief Tạo danh sách tên ghế (A1, A2, ..., B1, B2, ...)
     */
    std::vector<std::string> generateSeatNames() const {
        std::vector<std::string> seats;
        for (int r = 0; r < rows; r++) {
            char rowChar = 'A' + r;
            for (int s = 1; s <= cols; s++) {
                seats.push_back(std::string(1, rowChar) + std::to_string(s));
            }
        }
        return seats;
    }
};

/**
 * @brief Repository thống nhất quản lý Room
 */
class RoomRepository {
private:
    std::string filePath;
    std::vector<Room> rooms;
    
    /**
     * @brief Tạo ID room mới
     */
    std::string generateNewId() const {
        int maxId = 0;
        for (const auto& r : rooms) {
            // Parse PHONG_XXX format
            size_t pos = r.id.find('_');
            if (pos != std::string::npos) {
                try {
                    int id = std::stoi(r.id.substr(pos + 1));
                    if (id > maxId) maxId = id;
                } catch (...) {}
            }
        }
        maxId++;
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "PHONG_%03d", maxId);
        return std::string(buffer);
    }

public:
    explicit RoomRepository(const std::string& path = "../data/rooms.txt")
        : filePath(path) {
        loadFromFile();
    }
    
    // ===== LOAD/SAVE =====
    
    void loadFromFile() {
        rooms.clear();
        auto lines = FileUtils::readLines(filePath);
        
        bool isFirst = true;
        for (const auto& line : lines) {
            if (line.empty()) continue;
            
            auto cols = StringUtils::split(line, '|');
            
            if (isFirst && StringUtils::isHeaderRow(cols, "room_id")) {
                isFirst = false;
                continue;
            }
            isFirst = false;
            
            if (cols.size() >= 5) {
                Room r;
                r.id = cols[0];
                r.name = cols[1];
                try {
                    r.rows = std::stoi(cols[2]);
                    r.cols = std::stoi(cols[3]);
                    r.totalSeats = std::stoi(cols[4]);
                } catch (...) {
                    r.rows = 0;
                    r.cols = 0;
                    r.totalSeats = 0;
                }
                rooms.push_back(r);
            }
        }
    }
    
    void saveToFile() const {
        std::ofstream file(filePath, std::ios::trunc | std::ios::binary);
        if (!file.is_open()) return;
        
        // UTF-8 BOM
        const char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF };
        file.write(bom, 3);
        
        file << "room_id|room_name|seat_rows|seat_cols|total_seats\n";
        
        for (const auto& r : rooms) {
            file << r.id << "|" << r.name << "|" << r.rows << "|"
                 << r.cols << "|" << r.totalSeats << "\n";
        }
        file.close();
    }
    
    void reload() { loadFromFile(); }
    
    // ===== READ OPERATIONS =====
    
    const std::vector<Room>& getAll() const { return rooms; }
    
    DLL<Room> getAllAsDLL() const {
        DLL<Room> result;
        for (const auto& r : rooms) {
            result.push_back(r);
        }
        return result;
    }
    
    const Room* findById(const std::string& id) const {
        for (const auto& r : rooms) {
            if (r.id == id) return &r;
        }
        return nullptr;
    }
    
    const Room* findByName(const std::string& name) const {
        for (const auto& r : rooms) {
            if (r.name == name) return &r;
        }
        return nullptr;
    }
    
    int count() const { return static_cast<int>(rooms.size()); }
    
    /**
     * @brief Lấy dữ liệu dạng table cho EditableTable
     */
    std::vector<std::vector<std::string>> getAllAsTable() const {
        std::vector<std::vector<std::string>> result;
        for (const auto& r : rooms) {
            result.push_back({r.id, r.name, std::to_string(r.rows),
                             std::to_string(r.cols), std::to_string(r.totalSeats)});
        }
        return result;
    }
    
    std::vector<std::string> getAllNames() const {
        std::vector<std::string> result;
        for (const auto& r : rooms) {
            result.push_back(r.name);
        }
        return result;
    }
    
    std::vector<std::string> getAllIds() const {
        std::vector<std::string> result;
        for (const auto& r : rooms) {
            result.push_back(r.id);
        }
        return result;
    }
    
    // ===== WRITE OPERATIONS =====
    
    /**
     * @brief Thêm phòng mới
     */
    bool add(const Room& room) {
        Room r = room;
        if (r.id.empty()) {
            r.id = generateNewId();
        }
        if (r.totalSeats == 0) {
            r.totalSeats = r.rows * r.cols;
        }
        rooms.push_back(r);
        saveToFile();
        return true;
    }
    
    bool addFromRow(const std::vector<std::string>& row) {
        if (row.size() < 4) return false;
        Room r;
        r.id = row[0].empty() ? generateNewId() : row[0];
        r.name = row[1];
        try {
            r.rows = std::stoi(row[2]);
            r.cols = std::stoi(row[3]);
            r.totalSeats = row.size() > 4 ? std::stoi(row[4]) : r.rows * r.cols;
        } catch (...) {
            return false;
        }
        return add(r);
    }
    
    /**
     * @brief Cập nhật phòng
     */
    bool update(int index, const Room& room) {
        if (index < 0 || index >= static_cast<int>(rooms.size())) return false;
        rooms[index] = room;
        saveToFile();
        return true;
    }
    
    bool updateFromRow(int index, const std::vector<std::string>& row) {
        if (index < 0 || index >= static_cast<int>(rooms.size())) return false;
        if (row.size() < 4) return false;
        
        Room& r = rooms[index];
        r.id = row[0];
        r.name = row[1];
        try {
            r.rows = std::stoi(row[2]);
            r.cols = std::stoi(row[3]);
            r.totalSeats = row.size() > 4 ? std::stoi(row[4]) : r.rows * r.cols;
        } catch (...) {
            return false;
        }
        
        saveToFile();
        return true;
    }
    
    /**
     * @brief Xóa phòng theo index
     */
    bool remove(int index) {
        if (index < 0 || index >= static_cast<int>(rooms.size())) return false;
        rooms.erase(rooms.begin() + index);
        saveToFile();
        return true;
    }
    
    void deleteRecord(int index) { remove(index); }
};
