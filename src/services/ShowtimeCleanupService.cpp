#include "services/ShowtimeCleanupService.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

void ShowtimeCleanupService::maintainShowtimes(const string& showtimesPath, int daysToGenerate) {
    // Step 1: Remove expired showtimes
    removeExpiredShowtimes(showtimesPath);
    
    // Step 2: Add new showtimes if needed
    addNewShowtimes(showtimesPath, daysToGenerate);
}

void ShowtimeCleanupService::removeExpiredShowtimes(const string& showtimesPath) {
    vector<ShowtimeData> showtimes = loadShowtimes(showtimesPath);
    int originalCount = showtimes.size();
    
    // Filter out expired showtimes
    vector<ShowtimeData> validShowtimes;
    for (const auto& showtime : showtimes) 
        if (!isExpired(showtime)) validShowtimes.push_back(showtime);
    
    int removedCount = originalCount - validShowtimes.size();
    if (removedCount > 0) saveShowtimes(showtimesPath, validShowtimes);
}

void ShowtimeCleanupService::addNewShowtimes(const string& showtimesPath, int daysToGenerate) {
    vector<ShowtimeData> existingShowtimes = loadShowtimes(showtimesPath);
    
    // Get the latest date in existing showtimes
    string latestDate = getLatestDate(existingShowtimes);
    
    // Parse latest date
    tm latestTm = parseDate(latestDate);
    time_t latestTime = mktime(&latestTm);
    
    // Get current date
    time_t now = time(nullptr);
    tm* currentTm = localtime(&now);
    currentTm->tm_hour = 0;
    currentTm->tm_min = 0;
    currentTm->tm_sec = 0;
    time_t currentDay = mktime(currentTm);
    
    // Calculate days difference
    double daysDiff = difftime(latestTime, currentDay) / (24 * 60 * 60);
    
    // If we have less than 3 days of future showtimes, generate more
    if (daysDiff < 3) {
        int daysToAdd = daysToGenerate;
        
        vector<ShowtimeData> newShowtimes;
        
        // Start from the day after latest date
        time_t startTime = latestTime + (24 * 60 * 60);
        
        // Generate showtimes for next N days
        for (int day = 0; day < daysToAdd; day++) {
            time_t futureTime = startTime + (day * 24 * 60 * 60);
            tm* futureTm = localtime(&futureTime);
            string dateStr = formatDate(futureTm);
            
            int baseCounter = getNextCounter(existingShowtimes, dateStr);
            
            // For each of 10 movies (F0001 to F0010)
            for (int movieIdx = 0; movieIdx < 10; movieIdx++) {
                string movieId = "F000" + to_string(movieIdx + 1);
                if (movieIdx >= 9) movieId = "F00" + to_string(movieIdx + 1);
                
                // 3 time slots per movie
                vector<pair<string, int>> timeSlots;
                if (movieIdx == 0) timeSlots = {{"09:00", 65000}, {"14:30", 75000}, {"20:00", 80000}};
                else if (movieIdx == 1) timeSlots = {{"10:30", 70000}, {"16:00", 75000}, {"21:30", 80000}};
                else if (movieIdx == 2) timeSlots = {{"13:00", 70000}, {"18:30", 80000}, {"22:00", 80000}};
                else if (movieIdx == 3) timeSlots = {{"11:00", 65000}, {"15:30", 75000}, {"19:00", 80000}};
                else if (movieIdx == 4) timeSlots = {{"09:30", 70000}, {"14:00", 75000}, {"20:30", 80000}};
                else if (movieIdx == 5) timeSlots = {{"10:00", 65000}, {"13:30", 70000}, {"17:00", 75000}};
                else if (movieIdx == 6) timeSlots = {{"11:30", 70000}, {"17:30", 80000}, {"23:00", 80000}};
                else if (movieIdx == 7) timeSlots = {{"09:00", 65000}, {"13:00", 70000}, {"19:30", 80000}};
                else if (movieIdx == 8) timeSlots = {{"10:30", 70000}, {"15:00", 75000}, {"21:00", 80000}};
                else timeSlots = {{"09:30", 65000}, {"14:00", 75000}, {"22:30", 80000}};
                
                // Assign rooms
                int roomAssignment[] = {1, 2, 3, 4, 5, 6, 1, 2, 3, 4};
                string roomId = "PHONG_00" + to_string(roomAssignment[movieIdx]);
                
                // Generate showtimes for this movie
                for (const auto& slot : timeSlots) {
                    ShowtimeData st;
                    
                    // Generate showtime ID
                    string dateCompact = dateStr;
                    dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
                    stringstream ss;
                    ss << "SUATCHIEU_" << dateCompact << "_" 
                       << setfill('0') << setw(4) << baseCounter++;
                    st.showtime_id = ss.str();
                    
                    st.movie_id = movieId;
                    st.room_id = roomId;
                    st.date = dateStr;
                    st.time = slot.first;
                    st.price = slot.second;
                    
                    newShowtimes.push_back(st);
                }
            }
        }
        
        // Append new showtimes to existing ones
        existingShowtimes.insert(existingShowtimes.end(), newShowtimes.begin(), newShowtimes.end());
        saveShowtimes(showtimesPath, existingShowtimes);
    }
}

