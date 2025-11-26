#include "UI/components/Admin/MoviePanel.h"
#include "UI/components/Admin/RoundedRectRenderer.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <array>

MoviePanel::MoviePanel(Font& font, float width, float height)
        : font(font), width(width), height(height), currentPopup(NONE),
            selectedRow(-1), scrollOffset(0), hoveredRow(-1), 
            reloadTexture("../assets/elements/reload.png"),
            reloadSprite(reloadTexture) {
    
    // Initialize repository
    repository = make_unique<AdminMovieRepository>("../data/movies.txt");
    
    // Setup UI
    setupUI();
}

void MoviePanel::setupUI() {
    // Background - make it smaller to not cover buttons
    background.setSize(Vector2f(width, height)); // Leave space for buttons at bottom
    background.setFillColor(bgColor);
    
    // Title (reduced size for compact layout)
    string titleStr = "Quản lý phim";
    titleText = make_unique<Text>(font, String::fromUtf8(titleStr.begin(), titleStr.end()), 26);
    titleText->setFillColor(Color(27, 38, 59)); // #1B263B
    titleText->setStyle(Text::Bold);
    
    // Table header background
    tableHeaderBg.setSize(Vector2f(TABLE_WIDTH, HEADER_HEIGHT));
    tableHeaderBg.setFillColor(headerColor);
    
    // CRUD Buttons (aligned top-left, evenly spaced)
    const float btnW = 150.f;
    const float btnH = 44.f;
    const float spacing = 18.f;

    // Add button
    btnAddBg.setSize(Vector2f(btnW, btnH));
    btnAddBg.setFillColor(Color(20,118,172)); // #1476AC
    btnAddBg.setOutlineThickness(0.f);

    string btnAddStr = "Thêm phim";
    btnAddText = make_unique<Text>(font);
    btnAddText->setString(String::fromUtf8(btnAddStr.begin(), btnAddStr.end()));
    btnAddText->setCharacterSize(16);
    btnAddText->setFillColor(Color::White);

    // Edit button
    btnEditBg.setSize(Vector2f(btnW, btnH));
    btnEditBg.setFillColor(Color(233,164,0)); // #E9A400
    btnEditBg.setOutlineThickness(0.f);

    string btnEditStr = "Sửa thông tin";
    btnEditText = make_unique<Text>(font);
    btnEditText->setString(String::fromUtf8(btnEditStr.begin(), btnEditStr.end()));
    btnEditText->setCharacterSize(16);
    btnEditText->setFillColor(Color::White);

    // Delete button
    btnDeleteBg.setSize(Vector2f(btnW, btnH));
    btnDeleteBg.setFillColor(Color(211,47,47)); // #D32F2F
    btnDeleteBg.setOutlineThickness(0.f);

    string btnDeleteStr = "Xóa phim";
    btnDeleteText = make_unique<Text>(font);
    btnDeleteText->setString(String::fromUtf8(btnDeleteStr.begin(), btnDeleteStr.end()));
    btnDeleteText->setCharacterSize(16);
    btnDeleteText->setFillColor(Color::White);

    reloadButtonBg.setSize(Vector2f(btnH, btnH));
    reloadButtonBg.setFillColor(Color(20,118,172));
    reloadButtonBg.setOutlineThickness(0.f);

    reloadTexture.setSmooth(true);
    reloadSprite.setScale({0.1, 0.1});
    reloadSprite.setColor(Color::White);
    
    // Notification setup
    notificationBg.setSize(Vector2f(400, 60));
    notificationBg.setFillColor(Color(20, 118, 172, 230));
    notificationTextObj = make_unique<Text>(font);
    notificationTextObj->setCharacterSize(15);
    notificationTextObj->setFillColor(Color::White);
}

void MoviePanel::setPosition(Vector2f pos) {
    position = pos;
    background.setPosition(pos);
    
    if (titleText) titleText->setPosition(Vector2f(pos.x + 40, pos.y + 20));
    
    tableHeaderBg.setPosition(Vector2f(pos.x + TABLE_X, pos.y + TABLE_Y));

    // Align CRUD buttons in a single row above the table
    const float buttonRowY = TABLE_Y - 64.f;
    const float spacing = 18.f;
    const float btnHeight = btnAddBg.getSize().y;

    btnAddBg.setPosition(Vector2f(pos.x + TABLE_X, pos.y + buttonRowY));
    btnEditBg.setPosition(Vector2f(btnAddBg.getPosition().x + btnAddBg.getSize().x + spacing, pos.y + buttonRowY));
    btnDeleteBg.setPosition(Vector2f(btnEditBg.getPosition().x + btnEditBg.getSize().x + spacing, pos.y + buttonRowY));
    float reloadX = btnDeleteBg.getPosition().x + btnDeleteBg.getSize().x + spacing;
    float reloadY = pos.y + buttonRowY;

    reloadButtonBg.setPosition({reloadX, reloadY});

    FloatRect spriteBounds = reloadSprite.getGlobalBounds();
    reloadSprite.setPosition({reloadX + (reloadButtonBg.getSize().x - spriteBounds.size.x) / 2.f, reloadY + (btnHeight - spriteBounds.size.y) / 2.f});
}

