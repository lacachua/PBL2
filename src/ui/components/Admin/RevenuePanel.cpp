#include "UI/components/Admin/RevenuePanel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <vector>

using sf::Color;
using sf::Event;
using sf::FloatRect;
using sf::RectangleShape;
using sf::RenderWindow;
using sf::Vector2f;

RevenuePanel::RevenuePanel(Font& fontRef, float w, float h, StatsMode modeValue)
                : font(fontRef), width(w), height(h), position(0.f, 0.f), mode(modeValue),
                    titleText(font, String(), 28),
                    fromLabelText(font, String(), 14),
                    toLabelText(font, String(), 14) {
    ticketsFilePath = resolveDataPath("data/tickets.txt");
    showtimesFilePath = resolveDataPath("data/showtimes.txt");
    combosFilePath = resolveDataPath("data/combo.txt");
    initializeUI();
    loadData();
}

string RevenuePanel::resolveDataPath(const string& relative) const {
    namespace fs = filesystem;
    const array<string, 4> prefixes = {"../", "./", "../../", ""};
    for (const auto& prefix : prefixes) {
        fs::path candidate = fs::path(prefix) / relative;
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }
    return relative;
}

void RevenuePanel::initializeUI() {
    background.setSize(Vector2f(width, height));
    background.setFillColor(Color(240, 245, 250));
    background.setPosition(position);

    string heading = (mode == StatsMode::Revenue)
        ? "Dashboard / Doanh thu theo phim"
        : "Dashboard / Vé đã bán theo phim";
    titleText.setString(makeUtf8(heading));
    titleText.setFillColor(Color(95, 106, 128));
    titleText.setCharacterSize(16);
    titleText.setPosition(Vector2f(position.x + 24.f, position.y + 20.f));

    float margin = 24.f;
    float controlY = position.y + 72.f;
    float dropH = 36.f;
    float dropW = 72.f;
    float gap = 8.f;
    float groupGap = 36.f;
    float startX = position.x + margin;

    fromLabelText = Text(font, makeUtf8("Từ ngày"), 14);
    fromLabelText.setFillColor(Color(90, 104, 123));
    fromLabelText.setPosition(Vector2f(startX, controlY - 26.f));

    float toStartX = startX + (dropW + gap) * 2 + 96.f + groupGap;
    toLabelText = Text(font, makeUtf8("Đến ngày"), 14);
    toLabelText.setFillColor(Color(90, 104, 123));
    toLabelText.setPosition(Vector2f(toStartX, controlY - 26.f));

    fromDayDropdown = make_unique<DropdownBox>(font, "", startX, controlY, dropW, dropH);
    fromMonthDropdown = make_unique<DropdownBox>(font, "", startX + dropW + gap, controlY, dropW, dropH);
    fromYearDropdown = make_unique<DropdownBox>(font, "", startX + (dropW + gap) * 2, controlY, 96.f, dropH);

    toDayDropdown = make_unique<DropdownBox>(font, "", toStartX, controlY, dropW, dropH);
    toMonthDropdown = make_unique<DropdownBox>(font, "", toStartX + dropW + gap, controlY, dropW, dropH);
    toYearDropdown = make_unique<DropdownBox>(font, "", toStartX + (dropW + gap) * 2, controlY, 96.f, dropH);

    if (fromDayDropdown) fromDayDropdown->setMaxVisibleOptions(12);
    if (toDayDropdown) toDayDropdown->setMaxVisibleOptions(12);

    float buttonWidth = 170.f;
    float buttonHeight = dropH;
    float buttonStartX = position.x + width - margin - buttonWidth;

    applyButton = make_unique<Button>(font, makeUtf8("Tải dữ liệu"), buttonWidth, buttonHeight, 14);
    applyButton->setFillColor(accentColor);
    applyButton->setTextColor(Color::White);
    applyButton->setPosition(Vector2f(buttonStartX, controlY));

    float contentY = controlY + dropH + 32.f;
    float availableHeight = height - (contentY - position.y) - 24.f;
    if (availableHeight < 260.f) {
        availableHeight = 260.f;
    }
    float columnGap = 24.f;
    float contentWidth = width - margin * 2.f;
    float columnWidth = (contentWidth - columnGap) / 2.f;

    chartCard.setSize(Vector2f(columnWidth, availableHeight));
    chartCard.setPosition(Vector2f(position.x + margin, contentY));
    chartCard.setFillColor(cardColor);
    chartCard.setOutlineThickness(1.f);
    chartCard.setOutlineColor(Color(225, 228, 236));

    tableCard.setSize(Vector2f(columnWidth, availableHeight));
    tableCard.setPosition(Vector2f(chartCard.getPosition().x + columnWidth + columnGap, contentY));
    tableCard.setFillColor(cardColor);
    tableCard.setOutlineThickness(1.f);
    tableCard.setOutlineColor(Color(225, 228, 236));

    tableHeader.setSize(Vector2f(columnWidth, 56.f));
    tableHeader.setPosition(tableCard.getPosition());
    tableHeader.setFillColor(Color(247, 249, 252));
    tableHeader.setOutlineThickness(0.f);
}

