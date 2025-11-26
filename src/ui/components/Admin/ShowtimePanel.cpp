#include "UI/components/Admin/ShowtimePanel.h"

#include "UI/components/Admin/RoundedRectRenderer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

using namespace std;
using namespace std::chrono;

namespace {
const sf::Color kBgColor(244, 246, 250);
const sf::Color kHeaderColor(20, 118, 172);
const sf::Color kRowColor(255, 255, 255);
const sf::Color kHoverColor(229, 241, 251);
const sf::Color kBorderColor(201, 206, 214);
const sf::Color kTextColor(34, 34, 34);
const sf::Color kSummaryCardColor(23, 124, 186);
const sf::Color kSummaryMutedText(210, 232, 247);
const sf::Color kSummaryTitleColor(255, 255, 255);
}

ShowtimePanel::ShowtimePanel(sf::Font& f, float w, float h)
    : font(f), width(w), height(h), reloadTexture("../assets/elements/reload.png") {
    position = sf::Vector2f(0.f, 0.f);
    setupUI();
    loadData();
}

void ShowtimePanel::setupUI() {
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(kBgColor);

    titleText = make_unique<sf::Text>(font);
    titleText->setCharacterSize(26);
    titleText->setFillColor(sf::Color(27, 38, 59));
    titleText->setStyle(sf::Text::Bold);
    const string titleLabel = "Quản lý suất chiếu";
    titleText->setString(sf::String::fromUtf8(titleLabel.begin(), titleLabel.end()));

    summaryCard.setSize(sf::Vector2f(380.f, 60.f));
    summaryCard.setFillColor(kSummaryCardColor);

    summaryPrimaryText = make_unique<sf::Text>(font);
    summaryPrimaryText->setCharacterSize(30);
    summaryPrimaryText->setFillColor(sf::Color::White);
    summaryPrimaryText->setStyle(sf::Text::Bold);

    summaryTitleText = make_unique<sf::Text>(font);
    summaryTitleText->setCharacterSize(16);
    summaryTitleText->setFillColor(kSummaryTitleColor);
    summaryTitleText->setStyle(sf::Text::Bold);

    summaryMetaDateText = make_unique<sf::Text>(font);
    summaryMetaDateText->setCharacterSize(15);
    summaryMetaDateText->setFillColor(kSummaryMutedText);

    summaryMetaRoomText = make_unique<sf::Text>(font);
    summaryMetaRoomText->setCharacterSize(15);
    summaryMetaRoomText->setFillColor(kSummaryMutedText);

    tableHeaderBg.setSize(sf::Vector2f(TABLE_WIDTH, HEADER_HEIGHT));
    tableHeaderBg.setFillColor(kHeaderColor);

    tableBorder.setSize(sf::Vector2f(TABLE_WIDTH, TABLE_HEIGHT));
    tableBorder.setFillColor(sf::Color::Transparent);
    tableBorder.setOutlineThickness(1.f);
    tableBorder.setOutlineColor(kBorderColor);

    reloadButtonBg.setSize(sf::Vector2f(48.f, 48.f));
    reloadButtonBg.setFillColor(kHeaderColor);
    reloadTexture.setSmooth(true);
    reloadSprite = make_unique<sf::Sprite>(reloadTexture);
    reloadSprite->setScale({0.1f, 0.1f});

    dateDropdown = make_unique<DropdownBox>(font, "Ngày chiếu", position.x + TABLE_X, position.y + 110.f, 260.f, 48.f);
    roomDropdown = make_unique<DropdownBox>(font, "Phòng chiếu", position.x + TABLE_X + 300.f, position.y + 110.f, 260.f, 48.f);
    dateDropdown->setMaxVisibleOptions(4);
    roomDropdown->setMaxVisibleOptions(4);
}

