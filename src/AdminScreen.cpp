#include "AdminScreen.h"
#include <fstream>
#include <sstream>
#include <algorithm>

AdminScreen::AdminScreen(const Font& font, vector<Showtime>& showtimes, const vector<Movie>& movies)
    : font(font), showtimes(showtimes), movies(movies),
      background({0.f, 0.f}),
      sidebar({280.f, 0.f}),
      contentArea({0.f, 0.f}),
      scrollbar({10.f, 100.f}),
      title(font, L"QUẢN LÝ SUẤT CHIẾU", 24),
      btnView(font, L"Xem tất cả", 16),
      btnAdd(font, L"Thêm mới", 16),
      btnEdit(font, L"Chỉnh sửa", 16),
      btnDelete(font, L"Xóa", 16),
      btnBack(font, L"Quay lại", 16),
      labelMovieId(font, L"ID Phim:", 16),
      labelDate(font, L"Ngày (YYYY-MM-DD):", 16),
      labelTime(font, L"Giờ (HH:MM):", 16),
      labelRoom(font, L"Phòng:", 16),
      labelPrice(font, L"Giá vé:", 16),
      inputMovieId({300.f, 40.f}),
      inputDate({300.f, 40.f}),
      inputTime({300.f, 40.f}),
      inputRoom({300.f, 40.f}),
      inputPrice({300.f, 40.f}),
      textMovieId(font, L"", 16),
      textDate(font, L"", 16),
      textTime(font, L"", 16),
      textRoom(font, L"", 16),
      textPrice(font, L"", 16),
      btnSave(font, L"💾 Lưu", 18),
      btnCancel(font, L"❌ Hủy", 18),
      message(font, L"", 16),
      caret({2.f, 24.f})
{
    // Không set background - sẽ dùng background của HomeScreen
    sidebar.setFillColor(Color(50, 50, 60, 230)); // Semi-transparent
    contentArea.setFillColor(Color(255, 255, 255, 250)); // Semi-transparent white
    contentArea.setOutlineThickness(2.f);
    contentArea.setOutlineColor(Color(200, 200, 200));
    scrollbar.setFillColor(Color(150, 150, 150));
    
    title.setFillColor(Color::White);
    btnView.setFillColor(Color(220, 220, 220));
    btnAdd.setFillColor(Color(220, 220, 220));
    btnEdit.setFillColor(Color(220, 220, 220));
    btnDelete.setFillColor(Color(220, 220, 220));
    btnBack.setFillColor(Color(255, 200, 100));
    
    labelMovieId.setFillColor(Color::Black);
    labelDate.setFillColor(Color::Black);
    labelTime.setFillColor(Color::Black);
    labelRoom.setFillColor(Color::Black);
    labelPrice.setFillColor(Color::Black);
    
    inputMovieId.setFillColor(Color(250, 250, 250));
    inputMovieId.setOutlineThickness(2.f);
    inputMovieId.setOutlineColor(Color(200, 200, 200));
    
    inputDate = inputTime = inputRoom = inputPrice = inputMovieId;
    
    textMovieId.setFillColor(Color::Black);
    textDate.setFillColor(Color::Black);
    textTime.setFillColor(Color::Black);
    textRoom.setFillColor(Color::Black);
    textPrice.setFillColor(Color::Black);
    
    btnSave.setFillColor(Color(60, 180, 90));
    btnCancel.setFillColor(Color(220, 80, 80));
    
    message.setFillColor(Color(60, 160, 90));
    caret.setFillColor(Color::Black);
}

void AdminScreen::update(Vector2f mouse, bool mousePressed, const Event& event) {
    if (currentMode == Mode::VIEW) {
        handleViewMode(mouse, mousePressed, event);
    } else if (currentMode == Mode::ADD || currentMode == Mode::EDIT) {
        handleAddEditMode(mouse, mousePressed, event);
    } else if (currentMode == Mode::DELETE) {
        handleDeleteMode(mouse, mousePressed, event);
    }
    
    // Caret blink
    if (caretClock.getElapsedTime().asSeconds() >= 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }
}

