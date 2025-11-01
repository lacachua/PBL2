#include "SeatStatusRepository.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace fs = std::filesystem;

// ✅ Helper: Slugify room name (Phòng 1 → Phong1, Phòng 2 → Phong2)
std::string SeatStatusRepository::slugifyRoom(const std::string& room) {
    std::string slug;
    for (char c : room) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            slug += c;
        }
    }
    return slug;
}

// ✅ Generate deterministic showtime key
// Unique by (date, time, room_id) - one movie per room per timeslot
std::string SeatStatusRepository::makeShowtimeKey(
    const std::string& date,  // YYYY-MM-DD
    const std::string& time,  // HH:MM
    const std::string& room_id  // Already slugified like "Phong1"
) {
    // Extract YYYYMMDD from date
    std::string dateCompact = date;
    dateCompact.erase(std::remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
    
    // Extract HHMM from time
    std::string timeCompact = time;
    timeCompact.erase(std::remove(timeCompact.begin(), timeCompact.end(), ':'), timeCompact.end());
    
    // Format: ST-{YYYYMMDD}-{HHMM}-{room_id}
    std::ostringstream oss;
    oss << "ST-" << dateCompact << "-" << timeCompact << "-" << room_id;
    
    std::cout << "[SeatStatus] Generated key: " << oss.str() 
              << " (date=" << date << ", time=" << time << ", room=" << room_id << ")" << std::endl;
    
    return oss.str();
}

// ✅ Helper: Parse CSV line respecting quoted fields
std::vector<std::string> SeatStatusRepository::parseCsvLine(const std::string& line) {
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

// ✅ Helper: Escape CSV field
std::string SeatStatusRepository::escapeCsvField(const std::string& field) {
    bool needsQuotes = field.find(',') != std::string::npos ||
                       field.find('"') != std::string::npos ||
                       field.find('\n') != std::string::npos;
    
    if (!needsQuotes) return field;
    
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') escaped += "\"\"";
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

// ✅ Load or create seat layout
SeatLayout SeatStatusRepository::loadOrCreate(
    const std::string& path,
    const std::string& showtime_key,
    const std::string& room_id,
    int movie_id,
    const std::string& date,
    const std::string& time,
    int defaultRows,
    int defaultCols
) {
    std::cout << "[SeatStatus] loadOrCreate: " << showtime_key << " (room=" << room_id << ")" << std::endl;
    
    // ✅ Ensure parent directory exists
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    // Ensure file exists with header
    if (!fs::exists(path)) {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (file.is_open()) {
            file << "showtime_key,room_id,movie_id,date,time,rows,cols,seat_bits,updated_at_epoch,version\n";
            file.close();
            std::cout << "[SeatStatus] Created seats_status.csv with header" << std::endl;
        }
    }
    
    // Read file and search for showtime_key
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        // Return default if can't open
        std::string defaultBits(defaultRows * defaultCols, '1');
        return SeatLayout(defaultRows, defaultCols, defaultBits, std::time(nullptr));
    }
    
    std::string line;
    bool headerSkipped = false;
    
    while (std::getline(file, line)) {
        // Remove \r if present (CRLF handling)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCsvLine(line);
        if (fields.size() < 10) continue;
        
        // Check if this is our showtime
        if (fields[0] == showtime_key) {
            int rows = std::stoi(fields[5]);
            int cols = std::stoi(fields[6]);
            std::string seat_bits = fields[7];
            long long epoch = std::stoll(fields[8]);
            
            file.close();
            return SeatLayout(rows, cols, seat_bits, epoch);
        }
    }
    
    file.close();
    
    // Not found - create new entry
    std::cout << "[SeatStatus] Key not found, creating new entry with " << (defaultRows * defaultCols) << " seats" << std::endl;
    
    std::string defaultBits(defaultRows * defaultCols, '1');
    long long now = std::time(nullptr);
    
    // Append to file
    std::ofstream outFile(path, std::ios::out | std::ios::app | std::ios::binary);
    if (outFile.is_open()) {
        outFile << showtime_key << ","
                << room_id << ","
                << movie_id << ","
                << date << ","
                << time << ","
                << defaultRows << ","
                << defaultCols << ","
                << defaultBits << ","
                << now << ","
                << "1\n";  // version
        outFile.close();
    }
    
    return SeatLayout(defaultRows, defaultCols, defaultBits, now);
}

// ✅ Save updated seat bits
bool SeatStatusRepository::save(
    const std::string& path,
    const std::string& showtime_key,
    const std::string& seat_bits
) {
    if (!fs::exists(path)) {
        return false;  // File must exist
    }
    
    // Read all lines
    std::ifstream inFile(path, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) return false;
    
    std::vector<std::string> lines;
    std::string line;
    bool found = false;
    
    while (std::getline(inFile, line)) {
        // Remove \r if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Check if this is the showtime to update
        if (!line.empty() && line.find(showtime_key) == 0) {
            auto fields = parseCsvLine(line);
            if (fields.size() >= 10 && fields[0] == showtime_key) {
                // Update seat_bits and timestamp
                long long now = std::time(nullptr);
                
                std::ostringstream oss;
                oss << fields[0] << ","  // showtime_key
                    << fields[1] << ","  // movie_id
                    << fields[2] << ","  // date
                    << fields[3] << ","  // time
                    << fields[4] << ","  // room
                    << fields[5] << ","  // rows
                    << fields[6] << ","  // cols
                    << seat_bits << ","  // ✅ Updated seat_bits
                    << now << ","        // ✅ Updated timestamp
                    << fields[9];        // version
                
                lines.push_back(oss.str());
                found = true;
                continue;
            }
        }
        
        lines.push_back(line);
    }
    
    inFile.close();
    
    if (!found) {
        // If not found, something is wrong - should have been created by loadOrCreate
        return false;
    }
    
    // Atomic write: write to temp file then rename
    std::string tempPath = path + ".tmp";
    std::ofstream outFile(tempPath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) return false;
    
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    
    outFile.close();
    
    // Rename temp file to actual file
    try {
        fs::remove(path);
        fs::rename(tempPath, path);
        return true;
    } catch (...) {
        return false;
    }
}
