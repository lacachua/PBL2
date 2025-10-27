#include "BookingScreen.h"
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
    shouldReturnHome(false)
{
    // ✅ Setup step buttons (navigation bar) - Centered at top
    float startX = 278.f;  // Center: (1728 - (5*180 + 4*10)) / 2
    float startY = 155.f;
    float buttonWidth = 160.f;
    float buttonHeight = 50.f;
    float spacing = 100;

    for (int i = 0; i < 5; i++) {
        buttons_box[i].setSize({buttonWidth, buttonHeight});
        buttons_box[i].setPosition({startX + i * (buttonWidth + spacing), startY});
        
        // Step 1 is active by default
        if (i == 0) {
            buttons_box[i].setFillColor(Color(52, 62, 209)); // Active color
        } else {
            buttons_box[i].setFillColor(Color(80, 80, 90)); // Inactive color
        }
        
        buttons_box[i].setOutlineThickness(2.f);
        buttons_box[i].setOutlineColor(Color(100, 100, 110));
    }

    // ✅ Position text labels on buttons (centered in each button)
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

    // ✅ Setup content container (bên phải poster)
    // Poster sẽ ở vị trí: x=168, y=126 với scale 0.36
    // Poster size gốc ~1000x1500, scaled = ~360x540
    content_area.setSize({982.f, 600.f});  // Container bên cạnh poster
    content_area.setPosition({628.f, 235.f});  // Bắt đầu sau poster (168 + 360 + margin)
    content_area.setFillColor(Color(24, 24, 28));
    content_area.setOutlineThickness(2.f);
    content_area.setOutlineColor(Color(60, 60, 70));

    // prepare empty button vectors
    dateButtons.clear();
    timeButtons.clear();
    
    // ✅ Setup action buttons (Xác nhận & Quay lại)
    // Vị trí: góc phải dưới của content_area
    float btnY = content_area.getPosition().y + content_area.getSize().y - 70.f;
    
    // Nút Xác nhận (bên phải)
    confirmButton.setPosition(
        content_area.getPosition().x + content_area.getSize().x - 192.f, 
        btnY
    );
    confirmButton.setNormalColor(Color(52, 150, 52)); // Xanh lá
    confirmButton.setHoverColor(Color(70, 180, 70));
    confirmButton.setDisabledColor(Color(60, 60, 60));
    confirmButton.setOutlineThickness(2.f);
    confirmButton.setOutlineColor(Color(100, 200, 100));
    
    // Nút Quay lại (bên trái nút Xác nhận)
    backButton.setPosition(
        content_area.getPosition().x + content_area.getSize().x - 362.f,
        btnY
    );
    backButton.setNormalColor(Color(150, 52, 52)); // Đỏ
    backButton.setHoverColor(Color(180, 70, 70));
    backButton.setDisabledColor(Color(60, 60, 60));
    backButton.setOutlineThickness(2.f);
    backButton.setOutlineColor(Color(200, 100, 100));
    
    // ✅ Khởi tạo dữ liệu ghế - sẽ được load từ seat_map khi chọn suất chiếu
    occupiedSeats.clear();
    selectedSeats.clear();
    
    // ✅ Khởi tạo menu đồ ăn
    initializeSnackMenu();
}

