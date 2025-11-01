#include "MoviesRepository.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cctype>

// Static members
std::map<int, std::string> MoviesRepository::titleCache;
bool MoviesRepository::cacheLoaded = false;

// Helper: Parse CSV line
static std::vector<std::string> parseCsvLine(const std::string& line) {
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

void MoviesRepository::loadCache(const std::string& path) {
    if (cacheLoaded) return;
    
    std::cout << "[MoviesRepo] Loading movies from: " << path << std::endl;
    
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[MoviesRepo] Cannot open movies.csv" << std::endl;
        cacheLoaded = true;
        return;
    }
    
    std::string line;
    bool headerSkipped = false;
    int count = 0;
    
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
        if (fields.size() >= 2) {
            try {
                std::string film_id_str = fields[0];  // "F0001", "F0002", etc.
                std::string title = fields[1];
                
                // Extract numeric ID from film_id (e.g., "F0001" → 1)
                int movie_id = 0;
                for (char c : film_id_str) {
                    if (std::isdigit(static_cast<unsigned char>(c))) {
                        movie_id = movie_id * 10 + (c - '0');
                    }
                }
                
                if (movie_id > 0) {
                    titleCache[movie_id] = title;
                    count++;
                }
            } catch (...) {
                // Skip invalid lines
            }
        }
    }
    
    file.close();
    cacheLoaded = true;
    
    std::cout << "[MoviesRepo] Loaded " << count << " movies" << std::endl;
}

std::string MoviesRepository::getTitleById(const std::string& path, int movie_id) {
    if (!cacheLoaded) {
        loadCache(path);
    }
    
    auto it = titleCache.find(movie_id);
    if (it != titleCache.end()) {
        return it->second;
    }
    
    std::cout << "[MoviesRepo] Movie not found: " << movie_id << std::endl;
    return "Unknown Movie";
}

void MoviesRepository::clearCache() {
    titleCache.clear();
    cacheLoaded = false;
}
