#pragma once
#include <string>
#include <vector>

// Room configuration from rooms.csv
struct RoomInfo {
    std::string room_id;      // R001, R002, ...
    std::string room_name;    // Phòng 1, Phòng 2, ...
    int rows;
    int cols;
    int total_seats;
    std::string screen_type;  // 2D, 3D, IMAX, 4DX
    std::string sound_system; // Dolby 7.1, Dolby Atmos, ...
    
    RoomInfo() : rows(9), cols(9), total_seats(81) {}
};

/**
 * Repository for managing cinema rooms
 * File: data/rooms.csv
 * Schema: room_id,room_name,rows,cols,total_seats,screen_type,sound_system
 */
class RoomsRepository {
public:
    /**
     * Load all rooms from CSV
     * @param path Path to rooms.csv
     * @return Vector of RoomInfo (use pointer iteration to avoid vector copy)
     */
    static std::vector<RoomInfo> loadAll(const std::string& path);
    
    /**
     * Get room by ID (efficient pointer-based search)
     * @param path Path to rooms.csv
     * @param room_id Room ID to search (e.g., "R001")
     * @return RoomInfo or default if not found
     */
    static RoomInfo getRoomById(const std::string& path, const std::string& room_id);
    
private:
    // Helper: Parse CSV line
    static std::vector<std::string> parseCsvLine(const std::string& line);
};
