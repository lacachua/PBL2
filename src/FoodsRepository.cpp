#include "FoodsRepository.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::string> FoodsRepository::parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
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

std::vector<FoodItem> FoodsRepository::loadAll(const std::string& path) {
    std::vector<FoodItem> items;
    
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[FoodsRepo] Cannot open: " << path << std::endl;
        return items;
    }
    
    std::string line;
    bool headerSkipped = false;
    
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }
        
        if (line.empty()) continue;
        
        auto fields = parseCsvLine(line);
        if (fields.size() >= 5) {
            FoodItem item;
            item.item_id = fields[0];
            item.name = fields[1];
            try {
                item.price = std::stoi(fields[2]);
            } catch (...) {
                item.price = 0;
            }
            item.category = fields[3];
            item.image_path = fields[4];
            item.quantity = 0;
            
            items.push_back(item);
        }
    }
    
    file.close();
    std::cout << "[FoodsRepo] Loaded " << items.size() << " food items" << std::endl;
    return items;
}
