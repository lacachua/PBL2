#include "RoomsRepository.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Helper: Parse CSV line
std::vector<std::string> RoomsRepository::parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    
    fields.push_back(current);
    return fields;
}

std::vector<RoomInfo> RoomsRepository::loadAll(const std::string& path) {
    std::vector<RoomInfo> rooms;
    std::ifstream file(path, std::ios::in | std::ios::binary);
    
    if (!file.is_open()) {
        std::cout << "[RoomsRepo] Cannot open: " << path << std::endl;
        return rooms;
    }
    
    std::string line;
    bool header_skipped = false;
    
    while (std::getline(file, line)) {
        // Remove \r if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCsvLine(line);
        if (fields.size() < 7) continue;
        
        RoomInfo room;
        room.room_id = fields[0];
        room.room_name = fields[1];
        room.rows = std::stoi(fields[2]);
        room.cols = std::stoi(fields[3]);
        room.total_seats = std::stoi(fields[4]);
        room.screen_type = fields[5];
        room.sound_system = fields[6];
        
        rooms.push_back(room);
    }
    
    file.close();
    std::cout << "[RoomsRepo] Loaded " << rooms.size() << " rooms" << std::endl;
    return rooms;
}

RoomInfo RoomsRepository::getRoomById(const std::string& path, const std::string& room_id) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    
    if (!file.is_open()) {
        return RoomInfo(); // Return default
    }
    
    std::string line;
    bool header_skipped = false;
    
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCsvLine(line);
        if (fields.size() < 7) continue;
        
        if (fields[0] == room_id) {
            RoomInfo room;
            room.room_id = fields[0];
            room.room_name = fields[1];
            room.rows = std::stoi(fields[2]);
            room.cols = std::stoi(fields[3]);
            room.total_seats = std::stoi(fields[4]);
            room.screen_type = fields[5];
            room.sound_system = fields[6];
            file.close();
            return room;
        }
    }
    
    file.close();
    return RoomInfo(); // Not found
}
