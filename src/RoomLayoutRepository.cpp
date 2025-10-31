#include "RoomLayoutRepository.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// Helper: Parse CSV line
std::vector<std::string> RoomLayoutRepository::parseCsvLine(const std::string& line) {
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

RoomLayout RoomLayoutRepository::loadOrCreate(
    const std::string& path,
    const std::string& room_id,
    int defaultRows,
    int defaultCols
) {
    std::cout << "[RoomLayout] Loading room: " << room_id << std::endl;
    
    // Ensure parent directory exists
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    // Ensure file exists with header
    if (!fs::exists(path)) {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (file.is_open()) {
            file << "room_id,rows,cols\n";
            file.close();
            std::cout << "[RoomLayout] Created rooms_layout.csv with header" << std::endl;
        }
    }
    
    // Read file and search for room_id
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[RoomLayout] Cannot open file, using defaults" << std::endl;
        return RoomLayout(defaultRows, defaultCols);
    }
    
    std::string line;
    bool headerSkipped = false;
    
    while (std::getline(file, line)) {
        // Remove \r if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCsvLine(line);
        if (fields.size() >= 3 && fields[0] == room_id) {
            int rows = std::stoi(fields[1]);
            int cols = std::stoi(fields[2]);
            file.close();
            std::cout << "[RoomLayout] Found room: " << room_id << " (" << rows << "x" << cols << ")" << std::endl;
            return RoomLayout(rows, cols);
        }
    }
    
    file.close();
    
    // Not found - append new room
    std::cout << "[RoomLayout] Room not found, creating: " << room_id << " (" << defaultRows << "x" << defaultCols << ")" << std::endl;
    
    std::ofstream outFile(path, std::ios::out | std::ios::app | std::ios::binary);
    if (outFile.is_open()) {
        outFile << room_id << "," << defaultRows << "," << defaultCols << "\n";
        outFile.close();
    }
    
    return RoomLayout(defaultRows, defaultCols);
}