void ShowtimePanel::setPosition(sf::Vector2f pos) {
    position = pos;
    background.setPosition(pos);

    if (titleText) {
        titleText->setPosition(sf::Vector2f(pos.x + 40.f, pos.y + 28.f));
    }

    const float controlsTop = pos.y + 110.f;
    const float controlHeight = 48.f;
    const float dateWidth = 260.f;
    const float roomWidth = 260.f;
    const float spacing = 24.f;
    const float controlsLeft = pos.x + TABLE_X;
    const float controlsRight = pos.x + TABLE_X + TABLE_WIDTH;

    if (dateDropdown) {
        dateDropdown->setPosition(sf::Vector2f(controlsLeft, controlsTop));
    }
    if (roomDropdown) {
        roomDropdown->setPosition(sf::Vector2f(controlsLeft + dateWidth + spacing, controlsTop));
    }

    reloadButtonBg.setSize(sf::Vector2f(controlHeight, controlHeight));
    reloadButtonBg.setPosition(sf::Vector2f(controlsRight - reloadButtonBg.getSize().x, controlsTop));

    float summaryX = controlsLeft + dateWidth + spacing + roomWidth + spacing;
    float availableWidth = reloadButtonBg.getPosition().x - spacing - summaryX;
    if (availableWidth < 220.f) {
        availableWidth = 220.f;
        summaryX = max(controlsLeft, reloadButtonBg.getPosition().x - spacing - availableWidth);
    }
    summaryCard.setSize(sf::Vector2f(availableWidth, controlHeight));
    summaryCard.setPosition(sf::Vector2f(summaryX, controlsTop));

    tableHeaderBg.setPosition(sf::Vector2f(pos.x + TABLE_X, pos.y + TABLE_Y));
    tableBorder.setPosition(sf::Vector2f(pos.x + TABLE_X, pos.y + TABLE_Y));

    updateSummaryText();
}

void ShowtimePanel::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (dateDropdown) {
        dateDropdown->handleEvent(event, mousePos);
    }
    if (roomDropdown) {
        roomDropdown->handleEvent(event, mousePos);
    }

    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        handleScroll(wheel->delta, window);
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            if (reloadButtonBg.getGlobalBounds().contains(mousePos)) {
                reloadPressed = true;
            }
        }
    }
}

void ShowtimePanel::update(sf::Vector2f mousePos, bool mousePressed) {
    if (reloadPressed && !mousePressed) {
        if (reloadButtonBg.getGlobalBounds().contains(mousePos)) {
            loadData();
        }
        reloadPressed = false;
    }

    reloadHovered = reloadButtonBg.getGlobalBounds().contains(mousePos);
    reloadButtonBg.setFillColor(reloadHovered ? sf::Color(30, 138, 192) : kHeaderColor);

    if (dateDropdown) {
        dateDropdown->update();
        string selected = dateDropdown->getSelectedValue();
        if (selected != activeDateFilter) {
            activeDateFilter = selected;
            applyFilters();
        }
    }
    if (roomDropdown) {
        roomDropdown->update();
        string selectedRoom = roomDropdown->getSelectedValue();
        if (selectedRoom != activeRoomFilterLabel) {
            activeRoomFilterLabel = selectedRoom;
            applyFilters();
        }
    }

    updateHoveredRow(mousePos);
}

void ShowtimePanel::render(sf::RenderWindow& window) {
    window.draw(background);
    if (titleText) {
        window.draw(*titleText);
    }

    RoundedRectRenderer::draw(window, summaryCard.getPosition(), summaryCard.getSize(), 18.f, summaryCard.getFillColor());
    if (summaryPrimaryText) {
        window.draw(*summaryPrimaryText);
    }
    if (summaryTitleText) {
        window.draw(*summaryTitleText);
    }
    if (summaryMetaDateText) {
        window.draw(*summaryMetaDateText);
    }
    if (summaryMetaRoomText) {
        window.draw(*summaryMetaRoomText);
    }

    window.draw(tableBorder);
    if (filteredShowtimes.empty()) {
        renderEmptyState(window);
    } else {
        renderTable(window);
    }

    RoundedRectRenderer::draw(window, reloadButtonBg.getPosition(), reloadButtonBg.getSize(), 6.f, reloadButtonBg.getFillColor());
    if (reloadSprite) {
        sf::FloatRect spriteBounds = reloadSprite->getLocalBounds();
        sf::Vector2f scale = reloadSprite->getScale();
        float spriteWidth = spriteBounds.size.x * scale.x;
        float spriteHeight = spriteBounds.size.y * scale.y;
        reloadSprite->setPosition(sf::Vector2f(
            reloadButtonBg.getPosition().x + (reloadButtonBg.getSize().x - spriteWidth) / 2.f,
            reloadButtonBg.getPosition().y + (reloadButtonBg.getSize().y - spriteHeight) / 2.f));
        reloadSprite->setColor(sf::Color::White);
        window.draw(*reloadSprite);
    }

    if (dateDropdown) {
        dateDropdown->draw(window);
    }
    if (roomDropdown) {
        roomDropdown->draw(window);
    }
}