void MoviePanel::openAddPopup() {
    currentPopup = ADD;
    
    // Popup overlay (full dark like RegisterScreen)
    popupOverlay.setSize(Vector2f(1728, 972));
    popupOverlay.setPosition(Vector2f(0, 0));
    popupOverlay.setFillColor(Color(0, 0, 0, 170));
    
    // Popup background - WIDER for 2 columns, shorter height
    const float popupW = 940.f;  // Wide enough for 2 columns
    const float popupH = 720.f;  // Taller to avoid overlapping controls
    const float popupX = (1728.f - popupW) / 2.f;
    const float popupY = (972.f - popupH) / 2.f;
    
    popupBackground.setSize(Vector2f(popupW, popupH));
    popupBackground.setPosition(Vector2f(popupX, popupY));
    popupBackground.setFillColor(Color(255, 255, 255, 240)); // Semi-transparent white
    popupBackground.setOutlineThickness(0);
    
    // Popup title (RegisterScreen style - bold, black, 26px)
    string popupTitleStr = "Thêm phim mới";
    popupTitle = make_unique<Text>(font, String::fromUtf8(popupTitleStr.begin(), popupTitleStr.end()), 26);
    popupTitle->setFillColor(Color::Black);
    popupTitle->setPosition(Vector2f(popupX + (popupW - 200) / 2, popupY + 20));
    
    // TWO COLUMNS LAYOUT
    // Left column: 6 fields (0-5), Right column: 5 fields (6-10) + dropdown
    const float colSpacing = 40.f;  // Space between columns
    const float leftColX = popupX + 40;
    const float rightColX = popupX + popupW / 2 + 20;
    const float inputW = 420.f;  // Increased from 400 to 420 for better fit
    const float inputH = 40.f;
    const float inputStartY = popupY + 100;
    const float inputSpacing = 76.f;
    
    // Fields definition (11 TextBox fields + 1 Dropdown)
    vector<pair<string, string>> fields = {
        // Left column (0-5)
        {"Tên phim*", "Nhập tên phim"},
        {"Phân loại tuổi*", "T13, T16, T18, PG"},
        {"Quốc gia*", "Việt Nam, USA..."},
        {"Ngôn ngữ*", "Tiếng Việt, English"},
        {"Thể loại*", "Hành động, Tâm lý"},
        {"Thời lượng (phút)*", "120"},
        // Right column (6-10)
        {"Ngày khởi chiếu*", "dd/mm/yyyy"},
        {"Đạo diễn*", "Tên đạo diễn"},
        {"Diễn viên*", "Danh sách diễn viên"},
        {"Tóm tắt*", "Mô tả phim"},
        {"Poster", "../assets/posters/..."}
        // Trạng thái will be dropdown (handled separately)
    };
    
    inputBoxes.clear();
    for (size_t i = 0; i < fields.size(); i++) {
        float x = (i < 6) ? leftColX : rightColX;  // Left or right column
        float y = inputStartY + (i % 6) * inputSpacing;
        
        auto box = make_unique<TextBox>(font, fields[i].first, x, y, inputW, inputH);
        box->setPlaceholder(fields[i].second);
        inputBoxes.push_back(move(box));
    }
    
    // Status dropdown (right column, row 6)
    float dropdownY = inputStartY + 5 * inputSpacing;
    statusDropdown = make_unique<DropdownBox>(font, "Trạng thái*", rightColX, dropdownY, inputW, inputH);
    vector<string> statusOptions = {"Đang chiếu", "Sắp chiếu", "Đã chiếu"};
    statusDropdown->setOptions(statusOptions);
    statusDropdown->setSelectedIndex(0); // Default: "Đang chiếu"
    
    // Popup buttons (RegisterScreen style: blue "Lưu", gray "Quay lại")
    const float btnW = 190.f;
    const float btnH = 48.f;
    const float btnY = popupY + popupH - btnH - 32.f;
    const float btnSpacing = 24.f;
    const float btnTotalW = btnW * 2 + btnSpacing;
    const float btnStartX = popupX + (popupW - btnTotalW) / 2.f;
    
    string btnSaveStr = "Lưu";
    string btnCancelStr = "Quay lại";
    btnPopupSave = make_unique<Button>(font, String::fromUtf8(btnSaveStr.begin(), btnSaveStr.end()), btnW, btnH, 18);
    btnPopupSave->setPosition(Vector2f(btnStartX, btnY));
    btnPopupSave->setFillColor(Color(100, 149, 237)); // Blue like RegisterScreen
    btnPopupSave->setTextColor(Color::White);
    
    btnPopupCancel = make_unique<Button>(font, String::fromUtf8(btnCancelStr.begin(), btnCancelStr.end()), btnW, btnH, 18);
    btnPopupCancel->setPosition(Vector2f(btnStartX + btnW + btnSpacing, btnY));
    btnPopupCancel->setFillColor(Color(90, 90, 90)); // Gray like RegisterScreen
    btnPopupCancel->setTextColor(Color::White);
}

