#include "AdminScreen.h"
#include <fstream>
#include <sstream>

// ==================== ENUM VIEW =====================
enum class AdminView {
    NONE, MOVIE_LIST, MOVIE_ADD, MOVIE_EDIT, MOVIE_DELETE
};

// ==================== CONSTRUCTOR =====================
AdminScreen::AdminScreen(RenderWindow& window) : 
    uppercase_font("../assets/Montserrat_SemiBold.ttf"),
    lowercase_font("../assets/quicksand_medium.ttf"),
    sidebar({300.f, (float)window.getSize().y}),
    admin_logo({300.f, 80.f}),
    admin_logoText(uppercase_font, "ADMIN", 36),
    arrow_down("../assets/elements/arrow_down.png"),
    arrow_up("../assets/elements/arrow_up.png"),
    arrow_down_tex(arrow_down),
    arrow_up_tex(arrow_up)
{
    sidebar.setFillColor(Color(13, 16, 69));
    admin_logo.setFillColor(Color(59, 108, 177));
    admin_logoText.setOutlineColor(Color(105, 75, 160));
    admin_logoText.setOutlineThickness(2.f);
    admin_logoText.setPosition({
        admin_logo.getPosition().x + admin_logo.getSize().x / 2 - admin_logoText.getGlobalBounds().size.x / 2, 
        admin_logo.getPosition().y + admin_logo.getSize().y / 2 - admin_logoText.getGlobalBounds().size.y + 8.f});

    menuItems = {
        {lowercase_font, L"Dashboard", 300.f, 60.f},
        {lowercase_font, L"Quản lý rạp phim", 300.f, 60.f},
        {lowercase_font, L"Quản lý phim", 300.f, 60.f},
        {lowercase_font, L"Quản lý lịch chiếu", 300.f, 60.f},
        {lowercase_font, L"Quản lý suất chiếu", 300.f, 60.f},
        {lowercase_font, L"Quản lý đơn hàng", 300.f, 60.f},
        {lowercase_font, L"Quản lý giá vé", 300.f, 60.f},
        {lowercase_font, L"Quản lý khuyến mãi", 300.f, 60.f},
        {lowercase_font, L"Quản lý user", 300.f, 60.f},
        {lowercase_font, L"Quản lý combo - nước", 300.f, 60.f},
    };

    expanded.resize(menuItems.size(), false);
    subItems.resize(menuItems.size());
    
    subItems[0].push_back(Button(lowercase_font, L"Thống kê tổng quan", 300.f, 40.f, 16));

    subItems[1].push_back(Button(lowercase_font, L"Danh sách rạp", 300.f, 40.f, 16));
    subItems[1].push_back(Button(lowercase_font, L"Thêm rạp mới", 300.f, 40.f, 16));
    subItems[1].push_back(Button(lowercase_font, L"Sửa thông tin rạp", 300.f, 40.f, 16));

    subItems[2].push_back(Button(lowercase_font, L"Danh sách phim", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Thêm phim mới", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Chỉnh sửa phim", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Xoá phim", 300.f, 40.f, 16));
    
    subItems[3].push_back(Button(lowercase_font, L"Danh sách lịch chiếu", 300.f, 40.f, 16));
    subItems[3].push_back(Button(lowercase_font, L"Thêm lịch chiếu", 300.f, 40.f, 16));
    
    subItems[4].push_back(Button(lowercase_font, L"Danh sách suất chiếu", 300.f, 40.f, 16));
    subItems[4].push_back(Button(lowercase_font, L"Thêm suất chiếu", 300.f, 40.f, 16));
    
    subItems[5].push_back(Button(lowercase_font, L"Danh sách đơn hàng", 300.f, 40.f, 16));
    
    subItems[6].push_back(Button(lowercase_font, L"Danh sách giá vé", 300.f, 40.f, 16));
    subItems[6].push_back(Button(lowercase_font, L"Thêm giá vé", 300.f, 40.f, 16));
    
    subItems[7].push_back(Button(lowercase_font, L"Danh sách khuyến mãi", 300.f, 40.f, 16));
    subItems[7].push_back(Button(lowercase_font, L"Thêm mã giảm giá", 300.f, 40.f, 16));
    
    subItems[8].push_back(Button(lowercase_font, L"Danh sách user", 300.f, 40.f, 16));
    
    subItems[9].push_back(Button(lowercase_font, L"Danh sách combo", 300.f, 40.f, 16));
    subItems[9].push_back(Button(lowercase_font, L"Thêm combo mới", 300.f, 40.f, 16));
    
    for (int i = 0; i < menuItems.size(); i++) {
        menuItems[i].setPosition({0.f, 80.f + i * 60.f});
        menuItems[i].setNormalColor(Color(13, 16, 69));
        menuItems[i].setHoverColor(Color(57, 145, 222));
        menuItems[i].setOutlineThickness(0);
    }

    for (int i = 0; i < subItems.size(); i++) {
        for (int j = 0; j < subItems[i].size(); j++) {
            subItems[i][j].setNormalColor(Color(10, 15, 56));
            subItems[i][j].setHoverColor(Color(20, 25, 76));
            subItems[i][j].setTextFillColor(Color::White);
            subItems[i][j].setOutlineThickness(0);
        }
    }

    arrow_down_tex.setScale({0.5, 0.5});
    arrow_up_tex.setScale({0.5, 0.5});

    // Content area
    contentArea.setPosition({300.f, 0.f});
    contentArea.setSize({(float)window.getSize().x - 300.f, (float)window.getSize().y});
    contentArea.setFillColor(Color(245, 245, 250));

    windowPtr = &window;
    currentView = AdminView::NONE;

    // Initialize movie form
    movieForm = new AdminMovieForm(uppercase_font, lowercase_font);

    // Load movie data from CSV
    loadMovies("../data/movies.csv");
}

AdminScreen::~AdminScreen() {
    delete movieForm;
}

void AdminScreen::draw(RenderWindow& window) {
    window.clear(Color::White);
    window.draw(sidebar);
    window.draw(admin_logo);
    window.draw(admin_logoText);

    float currentY = 80.f;
    for (int i = 0; i < menuItems.size(); i++) {
        menuItems[i].setPosition({0.f, currentY});
        menuItems[i].setTextPosition(20.f, 15.f);
        menuItems[i].draw(window);

        if (expanded[i]) 
            arrow_up_tex.setPosition({150.f, currentY - 95.f});
        else 
            arrow_down_tex.setPosition({150.f, currentY - 95.f});
        window.draw(expanded[i] ?  arrow_up_tex : arrow_down_tex);

        float extraHeight = 0.f;
        if (expanded[i]) {
            for (int j = 0; j < subItems[i].size(); j++) {
                float subY = currentY + 60.f + j * 40.f;
                subItems[i][j].setPosition({0.f, subY});
                subItems[i][j].setTextPosition(40.f, 10.f);
                subItems[i][j].draw(window);
            }
            extraHeight += 40.f * subItems[i].size();
        }
        currentY += 60.f + extraHeight;
    }

    window.draw(contentArea);
    // Draw breadcrumb for current subview
    wstring crumb = L"Dashboard";
    switch (currentView) {
        case AdminView::MOVIE_LIST:
            crumb += L" / Danh sách phim";
            drawBreadcrumb(window, crumb);
            drawMovieList(window);
            break;
        case AdminView::MOVIE_ADD:
            crumb += L" / Danh sách phim / Thêm phim mới";
            drawBreadcrumb(window, crumb);
            movieForm->setMode(MovieFormMode::CREATE);
            movieForm->draw(window);
            break;
        case AdminView::MOVIE_EDIT:
            crumb += L" / Danh sách phim / Chỉnh sửa phim";
            drawBreadcrumb(window, crumb);
            movieForm->setMode(MovieFormMode::EDIT);
            movieForm->draw(window);
            break;
        case AdminView::MOVIE_DELETE:
            crumb += L" / Danh sách phim / Xoá phim";
            drawBreadcrumb(window, crumb);
            drawDeleteMovieForm(window);
            break;
        default:
            drawBreadcrumb(window, crumb);
            break;
    }
}

void AdminScreen::update(Vector2f mousePos, bool mousePressed) {
    for (int i = 0; i < menuItems.size(); i++) 
        menuItems[i].update(mousePos);

    for (int i = 0; i < subItems.size(); i++) 
        if (expanded[i]) 
            for (int j = 0; j < subItems[i].size(); j++) 
                subItems[i][j].update(mousePos);

    // Update movie form if active
    if (currentView == AdminView::MOVIE_ADD || currentView == AdminView::MOVIE_EDIT) {
        movieForm->update(mousePos, mousePressed);
        
        // Handle back button click
        if (movieForm->isBackClicked(mousePos, mousePressed)) {
            currentView = AdminView::MOVIE_LIST;
        }
        
        // Handle save button click
        if (movieForm->isSaveClicked(mousePos, mousePressed)) {
            Movie newMovie = movieForm->getMovieData();
            
            if (currentView == AdminView::MOVIE_ADD) {
                movies.push_back(newMovie);
            } else if (currentView == AdminView::MOVIE_EDIT && selectedMovieIndex >= 0) {
                movies[selectedMovieIndex] = newMovie;
            }
            
            saveMovies("../data/movies.csv");
            currentView = AdminView::MOVIE_LIST;
        }
        
        // Handle cancel button click
        if (movieForm->isCancelClicked(mousePos, mousePressed)) {
            currentView = AdminView::MOVIE_LIST;
        }
    }

    static bool mouseReleased = true;
    if (mousePressed && mouseReleased) {
        for (int i = 0; i < menuItems.size(); i++) {
            if (menuItems[i].isClicked(mousePos, mousePressed)) {
                expanded[i] = !expanded[i];
                break;
            }
        }
        
        // Handle sub-item clicks
        for (int i = 0; i < subItems.size(); i++) {
            if (expanded[i]) {
                for (int j = 0; j < subItems[i].size(); j++) {
                    if (subItems[i][j].isClicked(mousePos, mousePressed)) {
                        // Select the clicked sub-item and deselect others
                        for (int ii = 0; ii < (int)subItems.size(); ++ii) {
                            for (int jj = 0; jj < (int)subItems[ii].size(); ++jj) {
                                subItems[ii][jj].setSelected(ii == i && jj == j);
                            }
                        }
                        break;
                    }
                }
            }
            if (expanded[2]) {
                for (int j = 0; j < subItems[2].size(); j++) {
                    if (subItems[2][j].isClicked(mousePos, mousePressed)) {
                        if (j == 0) currentView = AdminView::MOVIE_LIST;
                        if (j == 1) {
                            currentView = AdminView::MOVIE_ADD;
                            movieForm->setMode(MovieFormMode::CREATE);
                        }
                        if (j == 2) {
                            currentView = AdminView::MOVIE_EDIT;
                            movieForm->setMode(MovieFormMode::EDIT);
                            if (!movies.empty()) {
                                selectedMovieIndex = 0;
                                movieForm->loadMovieData(movies[0]);
                            }
                        }
                        if (j == 3) currentView = AdminView::MOVIE_DELETE;
                    }
                }
            }
        }
        mouseReleased = false;
    }
    else if (!mousePressed) 
        mouseReleased = true;
}

void AdminScreen::handleEvent(const Event& event, Vector2f mousePos, bool mousePressed) {
    if (currentView == AdminView::MOVIE_ADD || currentView == AdminView::MOVIE_EDIT) {
        if (const auto* textEntered = event.getIf<Event::TextEntered>()) {
            movieForm->handleTextInput(textEntered->unicode);
        }
        else if (const auto* keyPressed = event.getIf<Event::KeyPressed>()) {
            movieForm->handleKeyPress(keyPressed->code);
        }
    }
}

void AdminScreen::drawBreadcrumb(RenderWindow& window, const wstring& path) {
    // Draw a light gray bar at the top of the content area
    RectangleShape bar({contentArea.getSize().x, 50.f});
    bar.setPosition(contentArea.getPosition());
    bar.setFillColor(Color(248, 249, 251)); // #F8F9FB
    // subtle bottom border
    RectangleShape border({contentArea.getSize().x, 1.f});
    border.setPosition({contentArea.getPosition().x, contentArea.getPosition().y + 50.f - 1.f});
    border.setFillColor(Color(229, 231, 235)); // #E5E7EB

    window.draw(bar);
    window.draw(border);

    Text t(lowercase_font, path, 14);
    t.setPosition({contentArea.getPosition().x + 20.f, contentArea.getPosition().y + 14.f});
    t.setFillColor(Color(107, 114, 128));
    window.draw(t);
}

// ==================== MOVIE MANAGEMENT =====================
wstring AdminScreen::stringToWString(const string& str) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

void AdminScreen::loadMovies(const string& path) {
    movies.clear();
    ifstream file(path);
    if (!file.is_open()) return;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        stringstream ss(line);
        Movie m;
        getline(ss, m.film_id, ',');
        getline(ss, m.title, ',');
        getline(ss, m.age_rating, ',');
        getline(ss, m.country, ',');
        getline(ss, m.language, ',');
        getline(ss, m.genres, ',');
        getline(ss, m.duration_min, ',');
        getline(ss, m.release_date, ',');
        getline(ss, m.director, ',');
        getline(ss, m.cast, ',');
        getline(ss, m.synopsis, ',');
        getline(ss, m.poster_path, ',');
        getline(ss, m.status, ',');
        movies.push_back(m);
    }
    file.close();
}

void AdminScreen::saveMovies(const string& path) {
    // Write CSV using wide stream with UTF-8 encoding to preserve Vietnamese characters
    std::wofstream wof(path);
    if (!wof.is_open()) return;
    try {
        wof.imbue(std::locale(wof.getloc(), new std::codecvt_utf8<wchar_t>()));
    } catch (...) {}

    wof << L"film_id,title,age_rating,country,language,genres,duration_min,release_date,director,cast,synopsis,poster_path,status\n";
    for (const auto& m : movies) {
        wof << stringToWString(m.film_id) << L"," << stringToWString(m.title) << L"," << stringToWString(m.age_rating) << L"," << stringToWString(m.country)
            << L"," << stringToWString(m.language) << L"," << stringToWString(m.genres) << L"," << stringToWString(m.duration_min) << L"," << stringToWString(m.release_date)
            << L"," << stringToWString(m.director) << L"," << stringToWString(m.cast) << L"," << stringToWString(m.synopsis) << L"," << stringToWString(m.poster_path)
            << L"," << stringToWString(m.status) << L"\n";
    }
    wof.close();
}

void AdminScreen::drawMovieList(RenderWindow& window) {
    // Header
    Text header(uppercase_font, L"DANH SÁCH PHIM", 28);
    header.setPosition({350.f, 30.f});
    header.setFillColor(Color(13, 16, 69));
    window.draw(header);

    // Table header
    RectangleShape tableHeader({1350.f, 50.f});
    tableHeader.setPosition({350.f, 100.f});
    tableHeader.setFillColor(Color(59, 108, 177));
    window.draw(tableHeader);

    vector<wstring> columns = {L"ID", L"Tên phim", L"Thể loại", L"Thời lượng", L"Ngày ra", L"Trạng thái"};
    vector<float> columnX = {360.f, 450.f, 800.f, 1050.f, 1250.f, 1450.f};
    
    for (int i = 0; i < columns.size(); i++) {
        Text col(lowercase_font, columns[i], 18);
        col.setPosition({columnX[i], 115.f});
        col.setFillColor(Color::White);
        col.setStyle(Text::Bold);
        window.draw(col);
    }

    // Table rows
    float rowY = 160.f;
    int start = currentPage * itemsPerPage;
    int end = min(start + itemsPerPage, (int)movies.size());
    
    for (int i = start; i < end; i++) {
        RectangleShape row({1350.f, 45.f});
        row.setPosition({350.f, rowY});
        row.setFillColor(i % 2 == 0 ? Color(255, 255, 255) : Color(240, 242, 245));
        window.draw(row);

        wstring id = stringToWString(movies[i].film_id);
        wstring title = stringToWString(movies[i].title);
        wstring genre = stringToWString(movies[i].genres);
        wstring duration = stringToWString(movies[i].duration_min + " phút");
        wstring date = stringToWString(movies[i].release_date);
        wstring status = stringToWString(movies[i].status);

        // Truncate long text
        if (title.length() > 25) title = title.substr(0, 25) + L"...";
        if (genre.length() > 20) genre = genre.substr(0, 20) + L"...";

        vector<wstring> data = {id, title, genre, duration, date, status};
        
        for (int j = 0; j < data.size(); j++) {
            Text t(lowercase_font, data[j], 16);
            t.setPosition({columnX[j], rowY + 12.f});
            t.setFillColor(Color::Black);
            window.draw(t);
        }

        rowY += 45.f;
    }

    // Pagination info
    wstring pageInfo = L"Trang " + to_wstring(currentPage + 1) + L" / " + to_wstring((movies.size() + itemsPerPage - 1) / itemsPerPage);
    Text pageText(lowercase_font, pageInfo, 16);
    pageText.setPosition({1550.f, 850.f});
    pageText.setFillColor(Color::Black);
    window.draw(pageText);

    // Total movies
    wstring total = L"Tổng: " + to_wstring(movies.size()) + L" phim";
    Text totalText(lowercase_font, total, 16);
    totalText.setPosition({350.f, 850.f});
    totalText.setFillColor(Color::Black);
    window.draw(totalText);
}

void AdminScreen::drawAddMovieForm(RenderWindow& window) {
    // Header
    Text header(uppercase_font, L"THÊM PHIM MỚI", 28);
    header.setPosition({350.f, 30.f});
    header.setFillColor(Color(13, 16, 69));
    window.draw(header);

    // Form container
    RectangleShape formBg({1200.f, 750.f});
    formBg.setPosition({400.f, 100.f});
    formBg.setFillColor(Color::White);
    formBg.setOutlineColor(Color(200, 200, 200));
    formBg.setOutlineThickness(2.f);
    window.draw(formBg);

    // Form fields
    vector<wstring> labels = {
        L"ID Phim:", L"Tên phim:", L"Độ tuổi:", L"Quốc gia:", 
        L"Ngôn ngữ:", L"Thể loại:", L"Thời lượng (phút):", L"Ngày phát hành:",
        L"Đạo diễn:", L"Diễn viên:", L"Tóm tắt:", L"Đường dẫn poster:", L"Trạng thái:"
    };

    float labelX = 450.f;
    float fieldY = 140.f;
    float fieldGap = 55.f;

    for (int i = 0; i < labels.size(); i++) {
        Text label(lowercase_font, labels[i], 18);
        label.setPosition({labelX, fieldY + i * fieldGap});
        label.setFillColor(Color::Black);
        window.draw(label);

        RectangleShape inputBox({800.f, 40.f});
        inputBox.setPosition({labelX + 220.f, fieldY + i * fieldGap - 5.f});
        inputBox.setFillColor(Color(245, 245, 245));
        inputBox.setOutlineColor(Color(150, 150, 150));
        inputBox.setOutlineThickness(1.f);
        window.draw(inputBox);
    }

    // Buttons
    RectangleShape saveBtn({150.f, 45.f});
    saveBtn.setPosition({1150.f, 850.f});
    saveBtn.setFillColor(Color(34, 139, 34));
    window.draw(saveBtn);
    Text saveText(lowercase_font, L"Lưu", 20);
    saveText.setPosition({1200.f, 860.f});
    saveText.setFillColor(Color::White);
    window.draw(saveText);

    RectangleShape cancelBtn({150.f, 45.f});
    cancelBtn.setPosition({980.f, 850.f});
    cancelBtn.setFillColor(Color(200, 50, 50));
    window.draw(cancelBtn);
    Text cancelText(lowercase_font, L"Hủy", 20);
    cancelText.setPosition({1035.f, 860.f});
    cancelText.setFillColor(Color::White);
    window.draw(cancelText);
}

void AdminScreen::drawEditMovieForm(RenderWindow& window) {
    // Header
    Text header(uppercase_font, L"CHỈNH SỬA PHIM", 28);
    header.setPosition({350.f, 30.f});
    header.setFillColor(Color(13, 16, 69));
    window.draw(header);

    // Movie selector dropdown
    Text selectLabel(lowercase_font, L"Chọn phim cần sửa:", 20);
    selectLabel.setPosition({400.f, 100.f});
    selectLabel.setFillColor(Color::Black);
    window.draw(selectLabel);

    RectangleShape dropdown({500.f, 45.f});
    dropdown.setPosition({650.f, 95.f});
    dropdown.setFillColor(Color::White);
    dropdown.setOutlineColor(Color(150, 150, 150));
    dropdown.setOutlineThickness(2.f);
    window.draw(dropdown);

    if (selectedMovieIndex >= 0 && selectedMovieIndex < movies.size()) {
        wstring movieTitle = stringToWString(movies[selectedMovieIndex].title);
        Text selected(lowercase_font, movieTitle, 18);
        selected.setPosition({665.f, 107.f});
        selected.setFillColor(Color::Black);
        window.draw(selected);
    } else {
        Text placeholder(lowercase_font, L"-- Chọn phim --", 18);
        placeholder.setPosition({665.f, 107.f});
        placeholder.setFillColor(Color(150, 150, 150));
        window.draw(placeholder);
    }

    // Form (similar to Add Movie)
    if (selectedMovieIndex >= 0) {
        RectangleShape formBg({1200.f, 650.f});
        formBg.setPosition({400.f, 170.f});
        formBg.setFillColor(Color::White);
        formBg.setOutlineColor(Color(200, 200, 200));
        formBg.setOutlineThickness(2.f);
        window.draw(formBg);

        vector<wstring> labels = {
            L"Tên phim:", L"Độ tuổi:", L"Quốc gia:", L"Ngôn ngữ:", 
            L"Thể loại:", L"Thời lượng:", L"Ngày phát hành:", L"Đạo diễn:",
            L"Diễn viên:", L"Tóm tắt:", L"Trạng thái:"
        };

        float labelX = 450.f;
        float fieldY = 210.f;
        float fieldGap = 55.f;

        for (int i = 0; i < labels.size(); i++) {
            Text label(lowercase_font, labels[i], 18);
            label.setPosition({labelX, fieldY + i * fieldGap});
            label.setFillColor(Color::Black);
            window.draw(label);

            RectangleShape inputBox({800.f, 40.f});
            inputBox.setPosition({labelX + 220.f, fieldY + i * fieldGap - 5.f});
            inputBox.setFillColor(Color(245, 245, 245));
            inputBox.setOutlineColor(Color(150, 150, 150));
            inputBox.setOutlineThickness(1.f);
            window.draw(inputBox);
        }

        // Buttons
        RectangleShape updateBtn({150.f, 45.f});
        updateBtn.setPosition({1150.f, 850.f});
        updateBtn.setFillColor(Color(255, 140, 0));
        window.draw(updateBtn);
        Text updateText(lowercase_font, L"Cập nhật", 20);
        updateText.setPosition({1175.f, 860.f});
        updateText.setFillColor(Color::White);
        window.draw(updateText);

        RectangleShape cancelBtn({150.f, 45.f});
        cancelBtn.setPosition({980.f, 850.f});
        cancelBtn.setFillColor(Color(200, 50, 50));
        window.draw(cancelBtn);
        Text cancelText(lowercase_font, L"Hủy", 20);
        cancelText.setPosition({1035.f, 860.f});
        cancelText.setFillColor(Color::White);
        window.draw(cancelText);
    }
}

void AdminScreen::drawDeleteMovieForm(RenderWindow& window) {
    // Header
    Text header(uppercase_font, L"XÓA PHIM", 28);
    header.setPosition({350.f, 30.f});
    header.setFillColor(Color(13, 16, 69));
    window.draw(header);

    // Movie selector
    Text selectLabel(lowercase_font, L"Chọn phim cần xóa:", 20);
    selectLabel.setPosition({400.f, 100.f});
    selectLabel.setFillColor(Color::Black);
    window.draw(selectLabel);

    RectangleShape dropdown({500.f, 45.f});
    dropdown.setPosition({650.f, 95.f});
    dropdown.setFillColor(Color::White);
    dropdown.setOutlineColor(Color(150, 150, 150));
    dropdown.setOutlineThickness(2.f);
    window.draw(dropdown);

    if (selectedMovieIndex >= 0 && selectedMovieIndex < movies.size()) {
        wstring movieTitle = stringToWString(movies[selectedMovieIndex].title);
        Text selected(lowercase_font, movieTitle, 18);
        selected.setPosition({665.f, 107.f});
        selected.setFillColor(Color::Black);
        window.draw(selected);

        // Confirmation box
        RectangleShape confirmBox({700.f, 300.f});
        confirmBox.setPosition({550.f, 250.f});
        confirmBox.setFillColor(Color::White);
        confirmBox.setOutlineColor(Color(220, 53, 69));
        confirmBox.setOutlineThickness(3.f);
        window.draw(confirmBox);

        // Warning icon
        Text warningIcon(uppercase_font, L"⚠", 60);
        warningIcon.setPosition({870.f, 280.f});
        warningIcon.setFillColor(Color(220, 53, 69));
        window.draw(warningIcon);

        // Confirmation message
        Text confirmMsg1(lowercase_font, L"Bạn có chắc chắn muốn xóa phim này?", 22);
        confirmMsg1.setPosition({620.f, 380.f});
        confirmMsg1.setFillColor(Color::Black);
        window.draw(confirmMsg1);

        Text confirmMsg2(lowercase_font, L"Hành động này không thể hoàn tác!", 18);
        confirmMsg2.setPosition({650.f, 420.f});
        confirmMsg2.setFillColor(Color(220, 53, 69));
        window.draw(confirmMsg2);

        // Buttons
        RectangleShape deleteBtn({150.f, 45.f});
        deleteBtn.setPosition({950.f, 480.f});
        deleteBtn.setFillColor(Color(220, 53, 69));
        window.draw(deleteBtn);
        Text deleteText(lowercase_font, L"Xóa", 20);
        deleteText.setPosition({1005.f, 490.f});
        deleteText.setFillColor(Color::White);
        window.draw(deleteText);

        RectangleShape cancelBtn({150.f, 45.f});
        cancelBtn.setPosition({780.f, 480.f});
        cancelBtn.setFillColor(Color(108, 117, 125));
        window.draw(cancelBtn);
        Text cancelText(lowercase_font, L"Hủy", 20);
        cancelText.setPosition({835.f, 490.f});
        cancelText.setFillColor(Color::White);
        window.draw(cancelText);
    } else {
        Text placeholder(lowercase_font, L"-- Chọn phim --", 18);
        placeholder.setPosition({665.f, 107.f});
        placeholder.setFillColor(Color(150, 150, 150));
        window.draw(placeholder);
    }
}


