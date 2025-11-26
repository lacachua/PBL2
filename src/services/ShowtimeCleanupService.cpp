#include "services/ShowtimeCleanupService.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

// Static members initialization
deque<MovieData> ShowtimeCleanupService::movieQueue;
vector<ShowtimeData> ShowtimeCleanupService::tempShowtimes;

void ShowtimeCleanupService::maintainShowtimes(const string& showtimesPath, int daysToGenerate) {
    // Step 1: Remove expired showtimes
    removeExpiredShowtimes(showtimesPath);
    
    // Step 2: Add new showtimes if needed
    addNewShowtimes(showtimesPath, daysToGenerate);
}

void ShowtimeCleanupService::forceRegenerate(const string& showtimesPath, int daysToGenerate) {
    cout << "Force regenerating all showtimes using Round-Robin algorithm..." << endl;
    
    // Xóa cache cũ của RoomPanel để tránh conflict
    remove("../data/room_schedule_cache.txt");
    
    // Load movies and rooms
    vector<MovieData> movies = loadMovies("../data/movies.txt");
    vector<string> rooms = loadRooms("../data/rooms.txt");
    
    if (movies.empty()) {
        cerr << "No movies found!" << endl;
        return;
    }
    if (rooms.empty()) {
        cerr << "No rooms found!" << endl;
        return;
    }
    
    cout << "Loaded " << movies.size() << " movies and " << rooms.size() << " rooms." << endl;
    
    // Initialize Round-Robin queue
    movieQueue.clear();
    for (const auto& movie : movies) {
        movieQueue.push_back(movie);
    }
    
    vector<ShowtimeData> allShowtimes;
    
    time_t now = time(nullptr);
    tm* nowTm = localtime(&now);
    string todayStr = formatDate(nowTm);
    int currentTimeMinutes = getCurrentTimeInMinutes();
    
    int globalCounter = 0;
    
    // Generate for each day
    for (int day = 0; day < daysToGenerate; day++) {
        time_t futureTime = now + (day * 24 * 60 * 60);
        tm* futureTm = localtime(&futureTime);
        string dateStr = formatDate(futureTm);
        
        bool isToday = (dateStr == todayStr);
        
        int dailyCounter = 1;
        
        // Clear temp showtimes for this day
        tempShowtimes.clear();
        
        // Generate showtimes for each room
        for (size_t r = 0; r < rooms.size(); r++) {
            string roomId = rooms[r];
            
            // Offset mỗi phòng để phim khởi chiếu lệch nhau
            int baseMinute = START_HOUR * 60 + (static_cast<int>(r) * ROOM_OFFSET);
            
            // Làm tròn lên bội số của 10
            int currentMinute = roundUpToNext10(baseMinute);
            int endDayMinute = END_HOUR * 60;
            
            // Nếu là hôm nay, bắt đầu từ giờ hiện tại (làm tròn lên)
            if (isToday) {
                int minStartMinute = roundUpToNext10(currentTimeMinutes + 30); // Ít nhất 30 phút từ bây giờ
                if (currentMinute < minStartMinute) {
                    currentMinute = minStartMinute;
                }
            }
            
            // Fill room with showtimes until end of day
            while (currentMinute < endDayMinute) {
                MovieData movie = selectNextMovie(currentMinute, roomId, dateStr);
                if (movie.id.empty()) break;
                
                int showtimeEndMinute = currentMinute + movie.duration;
                
                if (showtimeEndMinute > endDayMinute) break;
                
                // Create showtime
                globalCounter++;
                
                ShowtimeData st;
                
                string dateCompact = dateStr;
                dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
                stringstream ss;
                ss << "SUATCHIEU_" << dateCompact << "_" 
                   << setfill('0') << setw(4) << dailyCounter++;
                st.showtime_id = ss.str();
                
                st.movie_id = movie.id;
                st.room_id = roomId;
                st.date = dateStr;
                st.time = minutesToTime(currentMinute);
                st.price = getPriceByTime(currentMinute / 60);
                
                allShowtimes.push_back(st);
                tempShowtimes.push_back(st);
                
                // Move to next slot (làm tròn lên bội số của 10)
                currentMinute = roundUpToNext10(showtimeEndMinute + BUFFER_MINUTES);
            }
        }
    }
    
    // Save all showtimes
    saveShowtimes(showtimesPath, allShowtimes);
    
    cout << "Generated " << globalCounter << " showtimes for " << daysToGenerate << " days." << endl;
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

// ==================== DATA LOADING ====================

vector<MovieData> ShowtimeCleanupService::loadMovies(const string& moviesPath) {
    vector<MovieData> movies;
    ifstream file(moviesPath);
    if (!file.is_open()) {
        cerr << "Cannot open movies file: " << moviesPath << endl;
        return movies;
    }
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        MovieData movie;
        
        getline(ss, movie.id, '|');        // 0: movie_id
        getline(ss, movie.title, '|');     // 1: title
        getline(ss, token, '|');           // 2: age_rating (skip)
        getline(ss, token, '|');           // 3: country (skip)
        getline(ss, token, '|');           // 4: language (skip)
        getline(ss, token, '|');           // 5: genres (skip)
        getline(ss, token, '|');           // 6: duration_min
        try {
            movie.duration = stoi(token);
        } catch (...) {
            movie.duration = 120;
        }
        getline(ss, token, '|');           // 7: release_date (skip)
        getline(ss, token, '|');           // 8: director (skip)
        getline(ss, token, '|');           // 9: cast (skip)
        getline(ss, token, '|');           // 10: synopsis (skip)
        getline(ss, token, '|');           // 11: poster_path (skip)
        getline(ss, movie.status, '|');    // 12: status
        
        // Chỉ lấy phim "Đang chiếu" hoặc "Sắp chiếu"
        if (movie.status.find("chiếu") != string::npos || 
            movie.status.find("chieu") != string::npos ||
            movie.status == "Đang chiếu" || movie.status == "Sắp chiếu") {
            movies.push_back(movie);
        }
    }
    
    file.close();
    return movies;
}