void MoviePanel::openEditPopup() {
    if (selectedRow < 0) {
        showSelectionWarning("Vui lòng chọn phim trước khi sửa.");
        return;
    }
    
    currentPopup = EDIT;
    
    // Same layout as Add popup (2-column)
    popupOverlay.setSize(Vector2f(1728, 972));
    popupOverlay.setPosition(Vector2f(0, 0));
    popupOverlay.setFillColor(Color(0, 0, 0, 170));
    
    const float popupW = 940.f;
    const float popupH = 720.f;
    const float popupX = (1728.f - popupW) / 2.f;
    const float popupY = (972.f - popupH) / 2.f;
    
    popupBackground.setSize(Vector2f(popupW, popupH));
    popupBackground.setPosition(Vector2f(popupX, popupY));
    popupBackground.setFillColor(Color(255, 255, 255, 240));
    popupBackground.setOutlineThickness(0);
    
    string editTitleStr = "Sửa thông tin phim";
    popupTitle = make_unique<Text>(font, String::fromUtf8(editTitleStr.begin(), editTitleStr.end()), 26);
    popupTitle->setFillColor(Color::Black);
    popupTitle->setPosition(Vector2f(popupX + (popupW - 240) / 2, popupY + 20));
    
    // Get current record data
    vector<string> record = repository->getRecord(selectedRow);
    
    // TWO COLUMNS LAYOUT (same as Add)
    const float colSpacing = 40.f;
    const float leftColX = popupX + 40;
    const float rightColX = popupX + popupW / 2 + 20;
    const float inputW = 420.f;
    const float inputH = 40.f;
    const float inputStartY = popupY + 100;
    const float inputSpacing = 76.f;
    
    vector<pair<string, string>> fields = {
        {"Tên phim*", "Nhập tên phim"},
        {"Phân loại tuổi*", "T13, T16, T18, PG"},
        {"Quốc gia*", "Việt Nam, USA..."},
        {"Ngôn ngữ*", "Tiếng Việt, English"},
        {"Thể loại*", "Hành động, Tâm lý"},
        {"Thời lượng (phút)*", "120"},
        {"Ngày khởi chiếu*", "dd/mm/yyyy"},
        {"Đạo diễn*", "Tên đạo diễn"},
        {"Diễn viên*", "Danh sách diễn viên"},
        {"Tóm tắt*", "Mô tả phim"},
        {"Poster", "../assets/posters/..."}
    };
    
    inputBoxes.clear();
    for (size_t i = 0; i < fields.size(); i++) {
        float x = (i < 6) ? leftColX : rightColX;
        float y = inputStartY + (i % 6) * inputSpacing;
        
        auto box = make_unique<TextBox>(font, fields[i].first, x, y, inputW, inputH);
        box->setPlaceholder(fields[i].second);
        inputBoxes.push_back(move(box));
    }
    
    // Status dropdown
    float dropdownY = inputStartY + 5 * inputSpacing;
    statusDropdown = make_unique<DropdownBox>(font, "Trạng thái*", rightColX, dropdownY, inputW, inputH);
    vector<string> statusOptions = {"Đang chiếu", "Sắp chiếu", "Đã chiếu"};
    statusDropdown->setOptions(statusOptions);
    
    // Pre-fill values from selected record
    if (record.size() >= 13) {
        inputBoxes[0]->setValue(record[1]);   // title
        inputBoxes[1]->setValue(record[2]);   // age_rating
        inputBoxes[2]->setValue(record[3]);   // country
        inputBoxes[3]->setValue(record[4]);   // language
        inputBoxes[4]->setValue(record[5]);   // genres
        inputBoxes[5]->setValue(record[6]);   // duration_min
        inputBoxes[6]->setValue(record[7]);   // release_date
        inputBoxes[7]->setValue(record[8]);   // director
        inputBoxes[8]->setValue(record[9]);   // cast
        inputBoxes[9]->setValue(record[10]);  // synopsis
        inputBoxes[10]->setValue(record[11]); // poster_path
        statusDropdown->setSelectedValue(record[12]); // status (dropdown)
    }
    
    // Buttons (same as Add popup)
    const float btnW = 190.f;
    const float btnH = 48.f;
    const float btnY = popupY + popupH - btnH - 32.f;
    const float btnSpacing = 24.f;
    const float btnTotalW = btnW * 2 + btnSpacing;
    const float btnStartX = popupX + (popupW - btnTotalW) / 2.f;
    
    string btnSaveStr = "Lưu";
    string btnCancelStr = "Quay lại";
    btnPopupSave = make_unique<Button>(font, String::fromUtf8(btnSaveStr.begin(), btnSaveStr.end()), btnW, btnH, 18);
    btnPopupSave->setPosition(Vector2f(btnStartX, btnY));
    btnPopupSave->setFillColor(Color(100, 149, 237));
    btnPopupSave->setTextColor(Color::White);
    
    btnPopupCancel = make_unique<Button>(font, String::fromUtf8(btnCancelStr.begin(), btnCancelStr.end()), btnW, btnH, 18);
    btnPopupCancel->setPosition(Vector2f(btnStartX + btnW + btnSpacing, btnY));
    btnPopupCancel->setFillColor(Color(90, 90, 90));
    btnPopupCancel->setTextColor(Color::White);
}

