#include "services/ShowtimeCleanupService.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

namespace {
// tickets.txt has: ticket_id|showtime_id|title|date|time|...
bool parseTicketLine(const std::string& line, std::string& outShowtimeId, std::string& outDateYyyyMmDd) {
    if (line.empty()) return false;
    std::stringstream ss(line);
    std::string ticketId;
    if (!std::getline(ss, ticketId, '|')) return false;
    if (!std::getline(ss, outShowtimeId, '|')) return false;
    std::string title;
    if (!std::getline(ss, title, '|')) return false;
    if (!std::getline(ss, outDateYyyyMmDd, '|')) return false;
    return !outShowtimeId.empty() && outDateYyyyMmDd.size() >= 10;
}
}

// Static members initialization
deque<MovieData> ShowtimeCleanupService::movieQueue;
vector<ShowtimeData> ShowtimeCleanupService::tempShowtimes;

void ShowtimeCleanupService::maintainShowtimes(const string& showtimesPath, int daysToGenerate) {
    // Hard requirement: only keep showtimes for today and the next 4 days.
    // Also remove any expired showtimes (past time).
    const int windowDays = max(1, min(daysToGenerate, 5));

    time_t now = time(nullptr);
    tm* nowTm = localtime(&now);
    const string todayStr = formatDate(nowTm);
    const int currentTimeMinutes = getCurrentTimeInMinutes();

    const time_t endTime = now + (static_cast<time_t>(windowDays - 1) * 24 * 60 * 60);
    tm* endTm = localtime(&endTime);
    const string endStr = formatDate(endTm);

    // Load locked showtime IDs (future tickets) so we never delete/modify booked future showtimes.
    const unordered_set<string> lockedShowtimeIds = loadLockedShowtimeIdsFromTickets("../data/tickets.txt", todayStr);

    // Load valid movies once; used to filter out invalid movie IDs from existing showtimes
    vector<MovieData> movies = loadMovies("../data/movies.txt");
    unordered_set<string> validMovieIds;
    validMovieIds.reserve(movies.size());
    for (const auto& movie : movies) {
        if (!movie.id.empty()) {
            validMovieIds.insert(movie.id);
        }
    }

    vector<ShowtimeData> existingShowtimes = loadShowtimes(showtimesPath);
    const size_t originalCount = existingShowtimes.size();

    // Keep only locked showtimes within the window; regenerate everything else.
    vector<ShowtimeData> lockedKept;
    lockedKept.reserve(existingShowtimes.size());
    for (const auto& st : existingShowtimes) {
        if (st.date < todayStr || st.date > endStr) continue;
        if (lockedShowtimeIds.count(st.showtime_id) == 0) continue;
        lockedKept.push_back(st);
    }

    // If there are no movies/rooms, just save locked ones.
    vector<string> rooms = loadRooms("../data/rooms.txt");
    if (movies.empty() || rooms.empty()) {
        saveShowtimes(showtimesPath, lockedKept);
        return;
    }

    // Build movie lookup and derive status relative to today based on start/end dates.
    auto toYmd = [](const string& ddmmyyyy) -> string {
        string dd, mm, yy;
        stringstream ss(ddmmyyyy);
        getline(ss, dd, '/');
        getline(ss, mm, '/');
        getline(ss, yy, '/');
        if (dd.empty() || mm.empty() || yy.empty()) return "";
        if (dd.size() == 1) dd = "0" + dd;
        if (mm.size() == 1) mm = "0" + mm;
        return yy + "-" + mm + "-" + dd;
    };
    time_t now2 = time(nullptr);
    const time_t future4 = now2 + (static_cast<time_t>(4) * 24 * 60 * 60);
    tm* future4Tm = localtime(&future4);
    const string todayPlus4 = formatDate(future4Tm);

    vector<MovieData> playable;
    playable.reserve(movies.size());
    for (auto m : movies) {
        if (m.end_date.empty()) {
            // Backfill end_date if missing
            // Reuse helper already available in this file
            // (status will be derived below)
        }

        // Derive status from dates
        string startYmd = toYmd(m.release_date);
        string endYmd = toYmd(m.end_date);
        if (startYmd.empty() || endYmd.empty()) {
            // If bad date, treat as currently showing
            m.status = "Đang chiếu";
        } else if (todayStr < startYmd) {
            m.status = "Sắp chiếu";
        } else if (todayStr >= endYmd) {
            m.status = "Ngừng chiếu";
        } else {
            m.status = "Đang chiếu";
        }

        if (m.status == "Ngừng chiếu") continue;
        playable.push_back(m);
    }

    // Initialize Round-Robin queue stable by status priority then id.
    stable_sort(playable.begin(), playable.end(), [](const MovieData& a, const MovieData& b) {
        int pa = statusPriority(a.status);
        int pb = statusPriority(b.status);
        if (pa != pb) return pa > pb;
        return a.id < b.id;
    });
    movieQueue.clear();
    for (const auto& m : playable) movieQueue.push_back(m);

    // Group locked showtimes by date+room and sort by time.
    struct LockedBlock { int start; int end; ShowtimeData st; };
    unordered_map<string, vector<LockedBlock>> lockedByRoomDate;
    lockedByRoomDate.reserve(lockedKept.size() * 2);
    unordered_set<string> usedShowtimeIds;
    usedShowtimeIds.reserve(lockedKept.size() * 2);

    // Duration lookup by movie_id (fallback 120)
    unordered_map<string, int> durationById;
    durationById.reserve(movies.size());
    for (const auto& m : movies) durationById[m.id] = (m.duration > 0 ? m.duration : 120);

    for (const auto& st : lockedKept) {
        int start = timeToMinutes(st.time);
        int dur = 120;
        auto it = durationById.find(st.movie_id);
        if (it != durationById.end()) dur = it->second;
        int end = start + dur;
        string key = st.date + "|" + st.room_id;
        lockedByRoomDate[key].push_back({start, end, st});
        usedShowtimeIds.insert(st.showtime_id);
    }
    for (auto& kv : lockedByRoomDate) {
        auto& vec = kv.second;
        sort(vec.begin(), vec.end(), [](const LockedBlock& a, const LockedBlock& b) {
            return a.start < b.start;
        });
    }

    vector<ShowtimeData> regenerated;
    regenerated.reserve(rooms.size() * 40);

    for (int dayOffset = 0; dayOffset < windowDays; dayOffset++) {
        time_t futureTime = now + (static_cast<time_t>(dayOffset) * 24 * 60 * 60);
        tm* futureTm = localtime(&futureTime);
        const string dateStr = formatDate(futureTm);
        const bool isToday = (dateStr == todayStr);

        // Determine eligible movies for THIS show date based on rule:
        // - "Đang chiếu": always eligible if show date in [start,end)
        // - "Sắp chiếu": only if start_date within next 4 days AND show date >= start_date
        vector<MovieData> dayEligible;
        dayEligible.reserve(playable.size());
        for (const auto& m : playable) {
            string startYmd = toYmd(m.release_date);
            string endYmd = toYmd(m.end_date);
            if (!startYmd.empty() && dateStr < startYmd) {
                // show date before start
                continue;
            }
            if (!endYmd.empty() && dateStr >= endYmd) {
                // show date at/after end
                continue;
            }

            if (m.status == "Đang chiếu") {
                dayEligible.push_back(m);
            } else if (m.status == "Sắp chiếu") {
                // start date must be within next 4 days
                if (!startYmd.empty() && startYmd <= todayPlus4) {
                    dayEligible.push_back(m);
                }
            }
        }
        if (dayEligible.empty()) continue;

        // Re-init queue each day for fairness across days
        stable_sort(dayEligible.begin(), dayEligible.end(), [](const MovieData& a, const MovieData& b) {
            int pa = statusPriority(a.status);
            int pb = statusPriority(b.status);
            if (pa != pb) return pa > pb;
            return a.id < b.id;
        });
        movieQueue.clear();
        for (const auto& m : dayEligible) movieQueue.push_back(m);

        int dailyCounter = getNextCounter(lockedKept, dateStr);

        // Seed tempShowtimes with ALL locked showtimes for this date (cross-room conflict checking)
        tempShowtimes.clear();
        for (const auto& st : lockedKept) {
            if (st.date == dateStr) tempShowtimes.push_back(st);
        }
        for (size_t r = 0; r < rooms.size(); r++) {
            const string& roomId = rooms[r];
            const int endDayMinute = END_HOUR * 60;
            int baseMinute = START_HOUR * 60 + (static_cast<int>(r) * ROOM_OFFSET);
            int currentMinute = roundUpToNext10(baseMinute);

            if (isToday) {
                int minStartMinute = roundUpToNext10(currentTimeMinutes + 30);
                if (currentMinute < minStartMinute) currentMinute = minStartMinute;
            }

            string roomDateKey = dateStr + "|" + roomId;
            const auto itBlocks = lockedByRoomDate.find(roomDateKey);
            const vector<LockedBlock>* blocks = (itBlocks == lockedByRoomDate.end()) ? nullptr : &itBlocks->second;
            size_t blockIndex = 0;

            while (currentMinute < endDayMinute) {
                // Skip over locked blocks if current is inside/at them
                if (blocks) {
                    while (blockIndex < blocks->size() && (*blocks)[blockIndex].end + BUFFER_MINUTES <= currentMinute) {
                        blockIndex++;
                    }
                    if (blockIndex < blocks->size()) {
                        const auto& b = (*blocks)[blockIndex];
                        if (currentMinute >= b.start && currentMinute < b.end + BUFFER_MINUTES) {
                            currentMinute = roundUpToNext10(b.end + BUFFER_MINUTES);
                            continue;
                        }
                    }
                }

                MovieData movie = selectNextMovie(currentMinute, roomId, dateStr);
                if (movie.id.empty()) break;

                int showtimeEndMinute = currentMinute + movie.duration;
                if (showtimeEndMinute > endDayMinute) break;

                // Ensure it doesn't overlap next locked block in this room
                if (blocks && blockIndex < blocks->size()) {
                    const auto& b = (*blocks)[blockIndex];
                    if (showtimeEndMinute + BUFFER_MINUTES > b.start) {
                        // Not enough space before locked showtime, jump to after locked
                        currentMinute = roundUpToNext10(b.end + BUFFER_MINUTES);
                        continue;
                    }
                }

                ShowtimeData st;
                string dateCompact = dateStr;
                dateCompact.erase(remove(dateCompact.begin(), dateCompact.end(), '-'), dateCompact.end());
                // Ensure unique showtime_id (avoid collisions with locked)
                while (true) {
                    stringstream ss;
                    ss << "SUATCHIEU_" << dateCompact << "_" << setfill('0') << setw(4) << dailyCounter++;
                    st.showtime_id = ss.str();
                    if (!usedShowtimeIds.count(st.showtime_id)) {
                        usedShowtimeIds.insert(st.showtime_id);
                        break;
                    }
                }
                st.movie_id = movie.id;
                st.room_id = roomId;
                st.date = dateStr;
                st.time = minutesToTime(currentMinute);
                st.price = getPriceByTime(currentMinute / 60);

                regenerated.push_back(st);
                tempShowtimes.push_back(st);

                currentMinute = roundUpToNext10(showtimeEndMinute + BUFFER_MINUTES);
            }
        }
    }

    // Merge locked + regenerated, sort, and write.
    vector<ShowtimeData> kept;
    kept.reserve(lockedKept.size() + regenerated.size());
    kept.insert(kept.end(), lockedKept.begin(), lockedKept.end());
    kept.insert(kept.end(), regenerated.begin(), regenerated.end());

    // Sort for stable output
    sort(kept.begin(), kept.end(), [&](const ShowtimeData& a, const ShowtimeData& b) {
        if (a.date != b.date) return a.date < b.date;
        if (a.room_id != b.room_id) return a.room_id < b.room_id;
        return timeToMinutes(a.time) < timeToMinutes(b.time);
    });

    const bool changed = (kept.size() != originalCount);
    if (changed) {
        // Clear RoomPanel cache to avoid stale schedule display.
        remove("../data/room_schedule_cache.txt");
    }

    saveShowtimes(showtimesPath, kept);
}

