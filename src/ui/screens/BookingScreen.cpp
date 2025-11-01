#include "ui/screens/BookingScreen.h"
#include <locale>
#include <codecvt>
#include <ctime>
#include <chrono>
#include <set>

BookingScreen::BookingScreen(Font& font) : 
    HomeScreen(font),
    buttons_font("../assets/BEBAS_NEUE_ZSMALL.ttf"),
    detailFont("../assets/quicksand_medium.ttf"),
    current_step(BookingStep::SELECT_DATE),
    suat_chieu(buttons_font, L"SUẤT CHIẾU", 36),
    ghe_ngoi(buttons_font, L"GHẾ NGỒI", 36),
    food(buttons_font, L"ĐỒ ĂN", 36),
    thanh_toan(buttons_font, L"THANH TOÁN", 36),
    xac_nhan(buttons_font, L"XÁC NHẬN", 36),
    tex("../assets/trangchumau1.jpg"),
    sprite(tex),
    currentMovieId(-1),
    selectedShowtimeIndex(-1),
    confirmButton(buttons_font, L"XÁC NHẬN", 150.f, 50.f, 24),
    backButton(buttons_font, L"QUAY LẠI", 150.f, 50.f, 24),
    hasConfirmedShowtime(false),
    shouldReturnHome(false),
    seatSelector(detailFont),
    snackMenu(buttons_font),
    paymentSummary(detailFont, buttons_font),
    confirmation(detailFont, buttons_font)
{
    // Setup step buttons (navigation bar)
    float startX = 278.f;
    float startY = 155.f;
    float buttonWidth = 160.f;
    float buttonHeight = 50.f;
    float spacing = 100;

    for (int i = 0; i < 5; i++) {
        buttons_box[i].setSize({buttonWidth, buttonHeight});
        buttons_box[i].setPosition({startX + i * (buttonWidth + spacing), startY});
        
        if (i == 0) {
            buttons_box[i].setFillColor(Color(52, 62, 209)); // Active
        } else {
            buttons_box[i].setFillColor(Color(80, 80, 90)); // Inactive
        }
        
        buttons_box[i].setOutlineThickness(2.f);
        buttons_box[i].setOutlineColor(Color(100, 100, 110));
    }

    // Position text labels on buttons
    suat_chieu.setPosition({buttons_box[0].getPosition().x + 18, 
                            buttons_box[0].getPosition().y + 3});
    suat_chieu.setFillColor(Color::White);

    ghe_ngoi.setPosition({buttons_box[1].getPosition().x + 30, 
                         buttons_box[1].getPosition().y + 3});
    ghe_ngoi.setFillColor(Color::White);

    food.setPosition({buttons_box[2].getPosition().x + 46, 
                     buttons_box[2].getPosition().y + 3});
    food.setFillColor(Color::White);

    thanh_toan.setPosition({buttons_box[3].getPosition().x + 10, 
                           buttons_box[3].getPosition().y + 3});
    thanh_toan.setFillColor(Color::White);

    xac_nhan.setPosition({buttons_box[4].getPosition().x + 26, 
                         buttons_box[4].getPosition().y + 3});
    xac_nhan.setFillColor(Color::White);

    // Setup content container
    content_area.setSize({982.f, 600.f});
    content_area.setPosition({628.f, 235.f});
    content_area.setFillColor(Color(24, 24, 28));
    content_area.setOutlineThickness(2.f);
    content_area.setOutlineColor(Color(60, 60, 70));

    // Setup action buttons
    float btnY = content_area.getPosition().y + content_area.getSize().y - 70.f;
    
    confirmButton.setPosition(
        content_area.getPosition().x + content_area.getSize().x - 192.f, 
        btnY
    );
    confirmButton.setNormalColor(Color(52, 150, 52));
    confirmButton.setHoverColor(Color(70, 180, 70));
    confirmButton.setDisabledColor(Color(60, 60, 60));
    confirmButton.setOutlineThickness(2.f);
    confirmButton.setOutlineColor(Color(100, 200, 100));
    
    backButton.setPosition(
        content_area.getPosition().x + content_area.getSize().x - 362.f,
        btnY
    );
    backButton.setNormalColor(Color(150, 52, 52));
    backButton.setHoverColor(Color(180, 70, 70));
    backButton.setDisabledColor(Color(60, 60, 60));
    backButton.setOutlineThickness(2.f);
    backButton.setOutlineColor(Color(200, 100, 100));
    
    // Initialize snack menu
    snackMenu.initialize();
}

