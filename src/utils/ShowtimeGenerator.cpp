#include "utils/ShowtimeGenerator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

void ShowtimeGenerator::generateShowtimesFile(const string& outputPath, int numDays) {
    ofstream file(outputPath);
    if (!file.is_open()) return;

    file << "showtime_id|movie_id|room_id|date|time|price\n";

    time_t now = time(nullptr);
    int showtimeCounter = 0;
    
    for (int day = 0; day < numDays; day++) {
        time_t futureTime = now + (day * 24 * 60 * 60);
        tm* futureDate = localtime(&futureTime);
        string dateStr = formatDate(futureDate);
        
        for (int movieIdx = 0; movieIdx < 10; movieIdx++) {
            string movieId = "F000" + to_string(movieIdx + 1);
            if (movieIdx >= 9) movieId = "F00" + to_string(movieIdx + 1);
            
            vector<TimeSlot> timeSlots = getTimeSlotsForMovie(movieIdx);
            
            // Generate showtimes for this movie
            for (size_t slotIdx = 0; slotIdx < timeSlots.size(); slotIdx++) {
                showtimeCounter++;
                string showtimeId = generateShowtimeId(dateStr, showtimeCounter);
                string roomId = getRoomForMovie(movieIdx, slotIdx);
                
                file << showtimeId << "|"
                     << movieId << "|"
                     << roomId << "|"
                     << dateStr << "|"
                     << timeSlots[slotIdx].time << "|"
                     << timeSlots[slotIdx].price << "\n";
            }
        }
    }
    
    file.close();
}

vector<ShowtimeGenerator::TimeSlot> ShowtimeGenerator::getTimeSlotsForMovie(int movieIndex) {
    // Each movie gets 3 time slots per day with varying times to avoid conflicts
    vector<vector<TimeSlot>> movieSchedules = {
        // F0001
        {{"09:00", 65000}, {"14:30", 75000}, {"20:00", 80000}},
        // F0002
        {{"10:30", 70000}, {"16:00", 75000}, {"21:30", 80000}},
        // F0003
        {{"13:00", 70000}, {"18:30", 80000}, {"22:00", 80000}},
        // F0004
        {{"11:00", 65000}, {"15:30", 75000}, {"19:00", 80000}},
        // F0005
        {{"09:30", 70000}, {"14:00", 75000}, {"20:30", 80000}},
        // F0006
        {{"10:00", 65000}, {"13:30", 70000}, {"17:00", 75000}},
        // F0007
        {{"11:30", 70000}, {"17:30", 80000}, {"23:00", 80000}},
        // F0008
        {{"09:00", 65000}, {"13:00", 70000}, {"19:30", 80000}},
        // F0009
        {{"10:30", 70000}, {"15:00", 75000}, {"21:00", 80000}},
        // F0010
        {{"09:30", 65000}, {"14:00", 75000}, {"22:30", 80000}}
    };
    
    // Alternate schedules for variety across days
    if (movieIndex % 2 == 1) {
        // Shift times by 30 minutes for odd-indexed movies on odd days
        vector<TimeSlot> altSchedule = {
            {movieSchedules[movieIndex][0].time, movieSchedules[movieIndex][0].price},
            {movieSchedules[movieIndex][1].time, movieSchedules[movieIndex][1].price},
            {movieSchedules[movieIndex][2].time, movieSchedules[movieIndex][2].price}
        };
        return altSchedule;
    }
    
    return movieSchedules[movieIndex];
}

string ShowtimeGenerator::getRoomForMovie(int movieIndex, int slotIndex) {
    // Assign rooms to movies (movies rotate through rooms)
    // F0001-F0006 get dedicated rooms, F0007-F0010 share rooms
    int roomAssignment[] = {1, 2, 3, 4, 5, 6, 1, 2, 3, 4};
    int roomNum = roomAssignment[movieIndex];
    
    stringstream ss;
    ss << "PHONG_00" << roomNum;
    return ss.str();
}

string ShowtimeGenerator::formatDate(tm* date) {
    stringstream ss;
    ss << (date->tm_year + 1900) << "-"
       << setfill('0') << setw(2) << (date->tm_mon + 1) << "-"
       << setfill('0') << setw(2) << date->tm_mday;
    return ss.str();
}

string ShowtimeGenerator::generateShowtimeId(const string& date, int counter) {
    // Remove dashes from date
    string dateCompact = date;
    dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
    
    stringstream ss;
    ss << "SUATCHIEU_" << dateCompact << "_" 
       << setfill('0') << setw(4) << counter;
    return ss.str();
}