// ✅ NEW: Tự động tạo suất chiếu cho 30 ngày với giờ chiếu đa dạng
vector<Showtime> BookingScreen::generateShowtimesForNext30Days(int movieId) {
    vector<Showtime> showtimes;
    
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    
    // ✅ Định nghĩa nhiều bộ giờ chiếu khác nhau
    vector<vector<string>> timeVariants = {
        {"09:00", "11:30", "14:00", "16:30", "19:00", "21:30", "22:00", "23:30"},  // Variant 1
        {"10:00", "12:00", "14:30", "17:00", "19:30", "21:00", "22:30", "00:00"},  // Variant 2
        {"09:30", "11:00", "13:30", "16:00", "18:30", "20:00", "22:00", "23:00"},  // Variant 3
        {"10:30", "13:00", "15:00", "17:30", "19:00", "21:00", "22:00", "23:30"},  // Variant 4
        {"08:30", "11:00", "14:00", "16:00", "18:00", "20:30", "22:30", "00:30"}   // Variant 5
    };
    
    // ✅ Định nghĩa bộ phòng chiếu tương ứng
    vector<vector<string>> roomVariants = {
        {"Phòng 1", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2"},
        {"Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 1", "Phòng 2"},
        {"Phòng 3", "Phòng 1", "Phòng 2", "Phòng 1", "Phòng 3", "Phòng 2", "Phòng 1", "Phòng 3"},
        {"Phòng 1", "Phòng 3", "Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3", "Phòng 1"},
        {"Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3", "Phòng 1", "Phòng 2", "Phòng 3"}
    };
    
    // Tạo suất chiếu cho 30 ngày
    for (int day = 0; day < 30; day++) {
        time_t futureTime = currentTime + (day * 24 * 60 * 60);
        tm* futureDate = localtime(&futureTime);
        
        char dateBuffer[11];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", futureDate);
        string dateStr(dateBuffer);
        
        // ✅ Bỏ qua 2 ngày cuối (ngày 4 và 5 trong 5 ngày đầu)
        int dayInCycle = day % 5;
        if (dayInCycle == 3 || dayInCycle == 4) {
            continue; // Không tạo suất chiếu cho 2 ngày này
        }
        
        // ✅ Chọn variant dựa vào ngày (xoay vòng 5 bộ)
        int variantIndex = day % 5;
        vector<string> times = timeVariants[variantIndex];
        vector<string> rooms = roomVariants[variantIndex];
        
        for (size_t i = 0; i < times.size(); i++) {
            // ✅ Giá động: suất sáng sớm (< 12h) 75k, trưa-chiều (12-18h) 85k, tối (>18h) 95k
            int price = 85000; // Mặc định
            int hour = 0;
            if (sscanf(times[i].c_str(), "%d:", &hour) == 1) {
                if (hour < 12) price = 75000;       // Sáng
                else if (hour >= 18) price = 95000; // Tối
                else price = 85000;                 // Trưa-chiều
            }
            
            // ✅ Số ghế trống ngẫu nhiên (80-100)
            int availableSeats = 80 + (day * 7 + i * 3) % 21; // Pseudo-random 80-100
            
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
        selectedSeats.clear(); // Reset ghế đã chọn
        
        // ✅ Get current date and time
        auto now = std::chrono::system_clock::now();
        time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&currentTime);
        
        char currentDate[11];
        strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", localTime);
        string todayStr(currentDate);
        
        int currentHour = localTime->tm_hour;
        int currentMinute = localTime->tm_min;
        
        // ✅ Generate next 5 days starting from today
        availableDates.clear();
        for (int i = 0; i < 5; i++) {
            time_t futureTime = currentTime + (i * 24 * 60 * 60); // Add i days
            tm* futureDate = localtime(&futureTime);
            
            // Format: YYYY-MM-DD for internal storage
            char dateBuffer[11];
            strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", futureDate);
            availableDates.push_back(string(dateBuffer));
        }
        
        // Set first date as selected
        if (!availableDates.empty()) {
            selectedDate = availableDates[0];
        }
        
        // ✅ Filter showtimes for selected date
        updateShowtimesForSelectedDate(currentHour, currentMinute, todayStr);

        // build date/time buttons
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
        // ✅ Convert YYYY-MM-DD to DD - MM - YYYY for display
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
    // limit to 8 slots
    for (size_t i = 0; i < showtimesForSelectedDate.size() && i < 8; ++i) {
        wstring_convert<codecvt_utf8<wchar_t>> conv;
        wstring label = conv.from_bytes(showtimesForSelectedDate[i].time);
        timeButtons.emplace_back(buttons_font, label, buttonW, buttonH, 20);
        timeButtons.back().setPosition(startX + i * (buttonW + spacing), startY + (i/8)*(buttonH+spacing));
        // disabled state if past
        // determine past state
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
        
        // If selected date is today, filter by time
        if (show.date == todayStr) {
            int showHour, showMinute;
            if (sscanf(show.time.c_str(), "%d:%d", &showHour, &showMinute) == 2) {
                // Skip if showtime has passed (add 30 min buffer)
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

    // ✅ KHÔNG cho phép click vào step buttons nếu chưa xác nhận suất chiếu
    // (trừ step SELECT_DATE - luôn được phép)
    
    // ✅ Handle action buttons (Xác nhận & Quay lại)
    confirmButton.update(mousePos);
    backButton.update(mousePos);
    
    if (confirmButton.isClicked(mousePos, mousePressed) && !confirmButton.getDisabled()) {
        // Logic xác nhận theo từng bước
        if (current_step == BookingStep::SELECT_DATE) {
            // Kiểm tra đã chọn suất chiếu chưa
            if (selectedShowtimeIndex >= 0 && !selectedDate.empty()) {
                hasConfirmedShowtime = true;
                current_step = BookingStep::SELECT_SEAT;
                
                // ✅ Load occupied seats từ seat_map của suất chiếu đã chọn
                if (selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
                    loadOccupiedSeatsFromSeatMap(showtimesForSelectedDate[selectedShowtimeIndex].seat_map);
                }
                
                // Update step button colors
                buttons_box[0].setFillColor(Color(80, 80, 90));
                buttons_box[1].setFillColor(Color(52, 62, 209));
            }
        } else if (current_step == BookingStep::SELECT_SEAT) {
            // ✅ Chỉ chuyển sang bước tiếp theo, CHƯA lưu vào file
            current_step = BookingStep::SELECT_SNACK;
            buttons_box[1].setFillColor(Color(80, 80, 90));
            buttons_box[2].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::SELECT_SNACK) {
            current_step = BookingStep::PAYMENT;
            buttons_box[2].setFillColor(Color(80, 80, 90));
            buttons_box[3].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::PAYMENT) {
            // ✅ CHỈ KHI THANH TOÁN mới lưu ghế vào file (đánh dấu X)
            saveSelectedSeatsToSeatMap();
            
            // ✅ Tạo mã vé 1 lần duy nhất
            srand(time(0));
            char code[32];
            snprintf(code, sizeof(code), "CX%04d%02d%02d", rand() % 10000, (rand() % 12) + 1, (rand() % 28) + 1);
            bookingCode = code;
            
            current_step = BookingStep::CONFIRM;
            buttons_box[3].setFillColor(Color(80, 80, 90));
            buttons_box[4].setFillColor(Color(52, 62, 209));
        } else if (current_step == BookingStep::CONFIRM) {
            // ✅ Về trang chủ sau khi đặt vé thành công
            shouldReturnHome = true;
        }
        return;
    }
    
    if (backButton.isClicked(mousePos, mousePressed) && !backButton.getDisabled()) {
        // ✅ Quay về step trước đó
        if (current_step == BookingStep::SELECT_SEAT) {
            current_step = BookingStep::SELECT_DATE;
            hasConfirmedShowtime = false;
            selectedSeats.clear(); // ✅ Reset ghế khi quay lại chọn suất
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
    
    // ✅ Date buttons (chỉ khi ở SELECT_DATE step)
    if (current_step == BookingStep::SELECT_DATE) {
        for (size_t i = 0; i < dateButtons.size(); ++i) {
            dateButtons[i].update(mousePos);
            if (dateButtons[i].isClicked(mousePos, mousePressed)) {
                selectedDate = availableDates[i];
                selectedShowtimeIndex = -1; // Reset lựa chọn giờ
                selectedSeats.clear(); // ✅ Reset ghế đã chọn khi đổi ngày
                // rebuild time buttons for this date
                // compute current time
                auto now = std::chrono::system_clock::now();
                time_t currentTime = std::chrono::system_clock::to_time_t(now);
                tm* localTime = localtime(&currentTime);
                char curDateBuf[11]; 
                strftime(curDateBuf, sizeof(curDateBuf), "%Y-%m-%d", localTime);
                updateShowtimesForSelectedDate(localTime->tm_hour, localTime->tm_min, string(curDateBuf));
                buildTimeButtons();
                // update selection state
                for (size_t j = 0; j < dateButtons.size(); ++j) dateButtons[j].setSelected(j == i);
                break;
            }
        }

        // Time buttons (chỉ khi ở SELECT_DATE step)
        for (size_t i = 0; i < timeButtons.size(); ++i) {
            timeButtons[i].update(mousePos);
            if (timeButtons[i].isClicked(mousePos, mousePressed)) {
                // if button disabled ignore
                if (timeButtons[i].getDisabled()) continue;
                selectedShowtimeIndex = (int)i;
                selectedSeats.clear(); // ✅ Reset ghế đã chọn khi đổi suất chiếu
                // mark selection visual
                for (size_t j = 0; j < timeButtons.size(); ++j) timeButtons[j].setSelected(j == i);
                break;
            }
        }
    }
    
    // ✅ Seat selection (chỉ khi ở SELECT_SEAT step)
    if (current_step == BookingStep::SELECT_SEAT) {
        // Tính toán vị trí sơ đồ ghế - PHẢI KHỚP HOÀN TOÀN VỚI drawSeatSelection
        float seatStartX = content_area.getPosition().x + 30.f;
        float seatStartY = content_area.getPosition().y + 30.f;
        float seatSize = 30.f;
        float seatSpacing = 6.f;
        
        for (int row = 0; row < SEAT_ROWS; row++) {
            for (int col = 0; col < SEAT_COLS; col++) {
                char rowLabel = 'A' + row;
                string seatID = string(1, rowLabel) + to_string(col + 1);
                
                // Bỏ qua ghế đã được đặt
                if (isSeatOccupied(seatID)) continue;
                
                float x = seatStartX + 65 + col * (seatSize + seatSpacing);
                float y = seatStartY + 161 + row * (seatSize + seatSpacing);
                
                FloatRect seatRect({x, y}, {seatSize, seatSize});
                if (seatRect.contains(mousePos)) {
                    // Toggle selection
                    auto it = find(selectedSeats.begin(), selectedSeats.end(), seatID);
                    if (it != selectedSeats.end()) {
                        selectedSeats.erase(it); // Bỏ chọn
                    } else {
                        selectedSeats.push_back(seatID); // Chọn
                    }
                    return;
                }
            }
        }
    }
    
    // ✅ Snack selection (chỉ khi ở SELECT_SNACK step)
    if (current_step == BookingStep::SELECT_SNACK) {
        for (size_t i = 0; i < snackItems.size(); i++) {
            // Nút cộng (+)
            if (plusButtons[i].isClicked(mousePos, mousePressed)) {
                snackItems[i].quantity++;
                return;
            }
            
            // Nút trừ (-)
            if (minusButtons[i].isClicked(mousePos, mousePressed) && !minusButtons[i].getDisabled()) {
                if (snackItems[i].quantity > 0) {
                    snackItems[i].quantity--;
                }
                return;
            }
        }
    }
}

void BookingScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    // ✅ Kiểm tra flag quay về home
    if (shouldReturnHome) {
        resetBookingData(); // ✅ Reset toàn bộ trước khi về home
        state = AppState::HOME;
        return;
    }
    
    // ✅ Gọi HomeScreen::update để các nút CineXine, Đặt vé, Đăng nhập hoạt động
    HomeScreen::update(mousePos, mousePressed, state);
    // Update button hover states so hover effect appears even without click
    for (auto &btn : dateButtons) btn.update(mousePos);
    for (auto &btn : timeButtons) btn.update(mousePos);
    
    // ✅ Update snack menu buttons
    for (auto &btn : plusButtons) btn.update(mousePos);
    for (auto &btn : minusButtons) btn.update(mousePos);
    
    // ✅ Update action buttons - cả 2 nút đều update
    confirmButton.update(mousePos);
    backButton.update(mousePos);
    
    // ✅ Logic enable/disable và đổi label nút
    if (current_step == BookingStep::SELECT_DATE) {
        // Xác nhận: chỉ disabled khi chưa chọn suất
        confirmButton.setDisabled(selectedShowtimeIndex < 0);
        confirmButton.setText(L"XÁC NHẬN");
        // Quay lại: luôn disabled ở step đầu
        backButton.setDisabled(true);
    } else if (current_step == BookingStep::SELECT_SEAT) {
        // Xác nhận: chỉ enabled khi đã chọn ít nhất 1 ghế
        confirmButton.setDisabled(selectedSeats.empty());
        confirmButton.setText(L"XÁC NHẬN");
        backButton.setDisabled(false);
    } else if (current_step == BookingStep::CONFIRM) {
        // Step cuối: Nút "Về trang chủ"
        confirmButton.setText(L"VỀ TRANG CHỦ");
        confirmButton.setDisabled(false);
        backButton.setDisabled(true); // Không cho quay lại sau khi đặt vé thành công
    } else {
        // Các step khác: Xác nhận luôn enabled, Quay lại luôn enabled
        confirmButton.setText(L"XÁC NHẬN");
        confirmButton.setDisabled(false);
        backButton.setDisabled(false);
    }
}

void BookingScreen::drawStepContent(RenderWindow& window) {
    // Position inside content area
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
            drawActionButtons(window); // Vẽ nút Xác nhận
            break;
        }
        
        case BookingStep::SELECT_SEAT: {
            Text stepTitle(buttons_font, L"CHỌN GHẾ NGỒI", 32);
            stepTitle.setPosition({contentX, contentY});
            stepTitle.setFillColor(Color::White);
            window.draw(stepTitle);
            
            drawSeatSelection(window);  // Vẽ sơ đồ ghế
            drawSeatSummary(window);    // Vẽ thông tin bên phải
            drawActionButtons(window); // Vẽ nút Quay lại
            break;
        }
        
        case BookingStep::SELECT_SNACK: {
            drawSnackMenu(window); // ✅ Vẽ menu đồ ăn
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::PAYMENT: {
            drawPaymentSummary(window); // ✅ Vẽ tổng hợp thanh toán
            drawActionButtons(window);
            break;
        }
        
        case BookingStep::CONFIRM: {
            drawConfirmation(window); // ✅ Vẽ xác nhận thành công
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
    // ✅ Luôn vẽ cả 2 nút ở mọi bước
    backButton.draw(window);
    confirmButton.draw(window);
}

bool BookingScreen::isSeatOccupied(const string& seat) const {
    return find(occupiedSeats.begin(), occupiedSeats.end(), seat) != occupiedSeats.end();
}

bool BookingScreen::isSeatSelected(const string& seat) const {
    return find(selectedSeats.begin(), selectedSeats.end(), seat) != selectedSeats.end();
}

// ✅ Load occupied seats từ seat_map (bitmap 81 ký tự)
void BookingScreen::loadOccupiedSeatsFromSeatMap(const string& seat_map) {
    occupiedSeats.clear();
    
    if (seat_map.length() != 81) {
        // Nếu seat_map không hợp lệ, return
        return;
    }
    
    for (int i = 0; i < 81; i++) {
        if (seat_map[i] == '0') { // 0 = đã đặt
            int row = i / 9;
            int col = i % 9;
            char rowLabel = 'A' + row;
            string seatID = string(1, rowLabel) + to_string(col + 1);
            occupiedSeats.push_back(seatID);
        }
    }
}

// ✅ Lưu selected seats vào seat_map và ghi file
void BookingScreen::saveSelectedSeatsToSeatMap() {
    if (selectedShowtimeIndex < 0 || selectedShowtimeIndex >= (int)showtimesForSelectedDate.size()) {
        return;
    }
    
    Showtime& showtime = showtimesForSelectedDate[selectedShowtimeIndex];
    
    // Tạo seat_map mới từ occupied seats hiện tại
    string seat_map = string(81, '1'); // Mặc định tất cả trống
    
    // Đánh dấu các ghế đã occupied
    for (const auto& seat : occupiedSeats) {
        char rowLabel = seat[0];
        int col = stoi(seat.substr(1)) - 1;
        int row = rowLabel - 'A';
        int index = row * 9 + col;
        if (index >= 0 && index < 81) {
            seat_map[index] = '0';
        }
    }
    
    // Đánh dấu các ghế vừa chọn (cũng là occupied)
    for (const auto& seat : selectedSeats) {
        char rowLabel = seat[0];
        int col = stoi(seat.substr(1)) - 1;
        int row = rowLabel - 'A';
        int index = row * 9 + col;
        if (index >= 0 && index < 81) {
            seat_map[index] = '0';
        }
    }
    
    // Lưu vào file
    saveSeatMap(showtime.movie_id, showtime.date, showtime.time, showtime.room, seat_map);
    
    // Cập nhật lại occupied seats để bao gồm ghế vừa chọn
    occupiedSeats.insert(occupiedSeats.end(), selectedSeats.begin(), selectedSeats.end());
    // ✅ KHÔNG xóa selectedSeats ở đây vì cần hiển thị trong màn hình confirmation
}

void BookingScreen::drawSeatSelection(RenderWindow& window) {
    float seatStartX = content_area.getPosition().x + 30.f;
    float seatStartY = content_area.getPosition().y + 30.f;
    float seatSize = 30.f;
    float seatSpacing = 6.f;
    
    // ✅ Legend (Chú thích) - Đặt ngay dưới "CHỌN GHẾ NGỒI"
    float legendX = content_area.getPosition().x + 95.f;
    float legendY = content_area.getPosition().y + 75.f;
    
    // Ghế trống
    RectangleShape legend1({25.f, 25.f});
    legend1.setPosition({legendX, legendY});
    legend1.setFillColor(Color(60, 60, 70));
    legend1.setOutlineThickness(1.f);
    legend1.setOutlineColor(Color(100, 100, 110));
    window.draw(legend1);
    
    Text legend1Text(detailFont, L"Trống", 14);
    legend1Text.setPosition({legendX + 32.f, legendY + 4.f});
    legend1Text.setFillColor(Color::White);
    window.draw(legend1Text);
    
    // Ghế đã chọn
    RectangleShape legend2({25.f, 25.f});
    legend2.setPosition({legendX + 113.f, legendY});
    legend2.setFillColor(Color(52, 150, 52));
    legend2.setOutlineThickness(2.f);
    legend2.setOutlineColor(Color(100, 255, 100));
    window.draw(legend2);
    
    Text legend2Text(detailFont, L"Đã chọn", 14);
    legend2Text.setPosition({legendX + 145.f, legendY + 4.f});
    legend2Text.setFillColor(Color::White);
    window.draw(legend2Text);
    
    // Ghế đã đặt
    RectangleShape legend3({25.f, 25.f});
    legend3.setPosition({legendX + 235.f, legendY});
    legend3.setFillColor(Color(80, 80, 80));
    legend3.setOutlineThickness(1.f);
    legend3.setOutlineColor(Color(100, 100, 100));
    window.draw(legend3);
    
    Text xMarkLegend(buttons_font, "X", 18);
    xMarkLegend.setPosition({legendX + 244.f, legendY + 1.f});
    xMarkLegend.setFillColor(Color(150, 150, 150));
    window.draw(xMarkLegend);
    
    Text legend3Text(detailFont, L"Đã đặt", 14);
    legend3Text.setPosition({legendX + 267.f, legendY + 4.f});
    legend3Text.setFillColor(Color::White);
    window.draw(legend3Text);
    
    // ✅ Vẽ màn hình (Screen) - Thu nhỏ cho 9x9
    RectangleShape screen({460.f, 8.f});
    screen.setPosition({seatStartX, seatStartY + 105.f});
    screen.setFillColor(Color(200, 200, 200));
    window.draw(screen);
    
    Text screenText(detailFont, L"MÀN HÌNH", 16);
    screenText.setPosition({screen.getPosition().x + 186, screen.getPosition().y - 20});
    screenText.setFillColor(Color(200, 200, 200));
    window.draw(screenText);
    
    // ✅ Lấy vị trí chuột để kiểm tra hover
    Vector2f mousePos = Vector2f(Mouse::getPosition(window));
    
    // ✅ Vẽ các ghế
    for (int row = 0; row < SEAT_ROWS; row++) {
        // Nhãn hàng (A, B, C...) - BỎ ĐI không vẽ nữa
        char rowLabel = 'A' + row;
        
        for (int col = 0; col < SEAT_COLS; col++) {
            string seatID = string(1, rowLabel) + to_string(col + 1);
            
            float x = seatStartX + 65 + col * (seatSize + seatSpacing);
            float y = seatStartY + 161 + row * (seatSize + seatSpacing);
            
            RectangleShape seat({seatSize, seatSize});
            seat.setPosition({x, y});
            
            // ✅ Kiểm tra hover
            bool isHovered = FloatRect({x, y}, {seatSize, seatSize}).contains(mousePos);
            
            // ✅ Màu sắc ghế
            if (isSeatOccupied(seatID)) {
                seat.setFillColor(Color(80, 80, 80)); // Đã đặt - Xám
                seat.setOutlineThickness(1.f);
                seat.setOutlineColor(Color(100, 100, 100));
                window.draw(seat);
                
                // Vẽ dấu X - Căn giữa
                Text xMark(buttons_font, "X", 28);
                FloatRect textBounds = xMark.getLocalBounds();
                xMark.setPosition({
                    x + (seatSize - textBounds.size.x) / 2.f - textBounds.position.x,
                    y + (seatSize - textBounds.size.y) / 2.f - textBounds.position.y
                });
                xMark.setFillColor(Color(150, 150, 150));
                window.draw(xMark);
            } else if (isSeatSelected(seatID)) {
                seat.setFillColor(Color(52, 150, 52)); // Đã chọn - Xanh lá
                seat.setOutlineThickness(2.f);
                seat.setOutlineColor(Color(100, 255, 100));
                window.draw(seat);
            } else {
                // ✅ Ghế trống - Thêm hiệu ứng hover
                if (isHovered) {
                    seat.setFillColor(Color(100, 100, 120)); // Hover - Sáng hơn
                    seat.setOutlineThickness(2.f);
                    seat.setOutlineColor(Color(150, 150, 180));
                } else {
                    seat.setFillColor(Color(60, 60, 70)); // Trống - Xám đậm
                    seat.setOutlineThickness(1.f);
                    seat.setOutlineColor(Color(100, 100, 110));
                }
                window.draw(seat);
            }
        }
    }
}

void BookingScreen::drawSeatSummary(RenderWindow& window) {
    // ✅ Hiển thị thông tin ghế đã chọn bên phải - Điều chỉnh cho 9x9
    float summaryX = content_area.getPosition().x + 530.f;
    float summaryY = content_area.getPosition().y + 30.f;
    
    // Background box - Thu nhỏ lại
    RectangleShape summaryBox({410.f, 380.f});
    summaryBox.setPosition({summaryX, summaryY + 100});
    summaryBox.setFillColor(Color(30, 30, 35));
    summaryBox.setOutlineThickness(2.f);
    summaryBox.setOutlineColor(Color(60, 60, 70));
    window.draw(summaryBox);
    
    // Title
    Text title(buttons_font, L"THÔNG TIN ĐẶT VÉ", 36);
    title.setPosition({summaryX + 120.f, summaryY + 45.f});
    title.setFillColor(Color(255, 200, 100));
    window.draw(title);
    
    // Suất chiếu
    if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
        const auto& showtime = showtimesForSelectedDate[selectedShowtimeIndex];
        
        Text dateLabel(detailFont, L"Ngày chiếu:", 16);
        dateLabel.setPosition({summaryX + 20.f, summaryY + 120.f});
        dateLabel.setFillColor(Color(200, 200, 200));
        window.draw(dateLabel);
        
        // ✅ Chuyển đổi format từ YYYY-MM-DD sang DD - MM - YYYY
        string formattedDate = showtime.date;
        if (formattedDate.length() == 10) { // Format: YYYY-MM-DD
            string year = formattedDate.substr(0, 4);
            string month = formattedDate.substr(5, 2);
            string day = formattedDate.substr(8, 2);
            formattedDate = day + " - " + month + " - " + year; // DD - MM - YYYY
        }
        
        wstring_convert<codecvt_utf8<wchar_t>> conv;
        Text dateValue(detailFont, conv.from_bytes(formattedDate), 16);
        dateValue.setPosition({summaryX + 160.f, summaryY + 120.f});
        dateValue.setFillColor(Color::White);
        window.draw(dateValue);
        
        Text timeLabel(detailFont, L"Giờ chiếu:", 16);
        timeLabel.setPosition({summaryX + 20.f, summaryY + 150.f});
        timeLabel.setFillColor(Color(200, 200, 200));
        window.draw(timeLabel);
        
        Text timeValue(detailFont, conv.from_bytes(showtime.time), 16);
        timeValue.setPosition({summaryX + 160.f, summaryY + 150.f});
        timeValue.setFillColor(Color::White);
        window.draw(timeValue);
        
        Text roomLabel(detailFont, L"Phòng:", 16);
        roomLabel.setPosition({summaryX + 20.f, summaryY + 180.f});
        roomLabel.setFillColor(Color(200, 200, 200));
        window.draw(roomLabel);
        
        Text roomValue(detailFont, conv.from_bytes(showtime.room), 16);
        roomValue.setPosition({summaryX + 160.f, summaryY + 180.f});
        roomValue.setFillColor(Color::White);
        window.draw(roomValue);
    }
    
    // Danh sách ghế đã chọn
    Text seatsLabel(buttons_font, L"GHẾ ĐÃ CHỌN:", 34);
    seatsLabel.setPosition({summaryX + 20.f, summaryY + 220.f});
    seatsLabel.setFillColor(Color(255, 200, 100));
    window.draw(seatsLabel);
    
    if (selectedSeats.empty()) {
        Text noSeats(detailFont, L"Chưa chọn ghế nào", 16);
        noSeats.setPosition({summaryX + 20.f, summaryY + 275.f});
        noSeats.setFillColor(Color(200, 100, 100));
        window.draw(noSeats);
    } 
    else {
        // ✅ Hiển thị danh sách ghế theo chiều dọc (3 hàng), sau đó sang cột mới
        const int MAX_SEATS_DISPLAY = 11;
        const int ROWS_PER_COL = 3; // ✅ 3 ghế theo chiều dọc
        float startX = summaryX + 20.f;
        float startY = summaryY + 275.f; // ✅ Khớp với vị trí "Chưa chọn ghế nào"
        float colWidth = 75.f;
        float rowHeight = 30.f;
        
        wstring_convert<codecvt_utf8<wchar_t>> conv;
        size_t seatsToShow = min((size_t)MAX_SEATS_DISPLAY, selectedSeats.size());
        
        for (size_t i = 0; i < seatsToShow; i++) {
            int col = i / ROWS_PER_COL; // ✅ Chia cho 3 để biết cột nào
            int row = i % ROWS_PER_COL; // ✅ Chia dư 3 để biết hàng nào (0, 1, 2)
            
            Text seatItem(detailFont, conv.from_bytes("• Ghế " + selectedSeats[i]), 16);
            seatItem.setPosition({startX + col * colWidth, startY + row * rowHeight});
            seatItem.setFillColor(Color(100, 255, 100));
            window.draw(seatItem);
        }
        
        // Nếu có nhiều hơn 11 ghế, hiển thị "..." ở vị trí ghế thứ 12
        if (selectedSeats.size() > MAX_SEATS_DISPLAY) {
            int col = MAX_SEATS_DISPLAY / ROWS_PER_COL;
            int row = MAX_SEATS_DISPLAY % ROWS_PER_COL;
            
            Text moreSeats(detailFont, L"...", 16);
            moreSeats.setPosition({startX + col * colWidth, startY + row * rowHeight});
            moreSeats.setFillColor(Color::White);
            window.draw(moreSeats);
        }
        
        // Tổng tiền
        if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
            int pricePerSeat = showtimesForSelectedDate[selectedShowtimeIndex].price;
            int totalPrice = pricePerSeat * selectedSeats.size();
            
            // Line separator
            RectangleShape line({300.f, 2.f});
            line.setPosition({summaryX + 20.f, summaryY + 375.f});
            line.setFillColor(Color(100, 100, 100));
            window.draw(line);
            
            Text totalLabel(buttons_font, L"TỔNG TIỀN:", 36);
            totalLabel.setPosition({summaryX + 20.f, summaryY + 395.f});
            totalLabel.setFillColor(Color::White);
            window.draw(totalLabel);
            
            wstring_convert<codecvt_utf8<wchar_t>> conv;
            char priceStr[32];
            snprintf(priceStr, sizeof(priceStr), "%d VNĐ", totalPrice);
            Text totalValue(buttons_font, conv.from_bytes(priceStr), 36);
            totalValue.setPosition({summaryX + 160.f, summaryY + 395.f});
            totalValue.setFillColor(Color(255, 200, 100));
            window.draw(totalValue);
        }
    }
}

// ✅ Khởi tạo menu đồ ăn
void BookingScreen::initializeSnackMenu() {
    snackItems.clear();
    plusButtons.clear();
    minusButtons.clear();
    
    // ✅ Tạo các combo với giá
    snackItems.emplace_back("Combo 1", 65000, "../assets/elements/combo1.png");
    snackItems.emplace_back("Combo 2", 105000, "../assets/elements/combo2.png");
    snackItems.emplace_back("Bắp & Nước", 55000, "../assets/elements/pop_and_drink.png");
    snackItems.emplace_back("Bắp ngô", 35000, "../assets/elements/one_pop.png");
    snackItems.emplace_back("Nước uống", 25000, "../assets/elements/one_drink.png");
    
    // ✅ Load textures cho mỗi combo
    for (auto& item : snackItems) {
        if (!item.texture.loadFromFile(item.imagePath)) {
            // Handle error - texture failed to load
        }
    }
    
    // ✅ Tạo nút + và - cho mỗi combo
    for (size_t i = 0; i < snackItems.size(); i++) {
        // Nút trừ (-)
        minusButtons.emplace_back(buttons_font, L"-", 40.f, 40.f, 32);
        minusButtons.back().setNormalColor(Color(150, 52, 52));
        minusButtons.back().setHoverColor(Color(180, 70, 70));
        minusButtons.back().setDisabledColor(Color(60, 60, 60));
        minusButtons.back().setOutlineThickness(2.f);
        
        // Nút cộng (+)
        plusButtons.emplace_back(buttons_font, L"+", 40.f, 40.f, 32);
        plusButtons.back().setNormalColor(Color(52, 150, 52));
        plusButtons.back().setHoverColor(Color(70, 180, 70));
        plusButtons.back().setOutlineThickness(2.f);
    }
}

// ✅ Vẽ menu đồ ăn
void BookingScreen::drawSnackMenu(RenderWindow& window) {
    Text title(buttons_font, L"CHỌN ĐỒ ĂN & THỨC UỐNG", 32);
    title.setPosition({content_area.getPosition().x + 30.f, content_area.getPosition().y + 30.f});
    title.setFillColor(Color::White);
    window.draw(title);
    
    float startX = content_area.getPosition().x + 30.f;
    float startY = content_area.getPosition().y + 90.f;
    float itemWidth = 180.f;
    float itemHeight = 220.f;
    float spacing = 20.f;
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    
    for (size_t i = 0; i < snackItems.size(); i++) {
        float x = startX + (i % 5) * (itemWidth + spacing);
        float y = startY + (i / 5) * (itemHeight + spacing);
        
        // ✅ Vẽ hình ảnh combo
        float imageSize = 120.f;
        snackItems[i].draw(window, x, y, imageSize);
        
        // ✅ Tên combo
        Text nameText(detailFont, conv.from_bytes(snackItems[i].name), 16);
        nameText.setPosition({x, y + imageSize + 5});
        nameText.setFillColor(Color::White);
        window.draw(nameText);
        
        // ✅ Giá
        char priceStr[32];
        snprintf(priceStr, sizeof(priceStr), "%d VNĐ", snackItems[i].price);
        Text priceText(detailFont, conv.from_bytes(priceStr), 14);
        priceText.setPosition({x, y + imageSize + 28});
        priceText.setFillColor(Color(255, 200, 100));
        window.draw(priceText);
        
        // ✅ Nút trừ (-)
        minusButtons[i].setPosition(x, y + imageSize + 55);
        minusButtons[i].setDisabled(snackItems[i].quantity == 0);
        minusButtons[i].draw(window);
        
        // ✅ Số lượng (căn giữa giữa 2 nút - và +)
        char qtyStr[8];
        snprintf(qtyStr, sizeof(qtyStr), "%d", snackItems[i].quantity);
        Text qtyText(buttons_font, conv.from_bytes(qtyStr), 24);
        
        // Căn giữa hoàn hảo: tính từ giữa khoảng trống (45 pixel) trừ đi nửa độ rộng text
        FloatRect textBounds = qtyText.getLocalBounds();
        float centerX = x + 65; // Giữa 2 nút (0 -> 90, giữa là 45)
        qtyText.setPosition({
            centerX - textBounds.size.x / 2.f - textBounds.position.x,
            y + imageSize + 60
        });
        qtyText.setFillColor(Color::White);
        window.draw(qtyText);
        
        // ✅ Nút cộng (+)
        plusButtons[i].setPosition(x + 90, y + imageSize + 55);
        plusButtons[i].draw(window);
    }
    
    // ✅ Tổng tiền đồ ăn
    int totalSnackPrice = 0;
    for (const auto& item : snackItems) {
        totalSnackPrice += item.price * item.quantity;
    }
    
    RectangleShape separator({900.f, 2.f});
    separator.setPosition({startX, startY + itemHeight + 80});
    separator.setFillColor(Color(100, 100, 100));
    window.draw(separator);
    
    Text totalLabel(buttons_font, L"TỔNG TIỀN ĐỒ ĂN:", 36);
    totalLabel.setPosition({startX, startY + itemHeight + 100});
    totalLabel.setFillColor(Color::White);
    window.draw(totalLabel);
    
    char totalStr[32];
    snprintf(totalStr, sizeof(totalStr), "%d VNĐ", totalSnackPrice);
    Text totalText(buttons_font, conv.from_bytes(totalStr), 36);
    totalText.setPosition({startX + 300, startY + itemHeight + 100});
    totalText.setFillColor(Color(255, 200, 100));
    window.draw(totalText);
}

// ✅ Vẽ tổng hợp thanh toán
void BookingScreen::drawPaymentSummary(RenderWindow& window) {
    float contentX = content_area.getPosition().x + 30.f;
    float contentY = content_area.getPosition().y + 30.f;
    
    Text title(buttons_font, L"TỔNG HỢP THANH TOÁN", 32);
    title.setPosition({contentX, contentY});
    title.setFillColor(Color::White);
    window.draw(title);
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    float startY = contentY + 70.f;
    float lineHeight = 30.f;
    
    // ✅ Thông tin phim
    if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
        const auto& showtime = showtimesForSelectedDate[selectedShowtimeIndex];
        
        // Tiêu đề phim (lấy từ DetailScreen hoặc Movie data)
        Text movieLabel(buttons_font, L"PHIM:", 24);
        movieLabel.setPosition({contentX, startY});
        movieLabel.setFillColor(Color(255, 200, 100));
        window.draw(movieLabel);
        
        // TODO: Cần lưu tên phim trong BookingScreen, tạm thời dùng movie_id
        char movieInfo[128];
        snprintf(movieInfo, sizeof(movieInfo), "Phim ID: %d", showtime.movie_id);
        Text movieValue(detailFont, conv.from_bytes(movieInfo), 20);
        movieValue.setPosition({contentX + 150.f, startY + 3});
        movieValue.setFillColor(Color::White);
        window.draw(movieValue);
        
        startY += lineHeight + 10;
        
        // Ngày chiếu
        Text dateLabel(detailFont, L"Ngày chiếu:", 18);
        dateLabel.setPosition({contentX, startY});
        dateLabel.setFillColor(Color(200, 200, 200));
        window.draw(dateLabel);
        
        // Format ngày DD - MM - YYYY
        string formattedDate = showtime.date;
        if (formattedDate.length() == 10) {
            string year = formattedDate.substr(0, 4);
            string month = formattedDate.substr(5, 2);
            string day = formattedDate.substr(8, 2);
            formattedDate = day + " - " + month + " - " + year;
        }
        
        Text dateValue(detailFont, conv.from_bytes(formattedDate), 18);
        dateValue.setPosition({contentX + 150.f, startY});
        dateValue.setFillColor(Color::White);
        window.draw(dateValue);
        
        startY += lineHeight;
        
        // Giờ chiếu
        Text timeLabel(detailFont, L"Giờ chiếu:", 18);
        timeLabel.setPosition({contentX, startY});
        timeLabel.setFillColor(Color(200, 200, 200));
        window.draw(timeLabel);
        
        Text timeValue(detailFont, conv.from_bytes(showtime.time), 18);
        timeValue.setPosition({contentX + 150.f, startY});
        timeValue.setFillColor(Color::White);
        window.draw(timeValue);
        
        startY += lineHeight;
        
        // Phòng chiếu
        Text roomLabel(detailFont, L"Phòng:", 18);
        roomLabel.setPosition({contentX, startY});
        roomLabel.setFillColor(Color(200, 200, 200));
        window.draw(roomLabel);
        
        Text roomValue(detailFont, conv.from_bytes(showtime.room), 18);
        roomValue.setPosition({contentX + 150.f, startY});
        roomValue.setFillColor(Color::White);
        window.draw(roomValue);
        
        startY += lineHeight + 20;
        
        // ✅ Ghế ngồi
        Text seatsLabel(buttons_font, L"GHẾ NGỒI:", 24);
        seatsLabel.setPosition({contentX, startY});
        seatsLabel.setFillColor(Color(255, 200, 100));
        window.draw(seatsLabel);
        
        startY += 35;
        
        // Hiển thị danh sách ghế (tối đa 3 hàng x 4 cột)
        const int ROWS_PER_COL = 3;
        float seatStartX = contentX + 20.f;
        float seatColWidth = 70.f;
        float seatRowHeight = 25.f;
        
        for (size_t i = 0; i < selectedSeats.size() && i < 12; i++) {
            int col = i / ROWS_PER_COL;
            int row = i % ROWS_PER_COL;
            
            Text seatText(detailFont, conv.from_bytes("• " + selectedSeats[i]), 16);
            seatText.setPosition({seatStartX + col * seatColWidth, startY + row * seatRowHeight});
            seatText.setFillColor(Color(100, 255, 100));
            window.draw(seatText);
        }
        
        if (selectedSeats.size() > 12) {
            Text moreSeats(detailFont, L"...", 16);
            moreSeats.setPosition({seatStartX + 280, startY});
            moreSeats.setFillColor(Color::White);
            window.draw(moreSeats);
        }
        
        startY += 90;
        
        // ✅ Đồ ăn & thức uống
        Text snackLabel(buttons_font, L"ĐỒ ĂN & NƯỚC:", 24);
        snackLabel.setPosition({contentX, startY});
        snackLabel.setFillColor(Color(255, 200, 100));
        window.draw(snackLabel);
        
        startY += 35;
        
        int totalSnackPrice = 0;
        bool hasSnacks = false;
        
        for (const auto& item : snackItems) {
            if (item.quantity > 0) {
                hasSnacks = true;
                char snackInfo[128];
                int itemTotal = item.price * item.quantity;
                snprintf(snackInfo, sizeof(snackInfo), "%s x %d", item.name.c_str(), item.quantity);
                
                Text snackText(detailFont, conv.from_bytes(snackInfo), 16);
                snackText.setPosition({contentX + 20.f, startY});
                snackText.setFillColor(Color::White);
                window.draw(snackText);
                
                char priceStr[32];
                snprintf(priceStr, sizeof(priceStr), "%d VNĐ", itemTotal);
                Text priceText(detailFont, conv.from_bytes(priceStr), 16);
                priceText.setPosition({contentX + 400.f, startY});
                priceText.setFillColor(Color(255, 200, 100));
                window.draw(priceText);
                
                totalSnackPrice += itemTotal;
                startY += 25;
            }
        }
        
        if (!hasSnacks) {
            Text noSnacks(detailFont, L"Không chọn đồ ăn", 16);
            noSnacks.setPosition({contentX + 20.f, startY});
            noSnacks.setFillColor(Color(150, 150, 150));
            window.draw(noSnacks);
            startY += 25;
        }
        
        startY += 15;
        
        // ✅ Tổng tiền
        RectangleShape separator({900.f, 3.f});
        separator.setPosition({contentX, startY});
        separator.setFillColor(Color(100, 100, 100));
        window.draw(separator);
        
        startY += 20;
        
        // Tiền vé
        int ticketPrice = showtime.price * selectedSeats.size();
        
        Text ticketLabel(detailFont, L"Tiền vé:", 20);
        ticketLabel.setPosition({contentX, startY});
        ticketLabel.setFillColor(Color::White);
        window.draw(ticketLabel);
        
        char ticketStr[32];
        snprintf(ticketStr, sizeof(ticketStr), "%d VNĐ", ticketPrice);
        Text ticketValue(detailFont, conv.from_bytes(ticketStr), 20);
        ticketValue.setPosition({contentX + 400.f, startY});
        ticketValue.setFillColor(Color::White);
        window.draw(ticketValue);
        
        startY += 30;
        
        // Tiền đồ ăn
        Text snackPriceLabel(detailFont, L"Tiền đồ ăn:", 20);
        snackPriceLabel.setPosition({contentX, startY});
        snackPriceLabel.setFillColor(Color::White);
        window.draw(snackPriceLabel);
        
        char snackStr[32];
        snprintf(snackStr, sizeof(snackStr), "%d VNĐ", totalSnackPrice);
        Text snackPriceValue(detailFont, conv.from_bytes(snackStr), 20);
        snackPriceValue.setPosition({contentX + 400.f, startY});
        snackPriceValue.setFillColor(Color::White);
        window.draw(snackPriceValue);
        
        startY += 40;
        
        // TỔNG CỘNG
        int grandTotal = ticketPrice + totalSnackPrice;
        
        Text totalLabel(buttons_font, L"TỔNG CỘNG:", 28);
        totalLabel.setPosition({contentX, startY});
        totalLabel.setFillColor(Color(255, 200, 100));
        window.draw(totalLabel);
        
        char totalStr[32];
        snprintf(totalStr, sizeof(totalStr), "%d VNĐ", grandTotal);
        Text totalValue(buttons_font, conv.from_bytes(totalStr), 32);
        totalValue.setPosition({contentX + 400.f, startY - 5});
        totalValue.setFillColor(Color(100, 255, 100));
        window.draw(totalValue);
    }
}

// ✅ Vẽ màn hình xác nhận đặt vé thành công
void BookingScreen::drawConfirmation(RenderWindow& window) {
    float contentX = content_area.getPosition().x + 30.f;
    float contentY = content_area.getPosition().y + 30.f;
    
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    
    // ✅ Tiêu đề thành công (không có icon)
    Text successTitle(buttons_font, L"ĐẶT VÉ THÀNH CÔNG!", 36);
    FloatRect titleBounds = successTitle.getLocalBounds();
    successTitle.setPosition({contentX + (900.f - titleBounds.size.x) / 2.f, contentY + 20.f});
    successTitle.setFillColor(Color(100, 255, 100));
    window.draw(successTitle);
    
    // ✅ Thông báo
    Text successMsg(detailFont, L"Vé đã được đặt thành công. Mã vé sẽ được gửi qua email/SMS.", 18);
    FloatRect msgBounds = successMsg.getLocalBounds();
    successMsg.setPosition({contentX + (900.f - msgBounds.size.x) / 2.f, contentY + 70.f});
    successMsg.setFillColor(Color(200, 200, 200));
    window.draw(successMsg);
    
    float startY = contentY + 110.f;
    
    // ✅ Thông tin đặt vé
    if (selectedShowtimeIndex >= 0 && selectedShowtimeIndex < (int)showtimesForSelectedDate.size()) {
        const auto& showtime = showtimesForSelectedDate[selectedShowtimeIndex];
        
        // Box chứa thông tin - giảm chiều cao sau khi xóa tổng tiền
        RectangleShape infoBox({850.f, 280.f});
        infoBox.setPosition({contentX + 25.f, startY});
        infoBox.setFillColor(Color(40, 40, 45));
        infoBox.setOutlineThickness(2.f);
        infoBox.setOutlineColor(Color(100, 100, 100));
        window.draw(infoBox);
        
        float infoX = contentX + 50.f;
        float infoY = startY + 20.f;
        float lineHeight = 35.f;
        
        // ✅ Mã đặt vé (đã được tạo sẵn khi chuyển sang CONFIRM)
        Text codeLabel(buttons_font, L"MÃ ĐẶT VÉ:", 24);
        codeLabel.setPosition({infoX, infoY});
        codeLabel.setFillColor(Color(255, 200, 100));
        window.draw(codeLabel);
        
        Text codeValue(buttons_font, conv.from_bytes(bookingCode), 26);
        codeValue.setPosition({infoX + 550.f, infoY});
        codeValue.setFillColor(Color(100, 255, 100));
        window.draw(codeValue);
        
        infoY += lineHeight + 10;
        
        // Phim
        Text movieLabel(detailFont, L"Phim:", 18);
        movieLabel.setPosition({infoX, infoY});
        movieLabel.setFillColor(Color(200, 200, 200));
        window.draw(movieLabel);
        
        char movieInfo[128];
        snprintf(movieInfo, sizeof(movieInfo), "Movie ID: %d", showtime.movie_id);
        Text movieValue(detailFont, conv.from_bytes(movieInfo), 18);
        movieValue.setPosition({infoX + 550.f, infoY});
        movieValue.setFillColor(Color::White);
        window.draw(movieValue);
        
        infoY += lineHeight;
        
        // Ngày chiếu
        Text dateLabel(detailFont, L"Ngày chiếu:", 18);
        dateLabel.setPosition({infoX, infoY});
        dateLabel.setFillColor(Color(200, 200, 200));
        window.draw(dateLabel);
        
        string formattedDate = showtime.date;
        if (formattedDate.length() == 10) {
            string year = formattedDate.substr(0, 4);
            string month = formattedDate.substr(5, 2);
            string day = formattedDate.substr(8, 2);
            formattedDate = day + " - " + month + " - " + year;
        }
        
        Text dateValue(detailFont, conv.from_bytes(formattedDate), 18);
        dateValue.setPosition({infoX + 550.f, infoY});
        dateValue.setFillColor(Color::White);
        window.draw(dateValue);
        
        infoY += lineHeight;
        
        // Giờ chiếu
        Text timeLabel(detailFont, L"Giờ chiếu:", 18);
        timeLabel.setPosition({infoX, infoY});
        timeLabel.setFillColor(Color(200, 200, 200));
        window.draw(timeLabel);
        
        Text timeValue(detailFont, conv.from_bytes(showtime.time), 18);
        timeValue.setPosition({infoX + 550.f, infoY});
        timeValue.setFillColor(Color::White);
        window.draw(timeValue);
        
        infoY += lineHeight;
        
        // Phòng
        Text roomLabel(detailFont, L"Phòng:", 18);
        roomLabel.setPosition({infoX, infoY});
        roomLabel.setFillColor(Color(200, 200, 200));
        window.draw(roomLabel);
        
        Text roomValue(detailFont, conv.from_bytes(showtime.room), 18);
        roomValue.setPosition({infoX + 550.f, infoY});
        roomValue.setFillColor(Color::White);
        window.draw(roomValue);
        
        infoY += lineHeight;
        
        // Ghế
        Text seatsLabel(detailFont, L"Ghế ngồi:", 18);
        seatsLabel.setPosition({infoX, infoY});
        seatsLabel.setFillColor(Color(200, 200, 200));
        window.draw(seatsLabel);
        
        string seatsList;
        for (size_t i = 0; i < selectedSeats.size(); i++) {
            seatsList += selectedSeats[i];
            if (i < selectedSeats.size() - 1) seatsList += ", ";
        }
        
        Text seatsValue(detailFont, conv.from_bytes(seatsList), 18);
        seatsValue.setPosition({infoX + 550.f, infoY});
        seatsValue.setFillColor(Color::White);
        window.draw(seatsValue);
        
        infoY += lineHeight;
        
        // Số lượng vé
        Text qtyLabel(detailFont, L"Số lượng:", 18);
        qtyLabel.setPosition({infoX, infoY});
        qtyLabel.setFillColor(Color(200, 200, 200));
        window.draw(qtyLabel);
        
        char qtyStr[32];
        snprintf(qtyStr, sizeof(qtyStr), "%zu vé", selectedSeats.size());
        Text qtyValue(detailFont, conv.from_bytes(qtyStr), 18);
        qtyValue.setPosition({infoX + 550.f, infoY});
        qtyValue.setFillColor(Color::White);
        window.draw(qtyValue);
    }
    
    // ✅ Hướng dẫn - điều chỉnh vị trí
    Text instructionText(detailFont, L"Vui lòng đến rạp trước 15 phút để nhận vé. Cảm ơn quý khách!", 16);
    FloatRect instBounds = instructionText.getLocalBounds();
    instructionText.setPosition({contentX + (900.f - instBounds.size.x) / 2.f, contentY + 420.f});
    instructionText.setFillColor(Color(150, 150, 150));
    window.draw(instructionText);
}

// ✅ Reset toàn bộ dữ liệu đặt vé khi về trang chủ
void BookingScreen::resetBookingData() {
    current_step = BookingStep::SELECT_DATE;
    hasConfirmedShowtime = false;
    shouldReturnHome = false;
    bookingCode.clear();
    selectedDate.clear();
    selectedShowtimeIndex = -1;
    selectedSeats.clear();
    occupiedSeats.clear();
    
    // ✅ Reset tất cả snack về 0
    for (auto& item : snackItems) {
        item.quantity = 0;
    }
    
    // Reset màu buttons
    buttons_box[0].setFillColor(Color(52, 62, 209));
    for (int i = 1; i < 5; ++i) {
        buttons_box[i].setFillColor(Color(80, 80, 90));
    }
}
