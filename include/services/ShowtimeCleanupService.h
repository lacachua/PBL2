#pragma once
#include <string>
#include <vector>
#include <ctime>

using namespace std;

struct ShowtimeData {
    string showtime_id;
    string movie_id;
    string room_id;
    string date;      // Format: YYYY-MM-DD
    string time;      // Format: HH:MM
    int price;
};

class ShowtimeCleanupService {
public:
    // Main function: Clean expired showtimes and add new ones
    static void maintainShowtimes(const string& showtimesPath, int daysToGenerate = 7);
    
    // Remove showtimes that have passed current date/time
    static void removeExpiredShowtimes(const string& showtimesPath);
    
    // Add new showtimes for upcoming days if needed
    static void addNewShowtimes(const string& showtimesPath, int daysToGenerate);
    
private:
    // Load all showtimes from file
    static vector<ShowtimeData> loadShowtimes(const string& filepath);
    
    // Save showtimes back to file
    static void saveShowtimes(const string& filepath, const vector<ShowtimeData>& showtimes);
    
    // Check if a showtime has expired (date + time < current time)
    static bool isExpired(const ShowtimeData& showtime);
    
    // Parse date string (YYYY-MM-DD) to tm struct
    static tm parseDate(const string& dateStr);
    
    // Parse time string (HH:MM) and add to tm struct
    static void addTime(tm& datetime, const string& timeStr);
    
    // Get the latest date in showtimes
    static string getLatestDate(const vector<ShowtimeData>& showtimes);
    
    // Format date as YYYY-MM-DD
    static string formatDate(const tm* date);
    
    // Get next showtime counter for a given date
    static int getNextCounter(const vector<ShowtimeData>& showtimes, const string& date);
};