vector<string> ShowtimeCleanupService::loadRooms(const string& roomsPath) {
    vector<string> rooms;
    ifstream file(roomsPath);
    if (!file.is_open()) {
        cerr << "Cannot open rooms file: " << roomsPath << endl;
        return rooms;
    }
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        stringstream ss(line);
        string roomId;
        getline(ss, roomId, '|');
        if (!roomId.empty()) {
            rooms.push_back(roomId);
        }
    }
    
    file.close();
    return rooms;
}

// ==================== ROUND-ROBIN ALGORITHM ====================

bool ShowtimeCleanupService::hasConflict(const string& movieId, int startMinute, int endMinute,
                                          const string& excludeRoom, const string& date) {
    for (const auto& st : tempShowtimes) {
        if (st.date == date && st.movie_id == movieId && st.room_id != excludeRoom) {
            int stStart = timeToMinutes(st.time);
            int stEnd = stStart + 150; // ~2.5h window for safety
            
            if (!(endMinute <= stStart || startMinute >= stEnd)) {
                return true;
            }
        }
    }
    return false;
}

MovieData ShowtimeCleanupService::selectNextMovie(int startMinute, const string& roomId, const string& date) {
    if (movieQueue.empty()) {
        return {"", "", 120, ""};
    }
    
    size_t queueSize = movieQueue.size();
    
    for (size_t attempt = 0; attempt < queueSize; attempt++) {
        MovieData movie = movieQueue.front();
        movieQueue.pop_front();
        
        int movieEndMinute = startMinute + movie.duration;
        
        if (!hasConflict(movie.id, startMinute, movieEndMinute, roomId, date)) {
            movieQueue.push_back(movie);
            return movie;
        }
        
        movieQueue.push_back(movie);
    }
    
    // Fallback
    MovieData movie = movieQueue.front();
    movieQueue.pop_front();
    movieQueue.push_back(movie);
    return movie;
}

int ShowtimeCleanupService::getPriceByTime(int hour) {
    if (hour < 12) return 65000;
    if (hour < 17) return 70000;
    if (hour < 20) return 75000;
    return 80000;
}

int ShowtimeCleanupService::timeToMinutes(const string& timeStr) {
    int hour = 0, minute = 0;
    size_t colonPos = timeStr.find(':');
    if (colonPos != string::npos) {
        hour = stoi(timeStr.substr(0, colonPos));
        minute = stoi(timeStr.substr(colonPos + 1));
    }
    return hour * 60 + minute;
}

string ShowtimeCleanupService::minutesToTime(int minutes) {
    int hour = minutes / 60;
    int minute = minutes % 60;
    
    stringstream ss;
    ss << setfill('0') << setw(2) << hour << ":"
       << setfill('0') << setw(2) << minute;
    return ss.str();
}

int ShowtimeCleanupService::roundUpToNext10(int minutes) {
    // Làm tròn phút lên bội số của 10
    // 09:03 (543) -> 09:10 (550)
    // 09:17 (557) -> 09:20 (560)
    // 09:51 (591) -> 10:00 (600)
    int remainder = minutes % 10;
    if (remainder == 0) {
        return minutes; // Đã là bội số của 10
    }
    return minutes + (10 - remainder);
}

int ShowtimeCleanupService::getCurrentTimeInMinutes() {
    time_t now = time(nullptr);
    tm* currentTm = localtime(&now);
    return currentTm->tm_hour * 60 + currentTm->tm_min;
}