void RevenuePanel::setPosition(Vector2f pos) {
    position = pos;
    initializeUI();
    populateDateDropdowns();
    applySelection();
}

long long RevenuePanel::dateToKey(const string& date) const {
    // date format: DD/MM/YYYY
    if (date.size() != 10) return 0;
    try {
        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));
        return static_cast<long long>(year) * 10000LL + static_cast<long long>(month) * 100LL + day;
    } catch (...) {
        return 0;
    }
}

string RevenuePanel::keyToDisplay(long long key) const {
    int year = static_cast<int>(key / 10000LL);
    int month = static_cast<int>((key / 100LL) % 100LL);
    int day = static_cast<int>(key % 100LL);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", day, month, year);
    return string(buffer);
}

string RevenuePanel::formatCurrency(long long amount) const {
    string digits = to_string(amount);
    int insertPosition = static_cast<int>(digits.length()) - 3;
    while (insertPosition > 0) {
        digits.insert(static_cast<size_t>(insertPosition), ".");
        insertPosition -= 3;
    }
    return digits;
}

String RevenuePanel::makeUtf8(const string& text) const {
    return String::fromUtf8(text.begin(), text.end());
}

string RevenuePanel::ellipsize(const string& text, size_t maxChars) const {
    if (text.length() <= maxChars) return text;
    if (maxChars <= 3) return text.substr(0, maxChars);
    return text.substr(0, maxChars - 3) + "...";
}

string RevenuePanel::trim(const string& text) const {
    size_t first = text.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\n\r");
    return text.substr(first, last - first + 1);
}

int RevenuePanel::countSeats(const string& seatList) const {
    if (seatList.empty()) return 0;

    int count = 0;
    size_t start = 0;
    while (start < seatList.length()) {
        size_t end = seatList.find(',', start);
        string token = (end == string::npos)
            ? seatList.substr(start)
            : seatList.substr(start, end - start);

        if (!trim(token).empty()) {
            count++;
        }

        if (end == string::npos) break;
        start = end + 1;
    }
    return count;
}

int RevenuePanel::parseComboQuantity(const string& item) const {
    size_t xPos = item.rfind('x');
    if (xPos == string::npos) return 1;

    string quantityPart = trim(item.substr(xPos + 1));
    if (quantityPart.empty()) return 1;

    try {
        int value = stoi(quantityPart);
        return (value > 0) ? value : 1;
    } catch (...) {
        return 1;
    }
}

long long RevenuePanel::computeComboCost(const string& comboList) const {
    if (comboList.empty()) return 0;

    string trimmed = trim(comboList);
    if (trimmed.empty()) return 0;

    long long total = 0;
    size_t start = 0;
    while (start < trimmed.length()) {
        size_t end = trimmed.find(',', start);
        string item = (end == string::npos)
            ? trimmed.substr(start)
            : trimmed.substr(start, end - start);
        item = trim(item);
        if (!item.empty()) {
            int quantity = parseComboQuantity(item);
            size_t xPos = item.rfind('x');
            string name = (xPos == string::npos) ? item : trim(item.substr(0, xPos));

            auto comboIt = comboPrices.find(name);
            if (comboIt != comboPrices.end()) {
                total += static_cast<long long>(comboIt->second) * quantity;
            }
        }

        if (end == string::npos) break;
        start = (end == string::npos) ? trimmed.length() : end + 1;
    }

    return total;
}