void MoviePanel::openDeleteConfirm() {
    if (selectedRow < 0) {
        showSelectionWarning("Vui lòng chọn phim trước khi xóa.");
        return;
    }
    
    currentPopup = DELETE_CONFIRM;
    
    popupOverlay.setSize(Vector2f(1728, 972));
    popupOverlay.setPosition(Vector2f(0, 0));
    popupOverlay.setFillColor(Color(0, 0, 0, 128));
    
    // Delete confirmation popup (compact: 380x160)
    const float popupW = 380.f;
    const float popupH = 160.f;
    const float popupX = (1728.f - popupW) / 2.f;
    const float popupY = (972.f - popupH) / 2.f;
    
    popupBackground.setSize(Vector2f(popupW, popupH));
    popupBackground.setPosition(Vector2f(popupX, popupY));
    popupBackground.setFillColor(Color::White);
    popupBackground.setOutlineThickness(1);
    popupBackground.setOutlineColor(borderColor);
    
    string deleteTitleStr = "Xác nhận xóa phim này?";
    popupTitle = make_unique<Text>(font, String::fromUtf8(deleteTitleStr.begin(), deleteTitleStr.end()), 16);
    popupTitle->setFillColor(textColor);
    
    // Center title
    FloatRect titleBounds = popupTitle->getLocalBounds();
    popupTitle->setPosition(Vector2f(
        popupX + (popupW - titleBounds.size.x) / 2.f,
        popupY + 30
    ));
    
    // Buttons (centered, compact)
    const float btnW = 100.f;
    const float btnH = 36.f;
    const float btnY = popupY + popupH - btnH - 20;
    const float btnSpacing = 20.f;
    const float btnTotalW = btnW * 2 + btnSpacing;
    const float btnStartX = popupX + (popupW - btnTotalW) / 2.f;
    
    string btnDeleteStr = "Xóa";
    string btnCancelStr3 = "Hủy";
    btnPopupSave = make_unique<Button>(font, String::fromUtf8(btnDeleteStr.begin(), btnDeleteStr.end()), btnW, btnH, 16);
    btnPopupSave->setPosition(Vector2f(btnStartX, btnY));
    btnPopupSave->setFillColor(Color(211, 47, 47)); // Red for delete
    btnPopupSave->setTextColor(Color::White);
    
    btnPopupCancel = make_unique<Button>(font, String::fromUtf8(btnCancelStr3.begin(), btnCancelStr3.end()), btnW, btnH, 16);
    btnPopupCancel->setPosition(Vector2f(btnStartX + btnW + btnSpacing, btnY));
    btnPopupCancel->setFillColor(Color(160, 160, 160)); // Gray for cancel
    btnPopupCancel->setTextColor(Color::White);
}

void MoviePanel::closePopup() {
    currentPopup = NONE;
    inputBoxes.clear();
    statusDropdown.reset();
    btnPopupSave.reset();
    btnPopupCancel.reset();
}

void MoviePanel::handleAdd() {
    if (inputBoxes.getSize() < 11) return;  // Changed from 12 to 11
    
    // Get all 11 field values from TextBoxes
    string title = inputBoxes[0]->getValue();
    string ageRating = inputBoxes[1]->getValue();
    string country = inputBoxes[2]->getValue();
    string language = inputBoxes[3]->getValue();
    string genres = inputBoxes[4]->getValue();
    string duration = inputBoxes[5]->getValue();
    string releaseDate = inputBoxes[6]->getValue();
    string director = inputBoxes[7]->getValue();
    string cast = inputBoxes[8]->getValue();
    string synopsis = inputBoxes[9]->getValue();
    string posterPath = inputBoxes[10]->getValue();
    
    // Get status from dropdown
    string status = statusDropdown ? statusDropdown->getSelectedValue() : "Dang chieu";
    
    // Validate required fields
    if (title.empty()) {
        showNotification("Vui lòng nhập tên phim!");
        return;
    }
    
    // Create full record with all fields
    vector<string> record = {
        "",                                            // 0. ID (auto-generated)
        title,                                         // 1. title
        ageRating.empty() ? "T13" : ageRating,        // 2. age_rating
        country.empty() ? "Vietnam" : country,        // 3. country
        language.empty() ? "Vietnamese" : language,    // 4. language
        genres.empty() ? "Action" : genres,            // 5. genres
        duration.empty() ? "120" : duration,          // 6. duration_min
        releaseDate.empty() ? "01/01/2025" : releaseDate, // 7. release_date
        director.empty() ? "Unknown" : director,      // 8. director
        cast.empty() ? "Unknown" : cast,              // 9. cast
        synopsis.empty() ? "Description" : synopsis,  // 10. synopsis
        posterPath,                                    // 11. poster_path
        status.empty() ? "Coming Soon" : status       // 12. status
    };
    
    repository->addRecord(record);
    repository->saveToFile();
    repository->loadFromFile();
    
    closePopup();
    showNotification("✅ Đã thêm phim mới thành công!");
}

