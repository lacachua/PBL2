#include "UI/components/TicketBooking/ShowtimeSection.h"
#include <ctime>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

// =======================
// Layout constants (khu vực hiển thị section suất chiếu)
// =======================
static constexpr float kViewX = 174.f;
static constexpr float kViewY = 220.f;

// Dòng "Chọn ngày chiếu" + "Chọn suất chiếu"
static constexpr float kTitleOffsetX = 20.f;
static constexpr float kTitleOffsetY = 10.f;

// Khối nút ngày
static constexpr float kDateStartX = 20.f;
static constexpr float kDateStartY = 70.f;
static constexpr float kDateW = 130.f;
static constexpr float kDateH = 40.f;
static constexpr float kDateSpacing = 40.f;

// Khối nút giờ
static constexpr float kTimeStartX = 20.f;
static constexpr float kTimeStartY = 200.f; // phía dưới tiêu đề "CHỌN SUẤT CHIẾU"
static constexpr float kTimeW = 100.f;
static constexpr float kTimeH = 40.f;
static constexpr float kTimeSpacingX = 15.f;
static constexpr float kTimeSpacingY = 15.f;
static constexpr int   kTimeCols = 6;

ShowtimeSection::ShowtimeSection(Font& f, const String& movieId, const DLL<Showtime>& all)
    : font(f),
      allShowtimes(all),
      filterMovieId(movieId),
      selectedDateIndex(0),
      selectedShowtimeIndex(-1)  // KHÔNG auto-select
{
    initializeDates();
    loadShowtimesForDate(0);
    
    // KHÔNG tự động chọn suất chiếu đầu tiên
    // User phải click chọn suất chiếu
}

void ShowtimeSection::initializeDates() {
    availableDates.clear();
    time_t now = time(nullptr);

    // Tạo 5 ngày kế tiếp (bao gồm hôm nay)
    for (int i = 0; i < 5; ++i) {
        time_t futureTime = now + (static_cast<time_t>(i) * 24 * 60 * 60);
        tm* futureDate = localtime(&futureTime);

        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", futureDate);
        availableDates.push_back(String(buffer));
    }
}

// Helper function để convert time string "HH:MM" thành phút
static int timeToMinutes(const String& timeStr) {
    string t = timeStr.toAnsiString();
    int hour = 0, minute = 0;
    size_t colonPos = t.find(':');
    if (colonPos != string::npos) {
        hour = stoi(t.substr(0, colonPos));
        minute = stoi(t.substr(colonPos + 1));
    }
    return hour * 60 + minute;
}

// Helper function để lấy ngày hôm nay dạng YYYY-MM-DD
static string getTodayDateStr() {
    time_t now = time(nullptr);
    tm* nowTm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", nowTm);
    return string(buffer);
}

// Helper function để lấy thời gian hiện tại dạng phút
static int getCurrentTimeMinutes() {
    time_t now = time(nullptr);
    tm* nowTm = localtime(&now);
    return nowTm->tm_hour * 60 + nowTm->tm_min;
}