long long RevenuePanel::computeTicketRevenue(const Ticket& ticket) const {
    int seatCount = countSeats(ticket.booked);
    if (seatCount <= 0) return 0;

    auto it = showtimeSeatPrices.find(ticket.showtimeId);
    long long seatPrice = (it != showtimeSeatPrices.end()) ? it->second : 0;
    if (seatPrice > 0) {
        return seatPrice * seatCount;
    }

    long long baseAmount = static_cast<long long>(ticket.price) - computeComboCost(ticket.comboName);
    if (baseAmount <= 0) return 0;

    return baseAmount;
}

void RevenuePanel::loadData() {
    loadShowtimePrices();
    loadComboPrices();
    ticketTree.clear();
    filteredTickets.clear();
    availableKeys.clear();

    AdminTicketRepository repository(ticketsFilePath);
    DLL<Ticket> tickets = repository.loadAll();
    
    // Use a temporary DLL to store unique keys to avoid duplicates
    DLL<long long> uniqueKeys;

    Node<Ticket>* current = tickets.getHead();
    while (current) {
        long long key = dateToKey(current->data.bookedDate);
        if (key > 0) {
            ticketTree.insert(key, current->data);
            
            // Check if key exists in uniqueKeys
            bool exists = false;
            Node<long long>* kNode = uniqueKeys.getHead();
            while (kNode) {
                if (kNode->data == key) {
                    exists = true;
                    break;
                }
                kNode = kNode->next;
            }
            if (!exists) {
                uniqueKeys.push_back(key);
            }
        }
        current = current->next;
    }

    availableKeys = uniqueKeys; // Copy
    populateDateDropdowns();
    applySelection();
}

void RevenuePanel::refreshData() {
    loadData();
}

void RevenuePanel::loadShowtimePrices() {
    showtimeSeatPrices.clear();
    if (showtimesFilePath.empty()) return;

    ShowtimeRepository repository;
    DLL<Showtime> showtimes = repository.loadFromFile(makeUtf8(showtimesFilePath));

    Node<Showtime>* node = showtimes.getHead();
    while (node) {
        string showtimeId = node->data.showtime_id.toAnsiString();
        showtimeSeatPrices[showtimeId] = node->data.price;
        node = node->next;
    }
}

void RevenuePanel::loadComboPrices() {
    comboPrices.clear();
    if (combosFilePath.empty()) return;

    ComboRepository repository;
    DLL<Combo> combos = repository.loadFromFile(makeUtf8(combosFilePath));

    Node<Combo>* node = combos.getHead();
    while (node) {
        comboPrices[node->data.name.toAnsiString()] = node->data.price;
        node = node->next;
    }
}

void RevenuePanel::populateDateDropdowns() {
    // Days: 01-31
    std::vector<string> dayOpts;
    for (int i = 1; i <= 31; ++i) {
        char buf[4]; snprintf(buf, sizeof(buf), "%02d", i);
        dayOpts.push_back(buf);
    }
    fromDayDropdown->setOptions(dayOpts);
    toDayDropdown->setOptions(dayOpts);

    // Months: 01-12
    std::vector<string> monthOpts;
    for (int i = 1; i <= 12; ++i) {
        char buf[4]; snprintf(buf, sizeof(buf), "%02d", i);
        monthOpts.push_back(buf);
    }
    fromMonthDropdown->setOptions(monthOpts);
    toMonthDropdown->setOptions(monthOpts);

    // Years: Extract from availableKeys
    std::vector<string> yearOpts;
    DLL<int> uniqueYears;
    Node<long long>* kNode = availableKeys.getHead();
    while (kNode) {
        int y = static_cast<int>(kNode->data / 10000LL);
        bool exists = false;
        Node<int>* yNode = uniqueYears.getHead();
        while (yNode) {
            if (yNode->data == y) { exists = true; break; }
            yNode = yNode->next;
        }
        if (!exists) uniqueYears.push_back(y);
        kNode = kNode->next;
    }

    // Sort years (simple bubble sort on DLL or just copy to vector since UI is not "algorithm")
    // I'll just copy to vector and sort for dropdown
    std::vector<int> sortedYears;
    Node<int>* yNode = uniqueYears.getHead();
    while (yNode) {
        sortedYears.push_back(yNode->data);
        yNode = yNode->next;
    }
    std::sort(sortedYears.begin(), sortedYears.end());
    if (sortedYears.empty()) sortedYears.push_back(2025); // Default

    for (int y : sortedYears) {
        yearOpts.push_back(to_string(y));
    }
    fromYearDropdown->setOptions(yearOpts);
    toYearDropdown->setOptions(yearOpts);

    // Set defaults
    fromDayDropdown->setSelectedIndex(0);
    fromMonthDropdown->setSelectedIndex(0);
    fromYearDropdown->setSelectedIndex(0);

    toDayDropdown->setSelectedIndex(dayOpts.size() - 1);
    toMonthDropdown->setSelectedIndex(monthOpts.size() - 1);
    toYearDropdown->setSelectedIndex(yearOpts.size() - 1);
}