void MoviePanel::handleEdit() {
    if (selectedRow < 0 || inputBoxes.getSize() < 11) return;  // Changed from 12 to 11
    
    // Get all 11 field values from TextBoxes
    string title = inputBoxes[0]->getValue();
    string ageRating = inputBoxes[1]->getValue();
    string country = inputBoxes[2]->getValue();
    string language = inputBoxes[3]->getValue();
    string genres = inputBoxes[4]->getValue();
    string duration = inputBoxes[5]->getValue();
    string releaseDate = inputBoxes[6]->getValue();
    string director = inputBoxes[7]->getValue();
    string cast = inputBoxes[8]->getValue();
    string synopsis = inputBoxes[9]->getValue();
    string posterPath = inputBoxes[10]->getValue();
    
    // Get status from dropdown
    string status = statusDropdown ? statusDropdown->getSelectedValue() : "Dang chieu";
    
    // Get existing record
    vector<string> record = repository->getRecord(selectedRow);
    if (record.empty()) return;
    
    // Update all fields (keep ID at index 0)
    record[1] = title;
    record[2] = ageRating;
    record[3] = country;
    record[4] = language;
    record[5] = genres;
    record[6] = duration;
    record[7] = releaseDate;
    record[8] = director;
    record[9] = cast;
    record[10] = synopsis;
    record[11] = posterPath;
    record[12] = status;
    
    repository->updateRecord(selectedRow, record);
    repository->saveToFile();
    repository->loadFromFile();
    
    closePopup();
    showNotification("Cap nhat thanh cong!");
    selectedRow = -1;
}

void MoviePanel::handleDelete() {
    if (selectedRow < 0) return;
    
    repository->deleteRecord(selectedRow);
    repository->saveToFile();
    repository->loadFromFile();
    
    closePopup();
    showNotification("🗑️ Đã xóa thành công");
    selectedRow = -1;
}

void MoviePanel::handleReload() {
    repository->loadFromFile();
    selectedRow = -1;
    showNotification("Đã tải lại dữ liệu");
}

void MoviePanel::showNotification(const string& message) {
    notificationText = message;
    notificationVisible = !message.empty();
    if (notificationVisible) {
        notificationClock.restart();
    }
}

void MoviePanel::showSelectionWarning(const string& message) {
    selectionWarningVisible = true;
    selectionWarningMessage = message;
    selectionWarningOverlay.setSize(Vector2f(1728.f, 972.f));
    selectionWarningOverlay.setPosition(Vector2f(0.f, 0.f));
    selectionWarningOverlay.setFillColor(Color(0, 0, 0, 150));

    selectionWarningBg.setSize(Vector2f(420.f, 200.f));
    selectionWarningBg.setFillColor(Color::White);
    selectionWarningBg.setOutlineThickness(0.f);
    selectionWarningBg.setPosition(Vector2f((1728.f - 420.f) / 2.f, (972.f - 200.f) / 2.f));

    selectionWarningText = make_unique<Text>(font, String::fromUtf8(message.begin(), message.end()), 20);
    selectionWarningText->setFillColor(Color(27, 38, 59));
    selectionWarningText->setStyle(Text::Bold);
    FloatRect textBounds = selectionWarningText->getLocalBounds();
    selectionWarningText->setPosition(Vector2f(
        selectionWarningBg.getPosition().x + (selectionWarningBg.getSize().x - textBounds.size.x) / 2.f - textBounds.position.x,
        selectionWarningBg.getPosition().y + 40.f
    ));

    selectionWarningButton.setSize(Vector2f(160.f, 46.f));
    selectionWarningButton.setFillColor(Color(20, 118, 172));
    selectionWarningButton.setPosition(Vector2f(
        selectionWarningBg.getPosition().x + (selectionWarningBg.getSize().x - selectionWarningButton.getSize().x) / 2.f,
        selectionWarningBg.getPosition().y + selectionWarningBg.getSize().y - 70.f
    ));

    selectionWarningButtonText = make_unique<Text>(font, L"Đã hiểu", 18);
    selectionWarningButtonText->setFillColor(Color::White);
    FloatRect btnBounds = selectionWarningButtonText->getLocalBounds();
    selectionWarningButtonText->setPosition(Vector2f(
        selectionWarningButton.getPosition().x + (selectionWarningButton.getSize().x - btnBounds.size.x) / 2.f - btnBounds.position.x,
        selectionWarningButton.getPosition().y + (selectionWarningButton.getSize().y - btnBounds.size.y) / 2.f - btnBounds.position.y
    ));
    selectionWarningButtonHover = false;
}

void MoviePanel::handleSelectionWarningEvent(const Event& event, const RenderWindow& window) {
    if (!selectionWarningVisible) return;
    if (const auto* mouseEvent = event.getIf<Event::MouseButtonPressed>()) {
        if (mouseEvent->button == Mouse::Button::Left) {
            Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);
            if (selectionWarningButton.getGlobalBounds().contains(mousePos)) {
                selectionWarningVisible = false;
            }
        }
    }
    (void)window;
}

void MoviePanel::updateSelectionWarning(Vector2f mousePos, bool /*mousePressed*/) {
    if (!selectionWarningVisible) return;
    selectionWarningButtonHover = selectionWarningButton.getGlobalBounds().contains(mousePos);
    selectionWarningButton.setFillColor(selectionWarningButtonHover ? Color(17, 98, 144) : Color(20, 118, 172));
}