void AdminScreen::handleViewMode(Vector2f mouse, bool mousePressed, const Event& event) {
    if (mousePressed) {
        // Check sidebar buttons
        if (btnAdd.getGlobalBounds().contains(mouse)) {
            currentMode = Mode::ADD;
            clearForm();
            message.setString(L"");
        } else if (btnEdit.getGlobalBounds().contains(mouse)) {
            if (selectedIndex >= 0 && selectedIndex < showtimes.size()) {
                currentMode = Mode::EDIT;
                loadShowtimeToForm(selectedIndex);
                message.setString(L"");
            } else {
                message.setFillColor(Color(220, 80, 80));
                message.setString(L"Vui lòng chọn suất chiếu để chỉnh sửa!");
            }
        } else if (btnDelete.getGlobalBounds().contains(mouse)) {
            if (selectedIndex >= 0 && selectedIndex < showtimes.size()) {
                currentMode = Mode::DELETE;
                message.setString(L"");
            } else {
                message.setFillColor(Color(220, 80, 80));
                message.setString(L"Vui lòng chọn suất chiếu để xóa!");
            }
        } else if (btnBack.getGlobalBounds().contains(mouse)) {
            closeRequested = true;
        }
        
        // Check showtime selection (in content area)
        // Simple implementation: click on row
        float startY = 140.f;
        for (int i = scrollOffset; i < showtimes.size() && i < scrollOffset + 10; i++) {
            float rowY = startY + (i - scrollOffset) * 50.f;
            FloatRect rowBounds({280.f, rowY}, {900.f, 45.f});
            if (rowBounds.contains(mouse)) {
                selectedIndex = i;
                break;
            }
        }
    }
    
    // Scroll with mouse wheel
    if (event.is<Event::MouseWheelScrolled>()) {
        float delta = event.getIf<Event::MouseWheelScrolled>()->delta;
        scrollOffset -= (int)delta;
        scrollOffset = max(0, min(scrollOffset, (int)showtimes.size() - 10));
    }
}

void AdminScreen::handleAddEditMode(Vector2f mouse, bool mousePressed, const Event& event) {
    if (mousePressed) {
        // Check which field is clicked
        if (inputMovieId.getGlobalBounds().contains(mouse)) activeField = 0;
        else if (inputDate.getGlobalBounds().contains(mouse)) activeField = 1;
        else if (inputTime.getGlobalBounds().contains(mouse)) activeField = 2;
        else if (inputRoom.getGlobalBounds().contains(mouse)) activeField = 3;
        else if (inputPrice.getGlobalBounds().contains(mouse)) activeField = 4;
        else activeField = -1;
        
        // Buttons
        if (btnSave.getGlobalBounds().contains(mouse)) {
            saveShowtime();
        } else if (btnCancel.getGlobalBounds().contains(mouse)) {
            currentMode = Mode::VIEW;
            clearForm();
            message.setString(L"");
        }
    }
    
    // Keyboard input
    if (event.is<Event::KeyPressed>()) {
        auto code = event.getIf<Event::KeyPressed>()->code;
        if (code == Keyboard::Key::Escape) {
            currentMode = Mode::VIEW;
            clearForm();
        } else if (code == Keyboard::Key::Tab) {
            activeField = (activeField + 1) % 5;
        } else if (code == Keyboard::Key::Backspace) {
            if (activeField == 0 && !dataMovieId.empty()) dataMovieId.pop_back();
            else if (activeField == 1 && !dataDate.empty()) dataDate.pop_back();
            else if (activeField == 2 && !dataTime.empty()) dataTime.pop_back();
            else if (activeField == 3 && !dataRoom.empty()) dataRoom.pop_back();
            else if (activeField == 4 && !dataPrice.empty()) dataPrice.pop_back();
        }
    }
    
    if (event.is<Event::TextEntered>()) {
        char32_t unicode = event.getIf<Event::TextEntered>()->unicode;
        if (unicode >= 32 && unicode != 127) {
            wchar_t ch = (wchar_t)unicode;
            if (activeField == 0 && dataMovieId.size() < 10) dataMovieId.push_back(ch);
            else if (activeField == 1 && dataDate.size() < 20) dataDate.push_back(ch);
            else if (activeField == 2 && dataTime.size() < 10) dataTime.push_back(ch);
            else if (activeField == 3 && dataRoom.size() < 30) dataRoom.push_back(ch);
            else if (activeField == 4 && dataPrice.size() < 10) dataPrice.push_back(ch);
        }
    }
    
    // Update text displays
    textMovieId.setString(dataMovieId);
    textDate.setString(dataDate);
    textTime.setString(dataTime);
    textRoom.setString(dataRoom);
    textPrice.setString(dataPrice);
}