void ShowtimePanel::loadData() {
    movieCatalog.clear();
    roomNames.clear();
    allShowtimes.clear();
    filteredShowtimes.clear();
    archivedIds.clear();
    loadedShowtimeIds.clear();

    loadMovies();
    loadRooms();
    loadHistory();
    loadShowtimes();
    refreshDropdowns();
    applyFilters();
}

void ShowtimePanel::loadMovies() {
    ifstream file(moviesFile);
    if (!file.is_open()) {
        cerr << "[ShowtimePanel] Cannot open movies file: " << moviesFile << "\n";
        return;
    }

    string line;
    getline(file, line);
    while (getline(file, line)) {
        auto parts = splitLine(line, '|');
        if (parts.size() < 7) continue;
        MovieInfo info;
        info.title = trim(parts[1]);
        try {
            info.durationMinutes = stoi(trim(parts[6]));
        } catch (...) {
            info.durationMinutes = 0;
        }
        movieCatalog[trim(parts[0])] = info;
    }
}

void ShowtimePanel::loadRooms() {
    ifstream file(roomsFile);
    if (!file.is_open()) {
        cerr << "[ShowtimePanel] Cannot open rooms file: " << roomsFile << "\n";
        return;
    }

    string line;
    getline(file, line);
    while (getline(file, line)) {
        auto parts = splitLine(line, '|');
        if (parts.size() < 2) continue;
        string id = trim(parts[0]);
        string name = trim(parts[1]);
        if (!id.empty()) {
            roomNames[id] = name;
        }
    }
}

void ShowtimePanel::loadHistory() {
    ifstream file(historyFile);
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("showtime_id") != string::npos) {
            continue;
        }
        auto parts = splitLine(line, '|');
        if (auto item = buildShowtime(parts)) {
            archivedIds.insert(item->showtimeId);
            if (loadedShowtimeIds.insert(item->showtimeId).second) {
                allShowtimes.push_back(*item);
            }
        }
    }
}

void ShowtimePanel::loadShowtimes() {
    ifstream file(showtimeFile);
    if (!file.is_open()) {
        cerr << "[ShowtimePanel] Cannot open showtimes file: " << showtimeFile << "\n";
        return;
    }

    string line;
    bool firstLine = true;
    vector<TimelineItem> pendingHistory;
    while (getline(file, line)) {
        if (firstLine && line.find("showtime_id") != string::npos) {
            firstLine = false;
            continue;
        }
        firstLine = false;
        auto parts = splitLine(line, '|');
        if (auto item = buildShowtime(parts)) {
            if (loadedShowtimeIds.insert(item->showtimeId).second) {
                allShowtimes.push_back(*item);
            }
            if (isDateOnOrBeforeToday(item->date) && archivedIds.insert(item->showtimeId).second) {
                pendingHistory.push_back(*item);
            }
        }
    }

    if (!pendingHistory.empty()) {
        appendHistory(pendingHistory);
    }

    sort(allShowtimes.begin(), allShowtimes.end(), [](const TimelineItem& a, const TimelineItem& b) {
        if (a.date == b.date) {
            return a.start < b.start;
        }
        return a.date < b.date;
    });
}