void RevenuePanel::applySelection() {
    string d1 = fromDayDropdown->getSelectedValue();
    string m1 = fromMonthDropdown->getSelectedValue();
    string y1 = fromYearDropdown->getSelectedValue();
    
    string d2 = toDayDropdown->getSelectedValue();
    string m2 = toMonthDropdown->getSelectedValue();
    string y2 = toYearDropdown->getSelectedValue();

    if (d1.empty() || m1.empty() || y1.empty() || d2.empty() || m2.empty() || y2.empty()) return;

    long long k1 = stoll(y1) * 10000 + stoll(m1) * 100 + stoll(d1);
    long long k2 = stoll(y2) * 10000 + stoll(m2) * 100 + stoll(d2);

    if (k1 > k2) std::swap(k1, k2);

    startKey = k1;
    endKey = k2;

    filteredTickets = ticketTree.rangeQuery(startKey, endKey);
    updateSummary();
}

void RevenuePanel::handleEvent(const Event& event, const RenderWindow& window) {
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    
    fromDayDropdown->handleEvent(event, mousePos);
    fromMonthDropdown->handleEvent(event, mousePos);
    fromYearDropdown->handleEvent(event, mousePos);
    
    toDayDropdown->handleEvent(event, mousePos);
    toMonthDropdown->handleEvent(event, mousePos);
    toYearDropdown->handleEvent(event, mousePos);

    if (const auto* mousePressed = event.getIf<Event::MouseButtonPressed>()) {
        if (mousePressed->button == Mouse::Button::Left) {
            if (applyButton && applyButton->isClicked(mousePos, true)) {
                applySelection();
            }
        }
    }
}

void RevenuePanel::update(Vector2f mousePos, bool mousePressed) {
    if (applyButton) {
        applyButton->update(mousePos, mousePressed, Color(30, 140, 200), accentColor);
    }
}

void RevenuePanel::updateSummary() {
    movieStats.clear();
    
    Node<Ticket>* curr = filteredTickets.getHead();
    while (curr) {
        string title = curr->data.title;
        long long revenue = computeTicketRevenue(curr->data);
        
        bool found = false;
        Node<MovieStats>* mNode = movieStats.getHead();
        while (mNode) {
            if (mNode->data.title == title) {
                mNode->data.revenue += revenue;
                mNode->data.ticketCount++;
                found = true;
                break;
            }
            mNode = mNode->next;
        }
        
        if (!found) {
            movieStats.push_back({title, revenue, 1});
        }
        curr = curr->next;
    }

    sortMovieStats();
}

void RevenuePanel::sortMovieStats() {
    if (movieStats.getSize() < 2) return;

    Node<MovieStats>* outer = movieStats.getHead();
    while (outer) {
        Node<MovieStats>* inner = outer->next;
        while (inner) {
            bool shouldSwap = false;
            if (mode == StatsMode::Revenue) {
                shouldSwap = inner->data.revenue > outer->data.revenue;
            } else {
                shouldSwap = inner->data.ticketCount > outer->data.ticketCount;
            }
            if (shouldSwap) {
                MovieStats temp = outer->data;
                outer->data = inner->data;
                inner->data = temp;
            }
            inner = inner->next;
        }
        outer = outer->next;
    }
}