void ShowtimeSection::loadShowtimesForDate(int dateIndex) {
    dateButtons.clear();
    timeButtons.clear();
    filteredShowtimes.clear();

    if (dateIndex < 0 || dateIndex >= availableDates.getSize()) return;

    // Lọc theo ngày + movieId (nếu có)
    String targetDate = availableDates[dateIndex];
    string targetDateStr = targetDate.toAnsiString();
    string todayStr = getTodayDateStr();
    bool isToday = (targetDateStr == todayStr);
    int currentMinutes = getCurrentTimeMinutes();
    
    // Thu thập vào vector tạm để sort
    vector<Showtime> tempShowtimes;
    for (int i = 0; i < allShowtimes.getSize(); ++i) {
        const auto& st = allShowtimes[i];
        if (st.date == targetDate) {
            if (filterMovieId.isEmpty() || st.movie_id == filterMovieId) {
                // Nếu là hôm nay, chỉ lấy suất chiếu chưa qua giờ
                if (isToday) {
                    int showtimeMinutes = timeToMinutes(st.time);
                    // Chỉ hiển thị suất chiếu còn ít nhất 10 phút trước giờ chiếu
                    if (showtimeMinutes > currentMinutes + 10) {
                        tempShowtimes.push_back(st);
                    }
                } else {
                    // Ngày khác thì lấy tất cả
                    tempShowtimes.push_back(st);
                }
            }
        }
    }
    
    // Sắp xếp theo giờ chiếu (tăng dần)
    sort(tempShowtimes.begin(), tempShowtimes.end(), [](const Showtime& a, const Showtime& b) {
        return timeToMinutes(a.time) < timeToMinutes(b.time);
    });
    
    // Đưa vào DLL
    for (const auto& st : tempShowtimes) {
        filteredShowtimes.push_back(st);
    }

    // Tạo các nút ngày (hiển thị dd - mm - yyyy)
    for (int i = 0; i < availableDates.getSize(); ++i) {
        string dateStr = availableDates[i].toAnsiString();
        int year = 0, month = 0, day = 0;
        char displayDate[20] = "";
        if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
            snprintf(displayDate, sizeof(displayDate), "%02d - %02d - %04d", day, month, year);
        }

        Button btn(font, String(displayDate), kDateW, kDateH, 18);
        // vị trí tạm (sẽ cộng kViewX, kViewY khi draw); ta set theo local của section:
        btn.setPosition({kDateStartX + i * (kDateW + kDateSpacing), kDateStartY});

        if (i == dateIndex) {
            btn.setFillColor(Color(20, 118, 172));
            btn.setTextColor(Color::White);
        } else {
            btn.setFillColor(Color(50, 50, 60));
            btn.setTextColor(Color::White);
        }
        btn.setOutlineThickness(2.f);
        btn.setOutlineColor(Color(100, 100, 110));
        dateButtons.push_back(btn);
    }

    // Tạo các nút giờ (label = time). Grid kTimeCols cột
    for (int i = 0; i < filteredShowtimes.getSize(); ++i) {
        int row = i / kTimeCols;
        int col = i % kTimeCols;

        const auto& st = filteredShowtimes[i];
        Button btn(font, st.time, kTimeW, kTimeH, 16);
        btn.setPosition({
            kTimeStartX + col * (kTimeW + kTimeSpacingX),
            (kTimeStartY + 60.f) + row * (kTimeH + kTimeSpacingY) // +60 để cách tiêu đề "CHỌN SUẤT CHIẾU"
        });

        // CHỈ highlight nếu đã được chọn trước đó
        if (i == selectedShowtimeIndex) {
            btn.setFillColor(Color(20, 118, 172));
            btn.setTextColor(Color::White);
        } else {
            btn.setFillColor(Color(40, 40, 50));
            btn.setTextColor(Color::White);
        }

        btn.setOutlineThickness(2.f);
        btn.setOutlineColor(Color(100, 100, 110));
        timeButtons.push_back(btn);
    }
}

void ShowtimeSection::handleClick(Vector2f mousePos, bool mousePressed) {
    if (!mousePressed) return;

    // Click cho nút ngày
    for (int i = 0; i < dateButtons.getSize(); ++i) {
        auto bounds = dateButtons[i].getGlobalBounds();
        // Dịch theo view của section
        bounds.position.x = kViewX + kDateStartX + i * (kDateW + kDateSpacing);
        bounds.position.y = kViewY + kDateStartY;

        if (bounds.contains(mousePos)) {
            selectedDateIndex = i;
            selectedShowtimeIndex = -1;
            loadShowtimesForDate(i);
            return;
        }
    }

    // Click cho nút giờ
    for (int i = 0; i < timeButtons.getSize(); ++i) {
        auto bounds = timeButtons[i].getGlobalBounds();
        // Dịch theo view của section
        bounds.position.x += kViewX;
        bounds.position.y += kViewY;

        if (bounds.contains(mousePos)) {
            selectedShowtimeIndex = i;
            // Tô lại trạng thái nút
            for (int j = 0; j < timeButtons.getSize(); ++j) {
                if (j == i) {
                    timeButtons[j].setFillColor(Color(20, 118, 172));
                    timeButtons[j].setTextColor(Color::White);
                } else {
                    timeButtons[j].setFillColor(Color(40, 40, 50));
                    timeButtons[j].setTextColor(Color::White);
                }
            }
            return;
        }
    }
}