void ShowtimePanel::refreshDropdowns() {
    set<string> uniqueDates;
    for (const auto& item : allShowtimes) {
        uniqueDates.insert(item.date);
    }
    dateOptions.assign(uniqueDates.begin(), uniqueDates.end());

    if (dateDropdown) {
        if (!dateOptions.empty()) {
            dateDropdown->setOptions(dateOptions);
            dateDropdown->setSelectedIndex(0);
            dateDropdown->setEnabled(true);
            activeDateFilter = dateDropdown->getSelectedValue();
        } else {
            dateDropdown->setOptions({});
            dateDropdown->setEnabled(false);
            activeDateFilter.clear();
        }
    }

    roomOptionLabels.clear();
    roomLabelToId.clear();
    roomOptionLabels.push_back("Tất cả phòng");
    roomLabelToId[roomOptionLabels.back()] = "";

    vector<pair<string, string>> roomList(roomNames.begin(), roomNames.end());
    sort(roomList.begin(), roomList.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    });

    unordered_set<string> usedLabels;
    for (const auto& [id, name] : roomList) {
        string label = name;
        if (usedLabels.count(label)) {
            label += " (" + id + ")";
        }
        usedLabels.insert(label);
        roomOptionLabels.push_back(label);
        roomLabelToId[label] = id;
    }

    if (roomDropdown) {
        roomDropdown->setOptions(roomOptionLabels);
        roomDropdown->setSelectedIndex(0);
        activeRoomFilterLabel = roomDropdown->getSelectedValue();
    }
}

void ShowtimePanel::applyFilters() {
    filteredShowtimes.clear();

    if (activeDateFilter.empty()) {
        updateSummaryText();
        return;
    }

    string roomIdFilter;
    if (!activeRoomFilterLabel.empty()) {
        auto it = roomLabelToId.find(activeRoomFilterLabel);
        if (it != roomLabelToId.end()) {
            roomIdFilter = it->second;
        }
    }

    for (const auto& item : allShowtimes) {
        if (item.date != activeDateFilter) continue;
        if (!roomIdFilter.empty() && item.roomId != roomIdFilter) continue;
        filteredShowtimes.push_back(item);
    }

    sort(filteredShowtimes.begin(), filteredShowtimes.end(), [](const TimelineItem& a, const TimelineItem& b) {
        return a.start < b.start;
    });

    topRowIndex = 0;
    clampScroll();
    updateSummaryText();
}

void ShowtimePanel::updateSummaryText() {
    if (!summaryPrimaryText || !summaryTitleText || !summaryMetaDateText || !summaryMetaRoomText) return;

    const int count = static_cast<int>(filteredShowtimes.size());
    const string dateLabel = activeDateFilter.empty() ? "Chưa chọn ngày" : activeDateFilter;
    const string roomLabel = activeRoomFilterLabel.empty() ? "Tất cả phòng" : activeRoomFilterLabel;

    const string countLabel = to_string(count);
    const string leftCaption = "suất chiếu";
    const string dateLine = "Ngày: " + dateLabel;
    const string roomLine = "Phòng: " + roomLabel;

    summaryPrimaryText->setString(sf::String::fromUtf8(countLabel.begin(), countLabel.end()));
    summaryTitleText->setString(sf::String::fromUtf8(leftCaption.begin(), leftCaption.end()));
    summaryMetaDateText->setString(sf::String::fromUtf8(dateLine.begin(), dateLine.end()));
    summaryMetaRoomText->setString(sf::String::fromUtf8(roomLine.begin(), roomLine.end()));

    const float padding = 20.f;
    const float topPadding = 10.f;
    const float columnGapDefault = 32.f;
    const float minGap = 12.f;
    const float maxGap = 48.f;
    const float lineSpacing = 6.f;
    sf::Vector2f cardPos = summaryCard.getPosition();
    sf::Vector2f cardSize = summaryCard.getSize();

    sf::FloatRect countBounds = summaryPrimaryText->getLocalBounds();
    sf::FloatRect titleBounds = summaryTitleText->getLocalBounds();
    sf::FloatRect dateBounds = summaryMetaDateText->getLocalBounds();
    sf::FloatRect roomBounds = summaryMetaRoomText->getLocalBounds();

    float leftColumnWidth = max(countBounds.size.x, titleBounds.size.x);
    float rightColumnWidth = max(dateBounds.size.x, roomBounds.size.x);
    float availableWidth = max(0.f, cardSize.x - 2.f * padding);
    float desiredGap = columnGapDefault;
    if (leftColumnWidth + rightColumnWidth + desiredGap > availableWidth) {
        desiredGap = max(minGap, availableWidth - (leftColumnWidth + rightColumnWidth));
    }
    desiredGap = std::clamp(desiredGap, minGap, maxGap);

    float leftX = cardPos.x + padding;
    float rightX = min(cardPos.x + cardSize.x - padding - rightColumnWidth,
                       leftX + leftColumnWidth + desiredGap);
    rightX = max(rightX, leftX + leftColumnWidth + minGap);

    float topY = cardPos.y + topPadding;
    summaryPrimaryText->setPosition(sf::Vector2f(
        leftX - countBounds.position.x,
        topY - countBounds.position.y));

    summaryTitleText->setPosition(sf::Vector2f(
        leftX - titleBounds.position.x,
        summaryPrimaryText->getPosition().y + countBounds.size.y + 2.f - titleBounds.position.y));

    summaryMetaDateText->setPosition(sf::Vector2f(
        rightX - dateBounds.position.x,
        topY - dateBounds.position.y));

    summaryMetaRoomText->setPosition(sf::Vector2f(
        rightX - roomBounds.position.x,
        summaryMetaDateText->getPosition().y + dateBounds.size.y + lineSpacing - roomBounds.position.y));
}