void RevenuePanel::drawChart(RenderWindow& window) {
    Text heading(font, makeUtf8(mode == StatsMode::Revenue ? "Doanh thu theo phim" : "Số vé bán ra theo phim"), 18);
    heading.setFillColor(Color(45, 62, 80));
    heading.setStyle(Text::Bold);
    heading.setPosition(Vector2f(chartCard.getPosition().x + 20.f, chartCard.getPosition().y + 16.f));
    window.draw(heading);

    if (movieStats.isEmpty()) {
        Text empty(font, makeUtf8("Không có dữ liệu trong khoảng thời gian đã chọn."), 14);
        empty.setFillColor(Color(130, 140, 150));
        empty.setPosition(Vector2f(chartCard.getPosition().x + 20.f, chartCard.getPosition().y + 70.f));
        window.draw(empty);
        return;
    }

    const int maxBars = 7;
    int count = std::min(movieStats.getSize(), maxBars);

    long long maxRevenue = 0;
    int maxTickets = 0;
    Node<MovieStats>* node = movieStats.getHead();
    int processed = 0;
    while (node && processed < count) {
        if (mode == StatsMode::Revenue) {
            if (node->data.revenue > maxRevenue) maxRevenue = node->data.revenue;
        } else {
            if (node->data.ticketCount > maxTickets) maxTickets = node->data.ticketCount;
        }
        node = node->next;
        processed++;
    }
    if (mode == StatsMode::Revenue && maxRevenue == 0) maxRevenue = 1;
    if (mode == StatsMode::Tickets && maxTickets == 0) maxTickets = 1;

    float padding = 32.f;
    float chartWidth = chartCard.getSize().x - padding * 2.f;
    float chartHeight = chartCard.getSize().y - padding * 2.f - 40.f;
    float originX = chartCard.getPosition().x + padding;
    float originY = chartCard.getPosition().y + chartCard.getSize().y - padding;

    float gap = 16.f;
    float barWidth = (chartWidth - gap * (count + 1)) / count;
    if (barWidth < 22.f) {
        gap = 10.f;
        barWidth = (chartWidth - gap * (count + 1)) / count;
    }
    if (barWidth < 16.f) {
        barWidth = chartWidth / (count * 1.2f);
        gap = 8.f;
    }
    float startX = originX + gap;

    RectangleShape axis(Vector2f(chartWidth + 4.f, 1.5f));
    axis.setFillColor(Color(210, 213, 220));
    axis.setPosition(Vector2f(originX - 2.f, originY));
    window.draw(axis);

    node = movieStats.getHead();
    processed = 0;
    Color barColor = (mode == StatsMode::Revenue) ? secondaryColor : accentColor;

    while (node && processed < count) {
        float ratio = (mode == StatsMode::Revenue)
            ? static_cast<float>(node->data.revenue) / static_cast<float>(maxRevenue)
            : static_cast<float>(node->data.ticketCount) / static_cast<float>(maxTickets);
        float barHeight = ratio * chartHeight;
        if (barHeight < 4.f) barHeight = 4.f;

        float barX = startX + processed * (barWidth + gap);
        float barY = originY - barHeight;

        RectangleShape bar(Vector2f(barWidth, barHeight));
        bar.setFillColor(barColor);
        bar.setPosition(Vector2f(barX, barY));
        window.draw(bar);

        string valueText = (mode == StatsMode::Revenue)
            ? formatCurrency(node->data.revenue)
            : to_string(node->data.ticketCount);
        Text value(font, makeUtf8(valueText), 12);
        value.setFillColor(Color(60, 60, 60));
        FloatRect valueBounds = value.getLocalBounds();
        value.setPosition(Vector2f(barX + (barWidth - valueBounds.size.x) / 2.f - valueBounds.position.x,
                                   barY - valueBounds.size.y - 6.f));
        window.draw(value);

        string title = ellipsize(node->data.title, 14);
        Text label(font, makeUtf8(title), 11);
        label.setFillColor(Color(95, 100, 110));
        FloatRect labelBounds = label.getLocalBounds();
        label.setPosition(Vector2f(barX + (barWidth - labelBounds.size.x) / 2.f - labelBounds.position.x,
                                   originY + 4.f));
        window.draw(label);

        node = node->next;
        processed++;
    }
}