vector<Showtime> BookingScreen::generateShowtimesForNext30Days(int movieId) {
    vector<Showtime> showtimes;
    
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    
    vector<vector<string>> timeVariants = {
        {"09:00", "11:30", "14:00", "16:30", "19:00", "21:30", "22:00", "23:30"},
        {"10:00", "12:00", "14:30", "17:00", "19:30", "21:00", "22:30", "00:00"},
        {"09:30", "11:00", "13:30", "16:00", "18:30", "20:00", "22:00", "23:00"},
        {"10:30", "13:00", "15:00", "17:30", "19:00", "21:00", "22:00", "23:30"},
        {"08:30", "11:00", "14:00", "16:00", "18:00", "20:30", "22:30", "00:30"}
    };
    
    vector<vector<string>> roomVariants = {
        {"Phòng 1", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2"},
        {"Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 1", "Phòng 2"},
        {"Phòng 3", "Phòng 1", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3"},
        {"Phòng 1", "Phòng 3", "Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3", "Phòng 1"},
        {"Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3"}
    };
    
    for (int day = 0; day < 30; day++) {
        time_t futureTime = currentTime + (day * 24 * 60 * 60);
        tm* futureDate = localtime(&futureTime);
        
        char dateBuffer[11];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", futureDate);
        string dateStr(dateBuffer);
        
        int dayInCycle = day % 5;
        if (dayInCycle == 3 || dayInCycle == 4) continue;
        
        int variantIndex = day % 5;
        vector<string> times = timeVariants[variantIndex];
        vector<string> rooms = roomVariants[variantIndex];
        
        for (size_t i = 0; i < times.size(); i++) {
            int price = 85000;
            int hour = 0;
            if (sscanf(times[i].c_str(), "%d:", &hour) == 1) {
                if (hour < 12) price = 75000;
                else if (hour >= 18) price = 95000;
                else price = 85000;
            }
            
            int availableSeats = 80 + (day * 7 + i * 3) % 21;
            
            Showtime show;
            show.movie_id = movieId;
            show.date = dateStr;
            show.time = times[i];
            show.room = rooms[i];
            show.available_seats = availableSeats;
            show.total_seats = 100;
            show.price = price;
            showtimes.push_back(show);
        }
    }
    
    return showtimes;
}

void BookingScreen::draw(RenderWindow& window) {
    HomeScreen::draw(window);
    window.draw(sprite);
    window.draw(content_area);
    drawStepContent(window);

    for (int i = 0; i < 5; i++) 
        window.draw(buttons_box[i]);

    window.draw(suat_chieu);
    window.draw(ghe_ngoi);
    window.draw(food);
    window.draw(thanh_toan);
    window.draw(xac_nhan);
}

void BookingScreen::loadFromDetail(const DetailScreen& detail) {
    try {
        tex = detail.getPosterTexture();
        sprite.setTexture(tex, true);
        sprite.setScale({0.3f, 0.3f});
        sprite.setPosition({128.f, 235.f});
        
        currentMovieId = detail.getCurrentMovieId();
        allShowtimes = generateShowtimesForNext30Days(currentMovieId);
        availableDates.clear();
        selectedDate = "";
        selectedShowtimeIndex = -1;
        hasConfirmedShowtime = false;
        current_step = BookingStep::SELECT_DATE;
        seatSelector.clearSelection();
        
        auto now = std::chrono::system_clock::now();
        time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        
        char currentDate[11];
        strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", localTime);
        string todayStr(currentDate);
        
        int currentHour = localTime->tm_hour;
        int currentMinute = localTime->tm_min;
        
        availableDates.clear();
        for (int i = 0; i < 5; i++) {
            time_t futureTime = currentTime + (i * 24 * 60 * 60);
            tm* futureDate = localtime(&futureTime);
            
            char dateBuffer[11];
            strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", futureDate);
            availableDates.push_back(string(dateBuffer));
        }
        
        if (!availableDates.empty()) {
            selectedDate = availableDates[0];
        }
        
        updateShowtimesForSelectedDate(currentHour, currentMinute, todayStr);
        buildDateButtons();
        buildTimeButtons();
        
    } catch (const exception& e) {
        // Silent error handling
    }
}

void BookingScreen::buildDateButtons() {
    dateButtons.clear();
    float startX = content_area.getPosition().x + 30.f;
    float startY = content_area.getPosition().y + 120.f;
    float buttonW = 150.f; 
    float buttonH = 50.f; 
    float spacing = 20.f;
    
    for (size_t i = 0; i < availableDates.size(); ++i) {
        string dateStr = availableDates[i];
        int year, month, day;
        if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
            char displayDate[20];
            snprintf(displayDate, sizeof(displayDate), "%02d - %02d - %04d", day, month, year);
            
            wstring_convert<codecvt_utf8<wchar_t>> conv;
            wstring label = conv.from_bytes(displayDate);
            
            dateButtons.emplace_back(buttons_font, label, buttonW, buttonH, 18);
            dateButtons.back().setPosition(startX + i * (buttonW + spacing), startY);
            dateButtons.back().setNormalColor(Color(60, 60, 70));
            dateButtons.back().setHoverColor(Color(100, 100, 120));
            dateButtons.back().setDisabledColor(Color(40, 40, 45));
            dateButtons.back().setOutlineThickness(2.f);
            if (availableDates[i] == selectedDate) dateButtons.back().setSelected(true);
        }
    }
}

void BookingScreen::buildTimeButtons() {
    timeButtons.clear();
    float startX = content_area.getPosition().x + 30.f;
    float startY = content_area.getPosition().y + 220.f;
    float buttonW = 100.f; 
    float buttonH = 60.f; 
    float spacing = 15.f;
    
    for (size_t i = 0; i < showtimesForSelectedDate.size() && i < 8; ++i) {
        wstring_convert<codecvt_utf8<wchar_t>> conv;
        wstring label = conv.from_bytes(showtimesForSelectedDate[i].time);
        timeButtons.emplace_back(buttons_font, label, buttonW, buttonH, 20);
        timeButtons.back().setPosition(startX + i * (buttonW + spacing), startY + (i/8)*(buttonH+spacing));
        
        auto now = std::chrono::system_clock::now();
        time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        char curDateBuf[11]; 
        strftime(curDateBuf, sizeof(curDateBuf), "%Y-%m-%d", localTime);
        bool isPast = false;
        if (showtimesForSelectedDate[i].date == string(curDateBuf)) {
            int hh, mm; 
            if (sscanf(showtimesForSelectedDate[i].time.c_str(), "%d:%d", &hh, &mm)==2) {
                if (hh < localTime->tm_hour || (hh==localTime->tm_hour && mm < localTime->tm_min+30)) 
                    isPast = true;
            }
        }
        timeButtons.back().setDisabled(isPast);
    }
}

void BookingScreen::updateShowtimesForSelectedDate(int currentHour, int currentMinute, const string& todayStr) {
    showtimesForSelectedDate.clear();
    
    for (const auto& show : allShowtimes) {
        if (show.movie_id != currentMovieId || show.date != selectedDate) continue;
        
        if (show.date == todayStr) {
            int showHour, showMinute;
            if (sscanf(show.time.c_str(), "%d:%d", &showHour, &showMinute) == 2) {
                if (showHour < currentHour || 
                    (showHour == currentHour && showMinute < currentMinute + 30)) {
                    continue;
                }
            }
        }
        
        showtimesForSelectedDate.push_back(show);
    }
}

void BookingScreen::handleEvent(const RenderWindow& window, const Vector2f& mousePos, bool mousePressed) {
    if (!mousePressed) return;

    confirmButton.update(mousePos);
    backButton.update(mousePos);
    
    if (confirmButton.isClicked(mousePos, mousePressed) && !confirmButton.getDisabled()) {
        if (current_step == BookingStep::SELECT_DATE) {
            if (selectedShowtimeIndex >= 0 && !selectedDate.empty()) {
                hasConfirmedShowtime = true;
                current_step = BookingStep::SELECT_SEAT;
                
                if (selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
                    seatSelector.loadOccupiedSeats(showtimesForSelectedDate[selectedShowtimeIndex].seat_map);
                }
                
                buttons_box[0].setFillColor(Color(80, 80, 90));
                buttons_box[1].setFillColor(Color(52, 62, 209));
            }
        } else if (current_step == BookingStep::SELECT_SEAT) {
            current_step = BookingStep::SELECT_SNACK;
            buttons_box[1].setFillColor(Color(80, 80, 90));
            buttons_box[2].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::SELECT_SNACK) {
            current_step = BookingStep::PAYMENT;
            buttons_box[2].setFillColor(Color(80, 80, 90));
            buttons_box[3].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::PAYMENT) {
            // Save seat map
            saveSeatMap(
                showtimesForSelectedDate[selectedShowtimeIndex].movie_id,
                showtimesForSelectedDate[selectedShowtimeIndex].date,
                showtimesForSelectedDate[selectedShowtimeIndex].time,
                showtimesForSelectedDate[selectedShowtimeIndex].room,
                seatSelector.generateSeatMap()
            );
            
            // Generate booking code
            srand(time(0));
            char code[32];
            snprintf(code, sizeof(code), "CX%04d%02d%02d", rand() % 10000, (rand() % 12) + 1, (rand() % 28) + 1);
            bookingCode = code;
            
            current_step = BookingStep::CONFIRM;
            buttons_box[3].setFillColor(Color(80, 80, 90));
            buttons_box[4].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::CONFIRM) {
            shouldReturnHome = true;
        }
        return;
    }
    
    if (backButton.isClicked(mousePos, mousePressed) && !backButton.getDisabled()) {
        if (current_step == BookingStep::SELECT_SEAT) {
            current_step = BookingStep::SELECT_DATE;
            hasConfirmedShowtime = false;
            seatSelector.clearSelection();
            buttons_box[0].setFillColor(Color(52, 62, 209));
            buttons_box[1].setFillColor(Color(80, 80, 90));
        } else if (current_step == BookingStep::SELECT_SNACK) {
            current_step = BookingStep::SELECT_SEAT;
            buttons_box[1].setFillColor(Color(52, 62, 209));
            buttons_box[2].setFillColor(Color(80, 80, 90));
        } else if (current_step == BookingStep::PAYMENT) {
            current_step = BookingStep::SELECT_SNACK;
            buttons_box[2].setFillColor(Color(52, 62, 209));
            buttons_box[3].setFillColor(Color(80, 80, 90));
        } else if (current_step == BookingStep::CONFIRM) {
            current_step = BookingStep::PAYMENT;
            buttons_box[3].setFillColor(Color(52, 62, 209));
            buttons_box[4].setFillColor(Color(80, 80, 90));
        }
        return;
    }
    
    if (current_step == BookingStep::SELECT_DATE) {
        for (size_t i = 0; i < dateButtons.size(); ++i) {
            dateButtons[i].update(mousePos);
            if (dateButtons[i].isClicked(mousePos, mousePressed)) {
                selectedDate = availableDates[i];
                selectedShowtimeIndex = -1;
                seatSelector.clearSelection();
                
                auto now = std::chrono::system_clock::now();
                time_t currentTime = std::chrono::system_clock::to_time_t(now);
                tm* localTime = localtime(&currentTime);
                char curDateBuf[11]; 
                strftime(curDateBuf, sizeof(curDateBuf), "%Y-%m-%d", localTime);
                updateShowtimesForSelectedDate(localTime->tm_hour, localTime->tm_min, string(curDateBuf));
                buildTimeButtons();
                
                for (size_t j = 0; j < dateButtons.size(); ++j) dateButtons[j].setSelected(j == i);
                break;
            }
        }

        for (size_t i = 0; i < timeButtons.size(); ++i) {
            timeButtons[i].update(mousePos);
            if (timeButtons[i].isClicked(mousePos, mousePressed)) {
                if (timeButtons[i].getDisabled()) continue;
                selectedShowtimeIndex = (int)i;
                seatSelector.clearSelection();
                for (size_t j = 0; j < timeButtons.size(); ++j) timeButtons[j].setSelected(j == i);
                break;
            }
        }
    }
    
    if (current_step == BookingStep::SELECT_SEAT) {
        seatSelector.handleClick(mousePos, content_area.getGlobalBounds());
    }
    
    if (current_step == BookingStep::SELECT_SNACK) {
        snackMenu.handleClick(mousePos);
    }
}

void BookingScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    if (shouldReturnHome) {
        resetBookingData();
        state = AppState::HOME;
        return;
    }
    
    HomeScreen::update(mousePos, mousePressed, state);
    
    for (auto &btn : dateButtons) btn.update(mousePos);
    for (auto &btn : timeButtons) btn.update(mousePos);
    
    snackMenu.updateButtons(mousePos);
    
    confirmButton.update(mousePos);
    backButton.update(mousePos);
    
    if (current_step == BookingStep::SELECT_DATE) {
        confirmButton.setDisabled(selectedShowtimeIndex < 0);
        confirmButton.setText(L"XÁC NHẬN");
        backButton.setDisabled(true);
    } else if (current_step == BookingStep::SELECT_SEAT) {
        confirmButton.setDisabled(seatSelector.getSelectedSeats().empty());
        confirmButton.setText(L"XÁC NHẬN");
        backButton.setDisabled(false);
    } else if (current_step == BookingStep::CONFIRM) {
        confirmButton.setText(L"VỀ TRANG CHỦ");
        confirmButton.setDisabled(false);
        backButton.setDisabled(true);
    } else {
        confirmButton.setText(L"XÁC NHẬN");
        confirmButton.setDisabled(false);
        backButton.setDisabled(false);
    }
}