void AdminScreen::handleDeleteMode(Vector2f mouse, bool mousePressed, const Event& event) {
    if (mousePressed) {
        if (btnSave.getGlobalBounds().contains(mouse)) {
            deleteShowtime();
        } else if (btnCancel.getGlobalBounds().contains(mouse)) {
            currentMode = Mode::VIEW;
            message.setString(L"");
        }
    }
    
    if (event.is<Event::KeyPressed>()) {
        auto code = event.getIf<Event::KeyPressed>()->code;
        if (code == Keyboard::Key::Escape) {
            currentMode = Mode::VIEW;
        }
    }
}

void AdminScreen::saveShowtime() {
    // Validate inputs
    if (dataMovieId.empty() || dataDate.empty() || dataTime.empty() || 
        dataRoom.empty() || dataPrice.empty()) {
        message.setFillColor(Color(220, 80, 80));
        message.setString(L"Vui lòng điền đầy đủ thông tin!");
        return;
    }
    
    try {
        Showtime st;
        st.movie_id = stoi(wstring(dataMovieId.begin(), dataMovieId.end()));
        st.date = string(dataDate.begin(), dataDate.end());
        st.time = string(dataTime.begin(), dataTime.end());
        st.room = string(dataRoom.begin(), dataRoom.end());
        st.price = stoi(wstring(dataPrice.begin(), dataPrice.end()));
        st.total_seats = 100;
        st.available_seats = 100;
        st.seat_map = string(81, '1'); // All seats available
        
        if (currentMode == Mode::ADD) {
            showtimes.push_back(st);
            message.setFillColor(Color(60, 160, 90));
            message.setString(L"✅ Thêm suất chiếu thành công!");
        } else if (currentMode == Mode::EDIT) {
            if (selectedIndex >= 0 && selectedIndex < showtimes.size()) {
                // Keep the seat map from original
                st.seat_map = showtimes[selectedIndex].seat_map;
                st.available_seats = showtimes[selectedIndex].available_seats;
                showtimes[selectedIndex] = st;
                message.setFillColor(Color(60, 160, 90));
                message.setString(L"✅ Cập nhật suất chiếu thành công!");
            }
        }
        
        saveShowtimesToCSV();
        currentMode = Mode::VIEW;
        clearForm();
    } catch (...) {
        message.setFillColor(Color(220, 80, 80));
        message.setString(L"Lỗi: Dữ liệu không hợp lệ!");
    }
}

void AdminScreen::deleteShowtime() {
    if (selectedIndex >= 0 && selectedIndex < showtimes.size()) {
        showtimes.erase(showtimes.begin() + selectedIndex);
        saveShowtimesToCSV();
        message.setFillColor(Color(60, 160, 90));
        message.setString(L"✅ Xóa suất chiếu thành công!");
        selectedIndex = -1;
        currentMode = Mode::VIEW;
    }
}

void AdminScreen::clearForm() {
    dataMovieId.clear();
    dataDate.clear();
    dataTime.clear();
    dataRoom.clear();
    dataPrice.clear();
    activeField = -1;
}

void AdminScreen::loadShowtimeToForm(int index) {
    if (index < 0 || index >= showtimes.size()) return;
    
    const Showtime& st = showtimes[index];
    dataMovieId = to_wstring(st.movie_id);
    dataDate = wstring(st.date.begin(), st.date.end());
    dataTime = wstring(st.time.begin(), st.time.end());
    dataRoom = wstring(st.room.begin(), st.room.end());
    dataPrice = to_wstring(st.price);
}