void MoviePanel::renderSelectionWarning(RenderWindow& window) {
    if (!selectionWarningVisible) return;
    window.draw(selectionWarningOverlay);
    window.draw(selectionWarningBg);
    if (selectionWarningText) {
        window.draw(*selectionWarningText);
    }
    window.draw(selectionWarningButton);
    if (selectionWarningButtonText) {
        window.draw(*selectionWarningButtonText);
    }
}

void MoviePanel::handleEvent(const Event& event, const RenderWindow& window) {
    if (selectionWarningVisible) {
        handleSelectionWarningEvent(event, window);
        return;
    }
    // Change cursor on hover over CRUD buttons
    if (const auto* moveEvent = event.getIf<Event::MouseMoved>()) {
        // We intentionally do not change OS cursor (compatibility across platforms/SFML versions).
        // Hover visual is handled in update() by brightening the button colors.
        (void)moveEvent;
    }
    // Handle popup events
    if (currentPopup != NONE && currentPopup != DELETE_CONFIRM) {
        // Handle input boxes
        for (int i = 0; i < inputBoxes.getSize(); ++i) {
            if (inputBoxes[i]) {
                inputBoxes[i]->handleEvent(event);
            }
        }
        
        // Handle dropdown
        if (statusDropdown) {
            Vector2i mousePixelPos = Mouse::getPosition(window);
            Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
            statusDropdown->handleEvent(event, mousePos);
        }
    }
    
    // Handle mouse clicks
    if (const auto* mouseEvent = event.getIf<Event::MouseButtonPressed>()) {
        if (mouseEvent->button == Mouse::Button::Left) {
            Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);
            
            // Check if click is on table row
            if (currentPopup == NONE) {
                float startY = position.y + TABLE_Y + HEADER_HEIGHT;
                int rowCount = repository->getRecordCount();
                
                for (int i = 0; i < rowCount; i++) {
                    float rowY = startY + i * ROW_HEIGHT;
                    FloatRect rowBounds({position.x + TABLE_X, rowY}, {TABLE_WIDTH, ROW_HEIGHT});
                    
                    if (rowBounds.contains(mousePos)) {
                        selectedRow = i;
                        break;
                    }
                }
            }
        }
    }
}

void MoviePanel::update(Vector2f mousePos, bool mousePressed) {
    if (selectionWarningVisible) {
        updateSelectionWarning(mousePos, mousePressed);
        return;
    }
    // Update hover state for rows
    if (currentPopup == NONE) {
        float startY = position.y + TABLE_Y + HEADER_HEIGHT;
        int rowCount = repository->getRecordCount();
        hoveredRow = -1;
        
        for (int i = 0; i < rowCount; i++) {
            float rowY = startY + i * ROW_HEIGHT;
            FloatRect rowBounds({position.x + TABLE_X, rowY}, {TABLE_WIDTH, ROW_HEIGHT});
            
            if (rowBounds.contains(mousePos)) {
                hoveredRow = i;
                break;
            }
        }
    }
    
    // Adjust mouse position for panel offset
    Vector2f localPos = mousePos - position;
    
    // Update buttons
    if (currentPopup == NONE) {
        // Handle custom add/edit/delete/reload buttons hover and clicks
        FloatRect addBounds = btnAddBg.getGlobalBounds();
        FloatRect editBounds = btnEditBg.getGlobalBounds();
        FloatRect delBounds = btnDeleteBg.getGlobalBounds();
        FloatRect reloadBounds = reloadButtonBg.getGlobalBounds();

        btnAddHover = addBounds.contains(mousePos);
        btnEditHover = editBounds.contains(mousePos);
        btnDeleteHover = delBounds.contains(mousePos);
        btnReloadHover = reloadBounds.contains(mousePos);

        // Hover color tweak (10% brighter)
        auto brighten = [](const Color& c, float pct) {
            auto clamp = [](int v){ return (v<0?0:(v>255?255:v)); };
            int r = clamp((int)(c.r + (255 - c.r) * pct));
            int g = clamp((int)(c.g + (255 - c.g) * pct));
            int b = clamp((int)(c.b + (255 - c.b) * pct));
            return Color(r,g,b);
        };

        // Base colors
        Color addBase(20, 118, 172);
        Color editBase(233, 164, 0);
        Color delBase(211, 47, 47);
        Color reloadBase(20, 118, 172);

        // Apply hover/press visuals
        if (btnAddHover) btnAddBg.setFillColor(brighten(addBase, 0.10f)); else btnAddBg.setFillColor(addBase);
        if (btnEditHover) btnEditBg.setFillColor(brighten(editBase, 0.10f)); else btnEditBg.setFillColor(editBase);
        if (btnDeleteHover) btnDeleteBg.setFillColor(brighten(delBase, 0.10f)); else btnDeleteBg.setFillColor(delBase);
        reloadButtonBg.setFillColor(btnReloadHover ? brighten(reloadBase, 0.10f) : reloadBase);
        reloadSprite.setColor(btnReloadHover ? Color(180, 220, 255) : Color::White);

        if (mousePressed) {
            if (btnAddHover) {
                btnAddPressed = true;
            } else if (btnEditHover) {
                btnEditPressed = true;
            } else if (btnDeleteHover) {
                btnDeletePressed = true;
            } else if (btnReloadHover) {
                btnReloadPressed = true;
            }
        } else {
            // on mouse release trigger action if it was pressed
            if (btnAddPressed && btnAddHover) openAddPopup();
            if (btnEditPressed && btnEditHover) openEditPopup();
            if (btnDeletePressed && btnDeleteHover) openDeleteConfirm();
            if (btnReloadPressed && btnReloadHover) handleReload();
            btnAddPressed = btnEditPressed = btnDeletePressed = btnReloadPressed = false;
        }
    }
    
    // Update popup
    if (currentPopup != NONE) {
        // Update input boxes
        for (int i = 0; i < inputBoxes.getSize(); ++i) {
            if (inputBoxes[i]) {
                inputBoxes[i]->update(mousePos, mousePressed);
            }
        }
        
        // Update popup buttons
        if (btnPopupSave && btnPopupCancel) {
            Color btnSave(20, 118, 172);
            Color btnSaveHover(30, 138, 192);
            Color btnCancel(211, 47, 47);
            Color btnCancelHover(231, 67, 67);
            
            if (currentPopup == DELETE_CONFIRM) {
                btnCancel = Color(160, 160, 160);
                btnCancelHover = Color(180, 180, 180);
            }
            
            btnPopupSave->update(mousePos, mousePressed, btnSave, btnSaveHover);
            btnPopupCancel->update(mousePos, mousePressed, btnCancel, btnCancelHover);
            
            if (mousePressed) {
                if (btnPopupSave->isClicked(mousePos, true)) {
                    if (currentPopup == ADD) {
                        handleAdd();
                    } else if (currentPopup == EDIT) {
                        handleEdit();
                    } else if (currentPopup == DELETE_CONFIRM) {
                        handleDelete();
                    }
                } else if (btnPopupCancel->isClicked(mousePos, true)) {
                    closePopup();
                }
            }
        }
    }
}