void BookingScreen::drawStepContent(RenderWindow& window) {
    float contentX = content_area.getPosition().x + 30.f;
    float contentY = content_area.getPosition().y + 30.f;
    
    switch (current_step) {
        case BookingStep::SELECT_DATE: {
            Text stepTitle(buttons_font, L"CHỌN NGÀY & SUẤT CHIẾU", 32);
            stepTitle.setPosition({contentX, contentY});
            stepTitle.setFillColor(Color::White);
            window.draw(stepTitle);
            
            Text stepDesc(detailFont, L"Vui lòng chọn ngày và suất chiếu phù hợp", 20);
            stepDesc.setPosition({contentX, contentY + 50});
            stepDesc.setFillColor(Color(200, 200, 200));
            window.draw(stepDesc);
            
            drawDateSelection(window);
            drawTimeSelection(window);
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::SELECT_SEAT: {
            Text stepTitle(buttons_font, L"CHỌN GHẾ NGỒI", 32);
            stepTitle.setPosition({contentX, contentY});
            stepTitle.setFillColor(Color::White);
            window.draw(stepTitle);
            
            seatSelector.draw(window, content_area.getGlobalBounds());
            
            if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
                FloatRect summaryArea({contentX + 500.f, contentY}, {440.f, 500.f});
                seatSelector.drawSummary(
                    window, 
                    summaryArea,
                    showtimesForSelectedDate[selectedShowtimeIndex].date,
                    showtimesForSelectedDate[selectedShowtimeIndex].time,
                    showtimesForSelectedDate[selectedShowtimeIndex].room,
                    showtimesForSelectedDate[selectedShowtimeIndex].price
                );
            }
            
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::SELECT_SNACK: {
            snackMenu.draw(window, content_area.getGlobalBounds());
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::PAYMENT: {
            if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
                paymentSummary.draw(
                    window,
                    content_area.getGlobalBounds(),
                    showtimesForSelectedDate[selectedShowtimeIndex].movie_id,
                    showtimesForSelectedDate[selectedShowtimeIndex].date,
                    showtimesForSelectedDate[selectedShowtimeIndex].time,
                    showtimesForSelectedDate[selectedShowtimeIndex].room,
                    seatSelector.getSelectedSeats(),
                    showtimesForSelectedDate[selectedShowtimeIndex].price,
                    snackMenu
                );
            }
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::CONFIRM: {
            if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
                confirmation.draw(
                    window,
                    content_area.getGlobalBounds(),
                    bookingCode,
                    showtimesForSelectedDate[selectedShowtimeIndex].movie_id,
                    showtimesForSelectedDate[selectedShowtimeIndex].date,
                    showtimesForSelectedDate[selectedShowtimeIndex].time,
                    showtimesForSelectedDate[selectedShowtimeIndex].room,
                    seatSelector.getSelectedSeats()
                );
            }
            drawActionButtons(window);
            break;
        }
    }
}

void BookingScreen::drawDateSelection(RenderWindow& window) {
    if (availableDates.empty()) {
        Text noDates(detailFont, L"Không có ngày phù hợp", 18);
        noDates.setPosition({content_area.getPosition().x + 30.f, content_area.getPosition().y + 120.f});
        noDates.setFillColor(Color(200, 100, 100));
        window.draw(noDates);
        return;
    }

    for (auto &btn : dateButtons) 
        btn.draw(window);
}

void BookingScreen::drawTimeSelection(RenderWindow& window) {
    if (timeButtons.empty()) {
        Text noShowtimes(detailFont, L"Chưa có suất chiếu cho ngày này!", 18);
        noShowtimes.setPosition({content_area.getPosition().x + 30.f, content_area.getPosition().y + 220.f});
        noShowtimes.setFillColor(Color(200, 100, 100));
        window.draw(noShowtimes);
        return;
    }

    for (auto &btn : timeButtons) 
        btn.draw(window);
}

void BookingScreen::drawActionButtons(RenderWindow& window) {
    backButton.draw(window);
    confirmButton.draw(window);
}

void BookingScreen::resetBookingData() {
    current_step = BookingStep::SELECT_DATE;
    hasConfirmedShowtime = false;
    shouldReturnHome = false;
    bookingCode.clear();
    selectedDate.clear();
    selectedShowtimeIndex = -1;
    seatSelector.clearSelection();
    snackMenu.reset();
    
    buttons_box[0].setFillColor(Color(52, 62, 209));
    for (int i = 1; i < 5; ++i) {
        buttons_box[i].setFillColor(Color(80, 80, 90));
    }
}