string AdminScreen::getMovieTitle(int movieId) const {
    string id = to_string(movieId);
    for (const auto& m : movies) {
        if (m.film_id == id) return m.title;
    }
    return "Unknown";
}

void AdminScreen::saveShowtimesToCSV() {
    ofstream file("../data/showtimes.csv", ios::out | ios::trunc);
    if (!file.is_open()) return;
    
    file << "movie_id,date,time,room,available_seats,total_seats,price\n";
    for (const auto& st : showtimes) {
        file << st.movie_id << "," << st.date << "," << st.time << ","
             << st.room << "," << st.available_seats << "," << st.total_seats << ","
             << st.price << "\n";
    }
    file.close();
}

void AdminScreen::draw(RenderWindow& window) {
    auto size = window.getSize();
    
    // Không vẽ background nữa - để HomeScreen vẽ
    // Chỉ vẽ sidebar và content area trong vùng slider (khoảng y: 150-700)
    
    float sliderY = 150.f;
    float sliderHeight = 550.f;
    
    sidebar.setSize({280.f, sliderHeight});
    sidebar.setPosition({20.f, sliderY});
    
    contentArea.setSize({(float)size.x - 340.f, sliderHeight - 20.f});
    contentArea.setPosition({310.f, sliderY + 10.f});
    
    window.draw(sidebar);
    window.draw(contentArea);
    
    // Sidebar content
    title.setPosition({30.f, sliderY + 20.f});
    window.draw(title);
    
    btnView.setPosition({30.f, sliderY + 80.f});
    btnAdd.setPosition({30.f, sliderY + 120.f});
    btnEdit.setPosition({30.f, sliderY + 160.f});
    btnDelete.setPosition({30.f, sliderY + 200.f});
    btnBack.setPosition({30.f, sliderY + sliderHeight - 50.f});
    
    window.draw(btnView);
    window.draw(btnAdd);
    window.draw(btnEdit);
    window.draw(btnDelete);
    window.draw(btnBack);
    
    // Content - điều chỉnh position để fit trong content area
    if (currentMode == Mode::VIEW) {
        drawViewMode(window);
    } else if (currentMode == Mode::ADD || currentMode == Mode::EDIT) {
        drawAddEditMode(window);
    } else if (currentMode == Mode::DELETE) {
        drawDeleteMode(window);
    }
    
    // Message ở dưới content area
    message.setPosition({320.f, sliderY + sliderHeight - 30.f});
    window.draw(message);
}