void MoviePanel::renderTable(RenderWindow& window) {
    window.draw(tableHeaderBg);

    struct ColumnSpec {
        string label;
        float ratio; // ratio of TABLE_WIDTH
    };

    static const array<ColumnSpec, 5> columns = {{{"ID Phim", 0.10f},
        {"Tên phim", 0.36f}, {"Thời lượng", 0.14f}, {"Ngày chiếu", 0.20f}, {"Trạng thái", 0.20f}}};

    vector<float> columnWidths;
    columnWidths.reserve(columns.size());
    for (const auto& col : columns) {
        columnWidths.push_back(TABLE_WIDTH * col.ratio);
    }

    vector<float> columnLefts(columns.size(), position.x + TABLE_X);
    for (size_t i = 1; i < columns.size(); ++i) {
        columnLefts[i] = columnLefts[i - 1] + columnWidths[i - 1];
    }

    float headerTop = position.y + TABLE_Y;
    float tableLeft = position.x + TABLE_X;
    auto allData = repository->getAllData();
    float totalHeight = HEADER_HEIGHT + static_cast<float>(allData.size()) * ROW_HEIGHT;

    // Draw vertical separators (including left and right borders)
    RectangleShape separator(Vector2f(1.f, totalHeight));
    separator.setFillColor(borderColor);
    separator.setPosition(Vector2f(tableLeft, headerTop));
    window.draw(separator);
    for (size_t i = 0; i < columns.size(); ++i) {
        float x = columnLefts[i] + columnWidths[i];
        separator.setPosition(Vector2f(x, headerTop));
        window.draw(separator);
    }

    // Header text centered
    for (size_t i = 0; i < columns.size(); ++i) {
        Text headerText(font, String::fromUtf8(columns[i].label.begin(), columns[i].label.end()), 18);
        headerText.setFillColor(Color::White);
        headerText.setStyle(Text::Bold);
        FloatRect bounds = headerText.getLocalBounds();
        float colLeft = columnLefts[i];
        float colWidth = columnWidths[i];
        headerText.setPosition(Vector2f(
            colLeft + (colWidth - bounds.size.x) / 2.f - bounds.position.x,
            headerTop + (HEADER_HEIGHT - bounds.size.y) / 2.f - bounds.position.y
        ));
        window.draw(headerText);
    }

    // Draw rows
    float startY = headerTop + HEADER_HEIGHT;
    for (size_t i = 0; i < allData.size(); i++) {
        float rowY = startY + static_cast<float>(i) * ROW_HEIGHT;

        RectangleShape rowBg(Vector2f(TABLE_WIDTH, ROW_HEIGHT));
        rowBg.setPosition(Vector2f(tableLeft, rowY));

        if (static_cast<int>(i) == selectedRow) {
            rowBg.setFillColor(selectedColor);
        } else if (static_cast<int>(i) == hoveredRow) {
            rowBg.setFillColor(hoverColor);
        } else {
            rowBg.setFillColor(rowColor);
        }
        window.draw(rowBg);

        RectangleShape rowBorder(Vector2f(TABLE_WIDTH, 1.f));
        rowBorder.setPosition(Vector2f(tableLeft, rowY + ROW_HEIGHT));
        rowBorder.setFillColor(borderColor);
        window.draw(rowBorder);

        const auto& row = allData[i];
        if (row.size() < 13) continue;

        array<string, 5> cellValues = {
            row[0],
            row[1],
            row[6] + " phút",
            row[7],
            row[12]
        };

        for (size_t col = 0; col < columns.size(); ++col) {
            Text cellText(font, String::fromUtf8(cellValues[col].begin(), cellValues[col].end()), 15);
            cellText.setFillColor(textColor);
            FloatRect bounds = cellText.getLocalBounds();
            cellText.setPosition(Vector2f(
                columnLefts[col] + 12.f - bounds.position.x,
                rowY + (ROW_HEIGHT - bounds.size.y) / 2.f - bounds.position.y
            ));
            window.draw(cellText);
        }
    }
}

