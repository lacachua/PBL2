#pragma once
#include <string>
#include <vector>

struct RoomLayout {
    int rows;
    int cols;
    
    RoomLayout(int r = 9, int c = 9) : rows(r), cols(c) {}
};

class RoomLayoutRepository {
public:
    /**
     * Load or create room layout from rooms_layout.csv
     * If room_id not found, append with default rows x cols
     * 
     * @param path Path to rooms_layout.csv
     * @param room_id Room ID (e.g., "Phong1", "Phong2", ...)
     * @param defaultRows Default rows if room not found (default: 9)
     * @param defaultCols Default cols if room not found (default: 9)
     * @return RoomLayout with rows and cols
     */
    static RoomLayout loadOrCreate(
        const std::string& path,
        const std::string& room_id,
        int defaultRows = 9,
        int defaultCols = 9
    );

private:
    // Helper: Parse CSV line
    static std::vector<std::string> parseCsvLine(const std::string& line);
};