void AdminScreen::drawViewMode(RenderWindow& window) {
    float contentX = 320.f;
    float contentY = 170.f;
    
    Text header(font, L"DANH SÁCH SUẤT CHIẾU", 20);
    header.setFillColor(Color::Black);
    header.setPosition({contentX, contentY});
    window.draw(header);
    
    // Table headers
    Text hId(font, L"ID", 12);
    Text hMovie(font, L"Phim", 12);
    Text hDate(font, L"Ngày", 12);
    Text hTime(font, L"Giờ", 12);
    Text hRoom(font, L"Phòng", 12);
    Text hPrice(font, L"Giá", 12);
    
    hId.setFillColor(Color::Black);
    hMovie.setFillColor(Color::Black);
    hDate.setFillColor(Color::Black);
    hTime.setFillColor(Color::Black);
    hRoom.setFillColor(Color::Black);
    hPrice.setFillColor(Color::Black);
    
    float headerY = contentY + 40.f;
    hId.setPosition({contentX + 10.f, headerY});
    hMovie.setPosition({contentX + 60.f, headerY});
    hDate.setPosition({contentX + 300.f, headerY});
    hTime.setPosition({contentX + 450.f, headerY});
    hRoom.setPosition({contentX + 550.f, headerY});
    hPrice.setPosition({contentX + 700.f, headerY});
    
    window.draw(hId);
    window.draw(hMovie);
    window.draw(hDate);
    window.draw(hTime);
    window.draw(hRoom);
    window.draw(hPrice);
    
    // Showtimes list
    float startY = contentY + 70.f;
    int maxRows = 11; // Fit trong content area
    for (int i = scrollOffset; i < showtimes.size() && i < scrollOffset + maxRows; i++) {
        const Showtime& st = showtimes[i];
        float rowY = startY + (i - scrollOffset) * 35.f;
        
        // Highlight selected
        if (i == selectedIndex) {
            RectangleShape highlight({800.f, 33.f});
            highlight.setPosition({contentX, rowY - 2.f});
            highlight.setFillColor(Color(200, 220, 255));
            window.draw(highlight);
        }
        
        Text tId(font, to_wstring(st.movie_id), 12);
        string movieTitle = getMovieTitle(st.movie_id);
        Text tMovie(font, String::fromUtf8(movieTitle.begin(), movieTitle.end()), 12);
        Text tDate(font, String::fromUtf8(st.date.begin(), st.date.end()), 12);
        Text tTime(font, String::fromUtf8(st.time.begin(), st.time.end()), 12);
        Text tRoom(font, String::fromUtf8(st.room.begin(), st.room.end()), 12);
        Text tPrice(font, to_wstring(st.price), 12);
        
        tId.setFillColor(Color::Black);
        tMovie.setFillColor(Color::Black);
        tDate.setFillColor(Color::Black);
        tTime.setFillColor(Color::Black);
        tRoom.setFillColor(Color::Black);
        tPrice.setFillColor(Color::Black);
        
        tId.setPosition({contentX + 10.f, rowY});
        tMovie.setPosition({contentX + 60.f, rowY});
        tDate.setPosition({contentX + 300.f, rowY});
        tTime.setPosition({contentX + 450.f, rowY});
        tRoom.setPosition({contentX + 550.f, rowY});
        tPrice.setPosition({contentX + 700.f, rowY});
        
        window.draw(tId);
        window.draw(tMovie);
        window.draw(tDate);
        window.draw(tTime);
        window.draw(tRoom);
        window.draw(tPrice);
    }
    
    // Scrollbar
    if (showtimes.size() > maxRows) {
        scrollbar.setPosition({contentX + 820.f, startY + scrollOffset * 2.f});
        window.draw(scrollbar);
    }
}