void ShowtimePanel::updateHoveredRow(sf::Vector2f mousePos) {
    hoveredRow = -1;
    if (filteredShowtimes.empty()) return;

    const float bodyTop = position.y + TABLE_Y + HEADER_HEIGHT;
    const float bodyHeight = TABLE_HEIGHT - HEADER_HEIGHT;
    sf::FloatRect tableBounds(sf::Vector2f(position.x + TABLE_X, bodyTop),
                              sf::Vector2f(TABLE_WIDTH, bodyHeight));
    if (!tableBounds.contains(mousePos)) {
        return;
    }

    int visibleRows = min<int>(MAX_VISIBLE_ROWS, static_cast<int>(filteredShowtimes.size()) - topRowIndex);
    if (visibleRows <= 0) return;

    float relativeY = mousePos.y - bodyTop;
    int localIndex = static_cast<int>(relativeY / ROW_HEIGHT);
    if (localIndex >= 0 && localIndex < visibleRows) {
        hoveredRow = topRowIndex + localIndex;
    }
}

void ShowtimePanel::clampScroll() {
    int total = static_cast<int>(filteredShowtimes.size());
    int maxTop = max(0, total - MAX_VISIBLE_ROWS);
    if (topRowIndex > maxTop) topRowIndex = maxTop;
    if (topRowIndex < 0) topRowIndex = 0;
}

void ShowtimePanel::handleScroll(float delta, const sf::RenderWindow& window) {
    if (filteredShowtimes.size() <= static_cast<size_t>(MAX_VISIBLE_ROWS)) {
        return;
    }
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::FloatRect tableBounds(sf::Vector2f(position.x + TABLE_X, position.y + TABLE_Y),
                              sf::Vector2f(TABLE_WIDTH, TABLE_HEIGHT));
    if (!tableBounds.contains(mousePos)) {
        return;
    }

    if (delta > 0) {
        topRowIndex = max(0, topRowIndex - 1);
    } else {
        topRowIndex = min(topRowIndex + 1, max(0, static_cast<int>(filteredShowtimes.size()) - MAX_VISIBLE_ROWS));
    }
    clampScroll();
}

