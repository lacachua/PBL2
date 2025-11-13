#include "UI/components/Admin/MovieRepository.h"
#include <iostream>
#include <iomanip>

AdminMovieRepository::AdminMovieRepository(const string& path) : filePath(path) {
    loadFromFile();
}

vector<string> AdminMovieRepository::splitString(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

string AdminMovieRepository::generateNewId() {
    if (data.empty()) {
        return "F0001";
    }
    
    string lastId = data.back()[0];
    if (lastId.length() >= 2 && lastId[0] == 'F') {
        int num = stoi(lastId.substr(1)) + 1;
        stringstream ss;
        ss << "F" << setfill('0') << setw(4) << num;
        return ss.str();
    }
    
    return "F0001";
}

void AdminMovieRepository::loadFromFile() {
    data.clear();
    
    ifstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    string line;
    // Skip header
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Remove BOM if present
        if (line.size() >= 3 && 
            (unsigned char)line[0] == 0xEF && 
            (unsigned char)line[1] == 0xBB && 
            (unsigned char)line[2] == 0xBF) {
            line = line.substr(3);
        }
        
        vector<string> rowData = splitString(line, '|');
        if (rowData.size() >= 5) {  // Minimum required columns
            data.push_back(rowData);
        }
    }
    
    file.close();
}

void AdminMovieRepository::saveToFile() {
    ofstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    // Write header
    file << "movie_id|title|age_rating|country|language|genres|duration_min|release_date|director|cast|synopsis|poster_path|status\n";
    
    // Write data
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << "|";
        }
        file << "\n";
    }
    
    file.close();
}

void AdminMovieRepository::addRecord(const vector<string>& record) {
    vector<string> fullRecord = record;
    
    // If record doesn't have ID, generate it
    if (fullRecord.empty() || fullRecord[0].empty()) {
        fullRecord.insert(fullRecord.begin(), generateNewId());
    }
    
    // Ensure we have all required fields
    while (fullRecord.size() < 13) {
        fullRecord.push_back("");
    }
    
    data.push_back(fullRecord);
}

void AdminMovieRepository::updateRecord(int index, const vector<string>& record) {
    if (index < 0 || index >= data.size()) return;
    
    // Keep the ID from original record
    vector<string> updatedRecord = record;
    if (!data[index].empty()) {
        updatedRecord[0] = data[index][0];
    }
    
    data[index] = updatedRecord;
}

void AdminMovieRepository::deleteRecord(int index) {
    if (index < 0 || index >= data.size()) return;
    
    data.erase(data.begin() + index);
}

vector<string> AdminMovieRepository::getRecord(int index) const {
    if (index < 0 || index >= data.size()) return {};
    
    return data[index];
}