void MoviePanel::renderPopup(RenderWindow& window) {
    if (currentPopup == NONE) return;
    
    // Draw overlay
    window.draw(popupOverlay);
    
    // Draw popup background
    window.draw(popupBackground);
    
    // Draw title
    if (popupTitle) {
        window.draw(*popupTitle);
    }
    
    // Draw input boxes
    for (int i = 0; i < inputBoxes.getSize(); ++i) {
        if (inputBoxes[i]) {
            inputBoxes[i]->render(window);
        }
    }
    
    // Draw dropdown
    if (statusDropdown && (currentPopup == ADD || currentPopup == EDIT)) {
        statusDropdown->draw(window);
    }
    
    // Draw buttons
    if (btnPopupSave && btnPopupCancel) {
        btnPopupSave->draw(window);
        btnPopupCancel->draw(window);
    }
}

void MoviePanel::renderNotification(RenderWindow& window) {
    if (!notificationVisible) return;
    if (notificationClock.getElapsedTime().asSeconds() > 3.0f) {
        notificationVisible = false;
        return;
    }
    
    notificationBg.setPosition(Vector2f(1300, 50));
    window.draw(notificationBg);
    
    notificationTextObj->setString(String::fromUtf8(notificationText.begin(), notificationText.end()));
    FloatRect textBounds = notificationTextObj->getLocalBounds();
    notificationTextObj->setPosition(Vector2f(
        1500 - textBounds.size.x / 2,
        80 - textBounds.size.y / 2
    ));
    window.draw(*notificationTextObj);
}

void MoviePanel::render(RenderWindow& window) {
    // Draw background
    window.draw(background);
    
    // Draw title
    if (titleText) {
        window.draw(*titleText);
    }
    
    // Draw table
    renderTable(window);
    
    // Draw CRUD buttons (custom with rounded corners 6px)
    // Add button
    RoundedRectRenderer::draw(window, btnAddBg.getPosition(), btnAddBg.getSize(), 6.f, btnAddBg.getFillColor());
    if (btnAddText) {
        FloatRect tb = btnAddText->getLocalBounds();
        Vector2f pos = btnAddBg.getPosition();
        btnAddText->setPosition(Vector2f(
            pos.x + (btnAddBg.getSize().x - tb.size.x) / 2.f,
            pos.y + (btnAddBg.getSize().y - tb.size.y) / 2.f
        ));
        window.draw(*btnAddText);
    }

    // Edit button
    RoundedRectRenderer::draw(window, btnEditBg.getPosition(), btnEditBg.getSize(), 6.f, btnEditBg.getFillColor());
    if (btnEditText) {
        FloatRect tb = btnEditText->getLocalBounds();
        Vector2f pos = btnEditBg.getPosition();
        btnEditText->setPosition(Vector2f(
            pos.x + (btnEditBg.getSize().x - tb.size.x) / 2.f,
            pos.y + (btnEditBg.getSize().y - tb.size.y) / 2.f
        ));
        window.draw(*btnEditText);
    }

    // Delete button
    RoundedRectRenderer::draw(window, btnDeleteBg.getPosition(), btnDeleteBg.getSize(), 6.f, btnDeleteBg.getFillColor());
    if (btnDeleteText) {
        FloatRect tb = btnDeleteText->getLocalBounds();
        Vector2f pos = btnDeleteBg.getPosition();
        btnDeleteText->setPosition(Vector2f(
            pos.x + (btnDeleteBg.getSize().x - tb.size.x) / 2.f,
            pos.y + (btnDeleteBg.getSize().y - tb.size.y) / 2.f
        ));
        window.draw(*btnDeleteText);
    }

    // Reload button (icon centered inside rounded background)
    RoundedRectRenderer::draw(window, reloadButtonBg.getPosition(), reloadButtonBg.getSize(), 6.f, reloadButtonBg.getFillColor());
        FloatRect localBounds = reloadSprite.getLocalBounds();
        Vector2f spriteScale = reloadSprite.getScale();
        float scaledW = localBounds.size.x * spriteScale.x;
        float scaledH = localBounds.size.y * spriteScale.y;
        reloadSprite.setPosition({
            reloadButtonBg.getPosition().x + (reloadButtonBg.getSize().x - scaledW) / 2.f,
            reloadButtonBg.getPosition().y + (reloadButtonBg.getSize().y - scaledH) / 2.f
        });
        window.draw(reloadSprite);
    
    // Draw popup if active
    renderPopup(window);
    
    // Draw notification
    renderNotification(window);

    renderSelectionWarning(window);
}
