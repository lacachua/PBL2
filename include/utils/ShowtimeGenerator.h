#pragma once
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class ShowtimeGenerator {
public:
    // Generate showtimes for next N days starting from today
    static void generateShowtimesFile(const string& outputPath, int numDays = 30);
    
private:
    struct TimeSlot {
        string time;
        int price;
    };
    
    // Movie schedules - each movie gets specific time slots
    static vector<TimeSlot> getTimeSlotsForMovie(int movieIndex);
    
    // Get room ID for movie (each movie has assigned rooms)
    static string getRoomForMovie(int movieIndex, int slotIndex);
    
    // Format date as YYYY-MM-DD
    static string formatDate(tm* date);
    
    // Generate showtime ID
    static string generateShowtimeId(const string& date, int counter);
};