void ShowtimePanel::renderTable(sf::RenderWindow& window) {
    struct Column {
        string label;
        float width;
    };

    array<Column, 4> columns = {
        Column{"Giờ chiếu", 160.f},
        Column{"Phòng", 200.f},
        Column{"Phim", 580.f},
        Column{"Giá vé", 160.f}
    };

    vector<float> columnLeft(columns.size(), position.x + TABLE_X);
    for (size_t i = 1; i < columns.size(); ++i) {
        columnLeft[i] = columnLeft[i - 1] + columns[i - 1].width;
    }

    window.draw(tableHeaderBg);

    for (size_t i = 0; i < columns.size(); ++i) {
        sf::Text text(font, sf::String::fromUtf8(columns[i].label.begin(), columns[i].label.end()), 18);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(sf::Vector2f(
            columnLeft[i] + (columns[i].width - bounds.size.x) / 2.f - bounds.position.x,
            tableHeaderBg.getPosition().y + (HEADER_HEIGHT - bounds.size.y) / 2.f - bounds.position.y));
        window.draw(text);
    }

    int visibleRows = min<int>(MAX_VISIBLE_ROWS, static_cast<int>(filteredShowtimes.size()) - topRowIndex);
    float startY = tableHeaderBg.getPosition().y + HEADER_HEIGHT;

    for (int row = 0; row < visibleRows; ++row) {
        int dataIndex = topRowIndex + row;
        const auto& item = filteredShowtimes[dataIndex];
        float rowY = startY + row * ROW_HEIGHT;

        sf::RectangleShape rowBg(sf::Vector2f(TABLE_WIDTH, ROW_HEIGHT));
        rowBg.setPosition(sf::Vector2f(position.x + TABLE_X, rowY));
        if (dataIndex == hoveredRow) {
            rowBg.setFillColor(kHoverColor);
        } else {
            rowBg.setFillColor(kRowColor);
        }
        window.draw(rowBg);

        sf::RectangleShape border(sf::Vector2f(TABLE_WIDTH, 1.f));
        border.setPosition(sf::Vector2f(position.x + TABLE_X, rowY + ROW_HEIGHT));
        border.setFillColor(kBorderColor);
        window.draw(border);

        array<string, 4> values = {
            formatTimeRange(item),
            roomNames.count(item.roomId) ? roomNames[item.roomId] : item.roomId,
            movieCatalog.count(item.movieId) ? movieCatalog[item.movieId].title : item.movieId,
            formatPrice(item.price)
        };

        for (size_t col = 0; col < columns.size(); ++col) {
            sf::Text text(font, sf::String::fromUtf8(values[col].begin(), values[col].end()), 16);
            text.setFillColor(kTextColor);
            sf::FloatRect bounds = text.getLocalBounds();
            float centeredX = columnLeft[col] + (columns[col].width - bounds.size.x) / 2.f - bounds.position.x;
            float centeredY = rowY + (ROW_HEIGHT - bounds.size.y) / 2.f - bounds.position.y;
            text.setPosition(sf::Vector2f(centeredX, centeredY));
            window.draw(text);
        }
    }
}

void ShowtimePanel::renderEmptyState(sf::RenderWindow& window) {
    const float areaLeft = position.x + TABLE_X;
    const float areaTop = position.y + TABLE_Y;
    sf::RectangleShape placeholder(sf::Vector2f(TABLE_WIDTH - 4.f, TABLE_HEIGHT - 4.f));
    placeholder.setPosition(sf::Vector2f(areaLeft + 2.f, areaTop + 2.f));
    placeholder.setFillColor(sf::Color(255, 255, 255));
    window.draw(placeholder);

    string message;
    if (activeDateFilter.empty()) {
        message = "Chọn một ngày để xem suất chiếu";
    } else {
        message = "Chưa có suất chiếu cho " + activeDateFilter;
        auto roomIt = roomLabelToId.find(activeRoomFilterLabel);
        if (roomIt != roomLabelToId.end() && !roomIt->second.empty()) {
            message += " tại phòng đã chọn";
        }
    }
    sf::Text text(font, sf::String::fromUtf8(message.begin(), message.end()), 20);
    text.setFillColor(kTextColor);
    sf::FloatRect bounds = text.getLocalBounds();
    float centerX = areaLeft + TABLE_WIDTH / 2.f;
    float centerY = areaTop + TABLE_HEIGHT / 2.f;
    text.setPosition(sf::Vector2f(centerX - bounds.size.x / 2.f - bounds.position.x,
                                  centerY - bounds.size.y / 2.f - bounds.position.y));
    window.draw(text);
}

optional<ShowtimePanel::TimelineItem> ShowtimePanel::buildShowtime(const vector<string>& parts) {
    if (parts.size() < 6) return nullopt;

    TimelineItem item;
    item.showtimeId = trim(parts[0]);
    item.movieId = trim(parts[1]);
    item.roomId = trim(parts[2]);
    item.date = trim(parts[3]);
    item.timeStr = trim(parts[4]);
    try {
        item.price = stoi(trim(parts[5]));
    } catch (...) {
        item.price = 0;
    }

    auto infoIt = movieCatalog.find(item.movieId);
    if (infoIt != movieCatalog.end()) {
        item.durationMinutes = max(1, infoIt->second.durationMinutes);
    } else {
        item.durationMinutes = 120;
    }

    auto startOpt = parseDateTime(item.date, item.timeStr);
    if (!startOpt.has_value()) {
        return nullopt;
    }
    item.start = startOpt.value();
    item.end = item.start + minutes(item.durationMinutes);
    return item;
}