void ShowtimeSection::draw(RenderWindow& window) {
    Text title(font, L"CHỌN NGÀY CHIẾU", 28);
    title.setFillColor(Color::White);
    title.setOutlineColor(Color(20, 118, 172));
    title.setOutlineThickness(2.f);
    title.setPosition({kViewX + kTitleOffsetX, kViewY + kTitleOffsetY});
    window.draw(title);

    // Vẽ các nút ngày
    for (int i = 0; i < dateButtons.getSize(); ++i) {
        Button& btn = dateButtons[i]; // Use reference instead of copy
        if (i == selectedDateIndex) {
            btn.setFillColor(Color(20, 118, 172));
            btn.setTextColor(Color::White);
        } else {
            btn.setFillColor(Color(40, 40, 50));
            btn.setTextColor(Color::White);
        }
        // Calculate absolute position (don't modify button's stored position)
        float absX = kViewX + kDateStartX + i * (kDateW + kDateSpacing);
        float absY = kViewY + kDateStartY;
        
        // Save original position
        Vector2f originalPos = btn.getPosition();
        btn.setPosition({absX, absY});
        btn.draw(window);
        // Restore original position
        btn.setPosition(originalPos);
    }

    // Nếu không có suất chiếu
    if (filteredShowtimes.getSize() == 0) {
        Text noShowtime(font, L"Không có suất chiếu cho ngày này.", 20);
        noShowtime.setFillColor(Color(200, 100, 100));
        noShowtime.setPosition({kViewX + 20.f, kViewY + 140.f});
        window.draw(noShowtime);
        return;
    }

    // Tiêu đề "CHỌN SUẤT CHIẾU"
    {
        Text stTitle(font, L"CHỌN SUẤT CHIẾU", 28);
        stTitle.setFillColor(Color::White);
        stTitle.setOutlineColor(Color(20, 118, 172));
        stTitle.setOutlineThickness(2.f);
        stTitle.setPosition({kViewX + 20.f, kViewY + kTimeStartY});
        window.draw(stTitle);
    }

    // Vẽ các nút giờ
    for (int i = 0; i < timeButtons.getSize(); ++i) {
        Button& btn = timeButtons[i]; // Use reference instead of copy
        // CHỈ highlight nếu đã chọn (selectedShowtimeIndex >= 0)
        if (i == selectedShowtimeIndex) {
            btn.setFillColor(Color(20, 118, 172));
            btn.setTextColor(Color::White);
        } else {
            btn.setFillColor(Color(40, 40, 50));
            btn.setTextColor(Color::White);
        }
        // Calculate absolute position (don't modify button's stored position)
        Vector2f originalPos = btn.getPosition();
        float absX = kViewX + originalPos.x;
        float absY = kViewY + originalPos.y;
        
        btn.setPosition({absX, absY});
        btn.draw(window);
        // Restore original position
        btn.setPosition(originalPos);
    }
}

// =======================
// Helper function để lấy tên phim từ movie_id
// =======================
static String getMovieTitleFromId(const String& movieId) {
    ifstream file("../data/movies.txt");
    if (!file.is_open()) return movieId;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string id, title;
        
        getline(ss, id, '|');
        getline(ss, title, '|');
        
        if (String(id) == movieId) {
            file.close();
            return String::fromUtf8(title.begin(), title.end());
        }
    }
    
    file.close();
    return movieId;
}

static String getRoomNameFromId(const String& roomId) {
    ifstream file("../data/rooms.txt");
    if (!file.is_open()) return roomId;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string id, room_name;
        
        getline(ss, id, '|');
        getline(ss, room_name, '|');
        
        if (String(id) == roomId) {
            file.close();
            return String::fromUtf8(room_name.begin(), room_name.end());
        }
    }
    
    file.close();
    return roomId;
}

// =======================
// Getters
// =======================
String ShowtimeSection::getSelectedMovieName() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize()) {
        String movieId = filteredShowtimes[selectedShowtimeIndex].movie_id;
        return getMovieTitleFromId(movieId);
    }
    // Nếu chưa chọn suất chiếu nhưng có filterMovieId thì vẫn lấy tên phim
    if (!filterMovieId.isEmpty()) {
        return getMovieTitleFromId(filterMovieId);
    }
    return String();
}

String ShowtimeSection::getSelectedMovieId() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize())
        return filteredShowtimes[selectedShowtimeIndex].movie_id;
    // Nếu chưa chọn suất chiếu nhưng có filterMovieId
    if (!filterMovieId.isEmpty())
        return filterMovieId;
    return String();
}

String ShowtimeSection::getSelectedShowtimeId() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize())
        return filteredShowtimes[selectedShowtimeIndex].showtime_id;
    return String();
}

String ShowtimeSection::getSelectedRoomId() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize())
        return filteredShowtimes[selectedShowtimeIndex].room_id;
    return String();
}

String ShowtimeSection::getSelectedDate() const {
    if (selectedDateIndex >= 0 && selectedDateIndex < availableDates.getSize())
        return availableDates[selectedDateIndex];
    return String();
}

String ShowtimeSection::getSelectedTime() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize())
        return filteredShowtimes[selectedShowtimeIndex].time;
    return String();
}

String ShowtimeSection::getSelectedRoomName() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize()) {
            String roomId = filteredShowtimes[selectedShowtimeIndex].room_id;
            return getRoomNameFromId(roomId);
        }
    return String();
}

int ShowtimeSection::getSelectedPrice() const {
    if (selectedShowtimeIndex >= 0 &&
        selectedShowtimeIndex < filteredShowtimes.getSize())
        return filteredShowtimes[selectedShowtimeIndex].price;
    return 0;
}

bool ShowtimeSection::hasSelectedShowtime() const {
    return selectedShowtimeIndex >= 0 && 
           selectedShowtimeIndex < filteredShowtimes.getSize();
}
