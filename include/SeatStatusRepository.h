#pragma once
#include <string>
#include <vector>

// ✅ Seat layout structure
struct SeatLayout {
    int rows;
    int cols;
    std::string seat_bits;  // Bitstring: '1'=available, '0'=booked
    long long updated_at_epoch;
    
    SeatLayout() : rows(9), cols(9), seat_bits(""), updated_at_epoch(0) {}
    SeatLayout(int r, int c, const std::string& bits, long long epoch)
        : rows(r), cols(c), seat_bits(bits), updated_at_epoch(epoch) {}
};

/**
 * Repository for managing seat status per showtime.
 * File format: seats_status.csv
 * Schema: showtime_key,movie_id,date,time,room,rows,cols,seat_bits,updated_at_epoch,version
 */
class SeatStatusRepository {
public:
    /**
     * Generate deterministic showtime key from showtime attributes.
     * Format: ST-{YYYYMMDD}-{HHMM}-{room_id}
     * Example: ST-20251024-1200-Phong1
     * NOTE: Unique by (date, time, room_id) - one movie per room per timeslot
     */
    static std::string makeShowtimeKey(
        const std::string& date,      // YYYY-MM-DD
        const std::string& time,      // HH:MM
        const std::string& room_id    // "Phong1", "Phong2", etc.
    );
    
    /**
     * Load or create seat layout for a showtime.
     * If showtime_key doesn't exist, creates new entry with all seats='1'.
     * 
     * @param path Path to seats_status.csv
     * @param showtime_key Unique deterministic key
     * @param room_id Room ID (e.g., "Phong1", "Phong2")
     * @param movie_id Movie ID (for CSV record)
     * @param date Date string YYYY-MM-DD
     * @param time Time string HH:MM
     * @param defaultRows Default rows if creating new
     * @param defaultCols Default cols if creating new
     * @return SeatLayout with rows, cols, seat_bits
     */
    static SeatLayout loadOrCreate(
        const std::string& path,
        const std::string& showtime_key,
        const std::string& room_id,
        int movie_id,
        const std::string& date,
        const std::string& time,
        int defaultRows,
        int defaultCols
    );
    
    /**
     * Save updated seat bits (atomic write).
     * Updates existing showtime_key or appends new entry.
     * 
     * @param path Path to seats_status.csv
     * @param showtime_key Unique key
     * @param seat_bits New seat bitstring
     * @return true if successful
     */
    static bool save(
        const std::string& path,
        const std::string& showtime_key,
        const std::string& seat_bits
    );
    
    /**
     * Helper: Slug room name (Phòng 1 → Phong1, Phòng 2 → Phong2)
     * Removes spaces and non-alphanumeric characters
     */
    static std::string slugifyRoom(const std::string& room);

private:
    // Helper: Parse CSV line respecting quotes
    static std::vector<std::string> parseCsvLine(const std::string& line);
    
    // Helper: Escape CSV field if needed
    static std::string escapeCsvField(const std::string& field);
};