vector<ShowtimeData> ShowtimeCleanupService::loadShowtimes(const string& filepath) {
    vector<ShowtimeData> showtimes;
    ifstream file(filepath);
    if (!file.is_open()) return showtimes;
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        ShowtimeData st;
        
        getline(ss, st.showtime_id, '|');
        getline(ss, st.movie_id, '|');
        getline(ss, st.room_id, '|');
        getline(ss, st.date, '|');
        getline(ss, st.time, '|');
        
        string priceStr;
        getline(ss, priceStr, '|');
        try {
            st.price = stoi(priceStr);
        } catch (...) {
            st.price = 70000; // Default price
        }
        
        showtimes.push_back(st);
    }
    
    file.close();
    return showtimes;
}

void ShowtimeCleanupService::saveShowtimes(const string& filepath, const vector<ShowtimeData>& showtimes) {
    ofstream file(filepath);
    if (!file.is_open()) return;
    
    // Write header
    file << "showtime_id|movie_id|room_id|date|time|price\n";
    
    // Write all showtimes
    for (const auto& st : showtimes) {
        file << st.showtime_id << "|"
             << st.movie_id << "|"
             << st.room_id << "|"
             << st.date << "|"
             << st.time << "|"
             << st.price << "\n";
    }
    
    file.close();
}

bool ShowtimeCleanupService::isExpired(const ShowtimeData& showtime) {
    // Parse showtime date and time
    tm showtimeTm = parseDate(showtime.date);
    addTime(showtimeTm, showtime.time);
    time_t showtimeTimestamp = mktime(&showtimeTm);
    
    // Get current time
    time_t now = time(nullptr);
    
    // Showtime is expired if it's in the past
    return difftime(now, showtimeTimestamp) > 0;
}

tm ShowtimeCleanupService::parseDate(const string& dateStr) {
    tm date = {};
    stringstream ss(dateStr);
    string year, month, day;
    
    getline(ss, year, '-');
    getline(ss, month, '-');
    getline(ss, day, '-');
    
    date.tm_year = stoi(year) - 1900;
    date.tm_mon = stoi(month) - 1;
    date.tm_mday = stoi(day);
    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;
    
    return date;
}

void ShowtimeCleanupService::addTime(tm& datetime, const string& timeStr) {
    stringstream ss(timeStr);
    string hour, minute;
    
    getline(ss, hour, ':');
    getline(ss, minute, ':');
    
    datetime.tm_hour = stoi(hour);
    datetime.tm_min = stoi(minute);
    datetime.tm_sec = 0;
}

string ShowtimeCleanupService::getLatestDate(const vector<ShowtimeData>& showtimes) {
    if (showtimes.empty()) {
        // Return today's date
        time_t now = time(nullptr);
        return formatDate(localtime(&now));
    }
    
    string latestDate = showtimes[0].date;
    for (const auto& st : showtimes) {
        if (st.date > latestDate) {
            latestDate = st.date;
        }
    }
    
    return latestDate;
}

string ShowtimeCleanupService::formatDate(const tm* date) {
    stringstream ss;
    ss << (date->tm_year + 1900) << "-"
       << setfill('0') << setw(2) << (date->tm_mon + 1) << "-"
       << setfill('0') << setw(2) << date->tm_mday;
    return ss.str();
}

int ShowtimeCleanupService::getNextCounter(const vector<ShowtimeData>& showtimes, const string& date) {
    int maxCounter = 0;
    
    string dateCompact = date;
    dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
    string prefix = "SUATCHIEU_" + dateCompact + "_";
    
    for (const auto& st : showtimes) {
        if (st.showtime_id.find(prefix) == 0) {
            string counterStr = st.showtime_id.substr(prefix.length());
            try {
                int counter = stoi(counterStr);
                if (counter > maxCounter) {
                    maxCounter = counter;
                }
            } catch (...) {}
        }
    }
    
    return maxCounter + 1;
}