// ==================== MAIN GENERATION (Round-Robin) ====================

void ShowtimeCleanupService::addNewShowtimes(const string& showtimesPath, int daysToGenerate) {
    vector<ShowtimeData> existingShowtimes = loadShowtimes(showtimesPath);
    
    // Get the latest date in existing showtimes
    string latestDate = getLatestDate(existingShowtimes);
    
    // Parse latest date
    tm latestTm = parseDate(latestDate);
    time_t latestTime = mktime(&latestTm);
    
    // Get current date and time
    time_t now = time(nullptr);
    tm* nowTm = localtime(&now);
    string todayStr = formatDate(nowTm);
    int currentTimeMinutes = getCurrentTimeInMinutes();
    
    tm currentTmCopy = *nowTm;
    currentTmCopy.tm_hour = 0;
    currentTmCopy.tm_min = 0;
    currentTmCopy.tm_sec = 0;
    time_t currentDay = mktime(&currentTmCopy);
    
    // Calculate days difference
    double daysDiff = difftime(latestTime, currentDay) / (24 * 60 * 60);
    
    // If we have less than 3 days of future showtimes, generate more
    if (daysDiff < 3) {
        // Load movies and rooms
        vector<MovieData> movies = loadMovies("../data/movies.txt");
        vector<string> rooms = loadRooms("../data/rooms.txt");
        
        if (movies.empty() || rooms.empty()) {
            cerr << "Cannot load movies or rooms for showtime generation" << endl;
            return;
        }
        
        // Initialize Round-Robin queue
        movieQueue.clear();
        for (const auto& movie : movies) {
            movieQueue.push_back(movie);
        }
        
        vector<ShowtimeData> newShowtimes;
        
        // Start from the day after latest date
        time_t startTime = latestTime + (24 * 60 * 60);
        
        // Generate showtimes for next N days using Round-Robin
        for (int day = 0; day < daysToGenerate; day++) {
            time_t futureTime = startTime + (day * 24 * 60 * 60);
            tm* futureTm = localtime(&futureTime);
            string dateStr = formatDate(futureTm);
            
            bool isToday = (dateStr == todayStr);
            
            int dailyCounter = getNextCounter(existingShowtimes, dateStr);
            
            // Clear temp showtimes for this day
            tempShowtimes.clear();
            
            // Generate showtimes for each room
            for (size_t r = 0; r < rooms.size(); r++) {
                string roomId = rooms[r];
                
                // Offset mỗi phòng để phim khởi chiếu lệch nhau
                int baseMinute = START_HOUR * 60 + (static_cast<int>(r) * ROOM_OFFSET);
                
                // Làm tròn lên bội số của 10
                int currentMinute = roundUpToNext10(baseMinute);
                int endDayMinute = END_HOUR * 60;
                
                // Nếu là hôm nay, bắt đầu từ giờ hiện tại (làm tròn lên)
                if (isToday) {
                    int minStartMinute = roundUpToNext10(currentTimeMinutes + 30);
                    if (currentMinute < minStartMinute) {
                        currentMinute = minStartMinute;
                    }
                }
                
                // Fill room with showtimes until end of day
                while (currentMinute < endDayMinute) {
                    MovieData movie = selectNextMovie(currentMinute, roomId, dateStr);
                    if (movie.id.empty()) break;
                    
                    int showtimeEndMinute = currentMinute + movie.duration;
                    
                    if (showtimeEndMinute > endDayMinute) break;
                    
                    // Create showtime
                    ShowtimeData st;
                    
                    string dateCompact = dateStr;
                    dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
                    stringstream ss;
                    ss << "SUATCHIEU_" << dateCompact << "_" 
                       << setfill('0') << setw(4) << dailyCounter++;
                    st.showtime_id = ss.str();
                    
                    st.movie_id = movie.id;
                    st.room_id = roomId;
                    st.date = dateStr;
                    st.time = minutesToTime(currentMinute);
                    st.price = getPriceByTime(currentMinute / 60);
                    
                    newShowtimes.push_back(st);
                    tempShowtimes.push_back(st);
                    
                    // Move to next slot (làm tròn lên bội số của 10)
                    currentMinute = roundUpToNext10(showtimeEndMinute + BUFFER_MINUTES);
                }
            }
        }
        
        // Append new showtimes to existing ones
        existingShowtimes.insert(existingShowtimes.end(), newShowtimes.begin(), newShowtimes.end());
        saveShowtimes(showtimesPath, existingShowtimes);
        
        cout << "Generated " << newShowtimes.size() << " new showtimes using Round-Robin algorithm." << endl;
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