vector<string> ShowtimePanel::splitLine(const string& line, char delimiter) {
    vector<string> parts;
    string part;
    stringstream ss(line);
    while (getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

string ShowtimePanel::trim(const string& text) {
    size_t start = 0;
    while (start < text.size() && isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }
    size_t end = text.size();
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(start, end - start);
}

optional<ShowtimePanel::TimePoint> ShowtimePanel::parseDateTime(const string& dateStr, const string& timeStr) {
    if (dateStr.size() < 10 || timeStr.size() < 5) return nullopt;
    int year = 0, month = 0, day = 0, hour = 0, minute = 0;
    try {
        year = stoi(dateStr.substr(0, 4));
        month = stoi(dateStr.substr(5, 2));
        day = stoi(dateStr.substr(8, 2));
        hour = stoi(timeStr.substr(0, 2));
        minute = stoi(timeStr.substr(3, 2));
    } catch (...) {
        return nullopt;
    }

    tm tmTime{};
    tmTime.tm_year = year - 1900;
    tmTime.tm_mon = month - 1;
    tmTime.tm_mday = day;
    tmTime.tm_hour = hour;
    tmTime.tm_min = minute;
    tmTime.tm_sec = 0;
    tmTime.tm_isdst = -1;

    time_t localTime = mktime(&tmTime);
    if (localTime == -1) return nullopt;
    return system_clock::from_time_t(localTime);
}

string ShowtimePanel::formatTime(const TimePoint& tp) {
    time_t raw = system_clock::to_time_t(tp);
    tm local{};
#ifdef _WIN32
    localtime_s(&local, &raw);
#else
    localtime_r(&raw, &local);
#endif
    ostringstream oss;
    oss << setfill('0') << setw(2) << local.tm_hour << ':' << setw(2) << local.tm_min;
    return oss.str();
}

string ShowtimePanel::formatTimeRange(const TimelineItem& item) {
    return formatTime(item.start) + " - " + formatTime(item.end);
}

string ShowtimePanel::formatPrice(int price) {
    string digits = price <= 0 ? "0" : to_string(price);
    string grouped;
    int counter = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        grouped.push_back(digits[static_cast<size_t>(i)]);
        ++counter;
        if (counter == 3 && i != 0) {
            grouped.push_back('.');
            counter = 0;
        }
    }
    reverse(grouped.begin(), grouped.end());
    grouped.append(" đ");
    return grouped;
}

string ShowtimePanel::getTodayDateString() {
    auto now = system_clock::now();
    time_t raw = system_clock::to_time_t(now);
    tm local{};
#ifdef _WIN32
    localtime_s(&local, &raw);
#else
    localtime_r(&raw, &local);
#endif
    ostringstream oss;
    oss << setw(4) << setfill('0') << (local.tm_year + 1900) << '-'
        << setw(2) << setfill('0') << (local.tm_mon + 1) << '-'
        << setw(2) << setfill('0') << local.tm_mday;
    return oss.str();
}

bool ShowtimePanel::isDateOnOrBeforeToday(const string& dateStr) {
    return dateStr <= getTodayDateString();
}

string ShowtimePanel::serializeShowtime(const TimelineItem& item) {
    ostringstream oss;
    oss << item.showtimeId << '|' << item.movieId << '|' << item.roomId << '|' << item.date << '|' << item.timeStr << '|' << item.price;
    return oss.str();
}

void ShowtimePanel::appendHistory(const vector<TimelineItem>& entries) {
    if (entries.empty()) {
        return;
    }

    bool hasContent = false;
    {
        ifstream check(historyFile);
        hasContent = check.good() && check.peek() != EOF;
    }

    ofstream file(historyFile, ios::app);
    if (!file.is_open()) {
        cerr << "[ShowtimePanel] Cannot append history file: " << historyFile << "\n";
        return;
    }

    if (!hasContent) {
        file << "showtime_id|movie_id|room_id|date|time|price\n";
    }

    for (const auto& entry : entries) {
        file << serializeShowtime(entry) << '\n';
    }
}