unordered_set<string> ShowtimeCleanupService::loadLockedShowtimeIdsFromTickets(const string& ticketsPath, const string& todayStr) {
    unordered_set<string> locked;
    ifstream file(ticketsPath);
    if (!file.is_open()) return locked;

    string line;
    getline(file, line); // header

    while (getline(file, line)) {
        string showtimeId;
        string ticketDate;
        if (!parseTicketLine(line, showtimeId, ticketDate)) continue;
        // lock only future/today tickets
        if (ticketDate >= todayStr) {
            locked.insert(showtimeId);
        }
    }
    return locked;
}

int ShowtimeCleanupService::statusPriority(const string& status) {
    // Higher is better.
    // Exclude "Ngừng chiếu" by returning -1.
    if (status == "Ngừng chiếu") return -1;
    if (status == "Đang chiếu") return 2;
    if (status == "Sắp chiếu") return 1;
    return 0; // unknown/empty
}

bool ShowtimeCleanupService::isReleaseOnOrBeforeShowDate(const string& releaseDateDdMmYyyy, const string& showDateYyyyMmDd) {
    // Very small helper: compare by converting dd/mm/yyyy -> yyyy-mm-dd
    if (releaseDateDdMmYyyy.size() < 8 || showDateYyyyMmDd.size() < 10) return true;
    string dd, mm, yyyy;
    {
        stringstream ss(releaseDateDdMmYyyy);
        getline(ss, dd, '/');
        getline(ss, mm, '/');
        getline(ss, yyyy, '/');
    }
    if (dd.empty() || mm.empty() || yyyy.empty()) return true;
    string normalized = yyyy + "-";
    if (mm.size() == 1) normalized += "0";
    normalized += mm + "-";
    if (dd.size() == 1) normalized += "0";
    normalized += dd;
    return normalized <= showDateYyyyMmDd;
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
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        MovieData movie;

        // Legacy 12 cols:
        // movie_id|title|age_rating|country|language|genres|duration_min|release_date|director|cast|synopsis|poster_path
        // Legacy 13 cols:
        // ...|poster_path|status
        // New 14 cols:
        // movie_id|title|age_rating|country|language|genres|duration_min|release_date|end_date|director|cast|synopsis|poster_path|status
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
        getline(ss, movie.release_date, '|'); // 7: release_date (start_date)

        // Try read next token; in new schema this is end_date, in legacy this is director.
        string maybeEndOrDirector;
        getline(ss, maybeEndOrDirector, '|');

        // Heuristic: end_date should contain '/' (dd/mm/yyyy). If not, treat as legacy.
        if (maybeEndOrDirector.find('/') != string::npos) {
            movie.end_date = maybeEndOrDirector;
            getline(ss, token, '|'); // director
            getline(ss, token, '|'); // cast
            getline(ss, token, '|'); // synopsis
            getline(ss, token, '|'); // poster_path
            if (getline(ss, token, '|')) movie.status = token; else movie.status.clear();
        } else {
            movie.end_date.clear();
            // legacy consumed director already
            getline(ss, token, '|'); // cast
            getline(ss, token, '|'); // synopsis
            getline(ss, token, '|'); // poster_path
            if (getline(ss, token, '|')) movie.status = token; else movie.status.clear();
        }

        if (!movie.id.empty()) {
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
        return {"", "", 120, "", "", ""};
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