void AdminScreen::drawAddEditMode(RenderWindow& window) {
    float contentX = 320.f;
    float contentY = 170.f;
    
    Text header(font, currentMode == Mode::ADD ? L"THÊM SUẤT CHIẾU MỚI" : L"CHỈNH SỬA SUẤT CHIẾU", 18);
    header.setFillColor(Color::Black);
    header.setPosition({contentX, contentY});
    window.draw(header);
    
    float startY = contentY + 40.f;
    float gap = 60.f;
    
    // Movie ID
    labelMovieId.setPosition({contentX, startY});
    inputMovieId.setPosition({contentX, startY + 25.f});
    inputMovieId.setOutlineColor(activeField == 0 ? Color(100, 149, 237) : Color(200, 200, 200));
    textMovieId.setPosition({contentX + 10.f, startY + 33.f});
    window.draw(labelMovieId);
    window.draw(inputMovieId);
    window.draw(textMovieId);
    if (activeField == 0 && caretVisible) {
        FloatRect bounds = textMovieId.getLocalBounds();
        caret.setPosition({contentX + 10.f + bounds.size.x + 2.f, startY + 31.f});
        window.draw(caret);
    }
    
    // Date
    labelDate.setPosition({contentX, startY + gap});
    inputDate.setPosition({contentX, startY + gap + 25.f});
    inputDate.setOutlineColor(activeField == 1 ? Color(100, 149, 237) : Color(200, 200, 200));
    textDate.setPosition({contentX + 10.f, startY + gap + 33.f});
    window.draw(labelDate);
    window.draw(inputDate);
    window.draw(textDate);
    if (activeField == 1 && caretVisible) {
        FloatRect bounds = textDate.getLocalBounds();
        caret.setPosition({contentX + 10.f + bounds.size.x + 2.f, startY + gap + 31.f});
        window.draw(caret);
    }
    
    // Time
    labelTime.setPosition({contentX, startY + gap * 2});
    inputTime.setPosition({contentX, startY + gap * 2 + 25.f});
    inputTime.setOutlineColor(activeField == 2 ? Color(100, 149, 237) : Color(200, 200, 200));
    textTime.setPosition({contentX + 10.f, startY + gap * 2 + 33.f});
    window.draw(labelTime);
    window.draw(inputTime);
    window.draw(textTime);
    if (activeField == 2 && caretVisible) {
        FloatRect bounds = textTime.getLocalBounds();
        caret.setPosition({contentX + 10.f + bounds.size.x + 2.f, startY + gap * 2 + 31.f});
        window.draw(caret);
    }
    
    // Room
    labelRoom.setPosition({contentX, startY + gap * 3});
    inputRoom.setPosition({contentX, startY + gap * 3 + 25.f});
    inputRoom.setOutlineColor(activeField == 3 ? Color(100, 149, 237) : Color(200, 200, 200));
    textRoom.setPosition({contentX + 10.f, startY + gap * 3 + 33.f});
    window.draw(labelRoom);
    window.draw(inputRoom);
    window.draw(textRoom);
    if (activeField == 3 && caretVisible) {
        FloatRect bounds = textRoom.getLocalBounds();
        caret.setPosition({contentX + 10.f + bounds.size.x + 2.f, startY + gap * 3 + 31.f});
        window.draw(caret);
    }
    
    // Price
    labelPrice.setPosition({contentX, startY + gap * 4});
    inputPrice.setPosition({contentX, startY + gap * 4 + 25.f});
    inputPrice.setOutlineColor(activeField == 4 ? Color(100, 149, 237) : Color(200, 200, 200));
    textPrice.setPosition({contentX + 10.f, startY + gap * 4 + 33.f});
    window.draw(labelPrice);
    window.draw(inputPrice);
    window.draw(textPrice);
    if (activeField == 4 && caretVisible) {
        FloatRect bounds = textPrice.getLocalBounds();
        caret.setPosition({contentX + 10.f + bounds.size.x + 2.f, startY + gap * 4 + 31.f});
        window.draw(caret);
    }
    
    // Buttons
    btnSave.setPosition({contentX, startY + gap * 5});
    btnCancel.setPosition({contentX + 150.f, startY + gap * 5});
    window.draw(btnSave);
    window.draw(btnCancel);
}

void AdminScreen::drawDeleteMode(RenderWindow& window) {
    float contentX = 320.f;
    float contentY = 170.f;
    
    Text header(font, L"XÓA SUẤT CHIẾU", 18);
    header.setFillColor(Color::Black);
    header.setPosition({contentX, contentY});
    window.draw(header);
    
    if (selectedIndex >= 0 && selectedIndex < showtimes.size()) {
        const Showtime& st = showtimes[selectedIndex];
        
        Text confirm(font, L"Bạn có chắc chắn muốn xóa suất chiếu này?", 16);
        confirm.setFillColor(Color::Black);
        confirm.setPosition({contentX, contentY + 40.f});
        window.draw(confirm);
        
        wstring info = L"Phim: " + wstring(getMovieTitle(st.movie_id).begin(), getMovieTitle(st.movie_id).end()) +
                       L"\nNgày: " + wstring(st.date.begin(), st.date.end()) +
                       L"\nGiờ: " + wstring(st.time.begin(), st.time.end()) +
                       L"\nPhòng: " + wstring(st.room.begin(), st.room.end());
        
        Text details(font, info, 14);
        details.setFillColor(Color::Black);
        details.setPosition({contentX, contentY + 80.f});
        window.draw(details);
        
        btnSave.setString(L"Xác nhận xóa");
        btnSave.setPosition({contentX, contentY + 200.f});
        btnCancel.setPosition({contentX + 150.f, contentY + 200.f});
        window.draw(btnSave);
        window.draw(btnCancel);
    }
}