void RevenuePanel::drawTable(RenderWindow& window) {
    Text title(font, makeUtf8(mode == StatsMode::Revenue ? "Bảng doanh thu" : "Bảng số vé bán"), 16);
    title.setFillColor(Color(45, 62, 80));
    title.setStyle(Text::Bold);
    title.setPosition(Vector2f(tableHeader.getPosition().x + 20.f, tableHeader.getPosition().y + 12.f));
    window.draw(title);

    float colNameX = tableCard.getPosition().x + 20.f;
    float colSecondX = tableCard.getPosition().x + tableCard.getSize().x * 0.55f;
    float colThirdX = tableCard.getPosition().x + tableCard.getSize().x * 0.82f;

    string secondLabel = (mode == StatsMode::Revenue) ? "Doanh thu" : "Số vé";
    string thirdLabel = (mode == StatsMode::Revenue) ? "Số vé" : "Doanh thu";

    Text headerSecond(font, makeUtf8(secondLabel), 14);
    headerSecond.setFillColor(Color(120, 128, 139));
    headerSecond.setPosition(Vector2f(colSecondX, tableHeader.getPosition().y + 32.f));

    Text headerThird(font, makeUtf8(thirdLabel), 14);
    headerThird.setFillColor(Color(120, 128, 139));
    headerThird.setPosition(Vector2f(colThirdX, tableHeader.getPosition().y + 32.f));

    Text headerName(font, makeUtf8("Tên phim"), 14);
    headerName.setFillColor(Color(120, 128, 139));
    headerName.setPosition(Vector2f(colNameX, tableHeader.getPosition().y + 32.f));

    window.draw(headerName);
    window.draw(headerSecond);
    window.draw(headerThird);

    float rowY = tableHeader.getPosition().y + tableHeader.getSize().y + 16.f;
    float rowHeight = 54.f;
    float rowSpacing = 8.f;
    float textOffset = 6.f;
    int maxRows = 12;
    int rowIndex = 0;
    Node<MovieStats>* node = movieStats.getHead();

    if (!node) {
        Text empty(font, makeUtf8("Không có dữ liệu để hiển thị."), 14);
        empty.setFillColor(Color(130, 140, 150));
        empty.setPosition(Vector2f(colNameX, rowY + 10.f));
        window.draw(empty);
        return;
    }

    while (node && rowIndex < maxRows) {
        RectangleShape rowBg(Vector2f(tableCard.getSize().x - 2.f, rowHeight - rowSpacing));
        rowBg.setFillColor((rowIndex % 2 == 0) ? Color(248, 250, 252) : Color(255, 255, 255));
        rowBg.setPosition(Vector2f(tableCard.getPosition().x + 1.f, rowY - 4.f));
        rowBg.setOutlineThickness(0.f);
        window.draw(rowBg);

        string movieName = ellipsize(node->data.title, 28);
        Text movie(font, makeUtf8(movieName), 14);
        movie.setFillColor(Color(45, 62, 80));
        movie.setPosition(Vector2f(colNameX, rowY + textOffset));
        window.draw(movie);

        string secondValue = (mode == StatsMode::Revenue)
            ? formatCurrency(node->data.revenue)
            : to_string(node->data.ticketCount);
        string thirdValue = (mode == StatsMode::Revenue)
            ? to_string(node->data.ticketCount)
            : formatCurrency(node->data.revenue);

        Text second(font, makeUtf8(secondValue), 14);
        second.setFillColor(Color(52, 73, 94));
        second.setPosition(Vector2f(colSecondX, rowY + textOffset));
        window.draw(second);

        Text third(font, makeUtf8(thirdValue), 14);
        third.setFillColor(Color(52, 73, 94));
        third.setPosition(Vector2f(colThirdX, rowY + textOffset));
        window.draw(third);

        rowY += rowHeight;
        node = node->next;
        rowIndex++;
    }
}

void RevenuePanel::render(RenderWindow& window) {
    window.draw(background);
    window.draw(titleText);
    window.draw(fromLabelText);
    window.draw(toLabelText);
    
    window.draw(chartCard);
    window.draw(tableCard);
    window.draw(tableHeader);

    drawChart(window);
    drawTable(window);

    // Draw Buttons
    if (applyButton) applyButton->draw(window);

    // Draw Dropdowns LAST to ensure they are on top
    if (fromDayDropdown) fromDayDropdown->draw(window);
    if (fromMonthDropdown) fromMonthDropdown->draw(window);
    if (fromYearDropdown) fromYearDropdown->draw(window);
    if (toDayDropdown) toDayDropdown->draw(window);
    if (toMonthDropdown) toMonthDropdown->draw(window);
    if (toYearDropdown) toYearDropdown->draw(window);
}
