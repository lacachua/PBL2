#include "UI/components/Admin/RevenuePanel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <iostream>

using sf::Color;
using sf::Event;
using sf::RectangleShape;
using sf::RenderWindow;
using sf::Vector2f;

RevenuePanel::RevenuePanel(Font& fontRef, float w, float h)
        : font(fontRef), width(w), height(h), position(0.f, 0.f),
          titleText(font, String(), 28),
          rangeLabel(font, String(), 18) {
    ticketsFilePath = resolveDataPath("data/tickets.txt");
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
    background.setFillColor(Color(240, 245, 250)); // Light background
    background.setPosition(position);

    // Title
    titleText.setString(makeUtf8("Dashboard / Doanh thu theo phim"));
    titleText.setFillColor(Color(100, 100, 100));
    titleText.setCharacterSize(16);
    titleText.setPosition(Vector2f(position.x + 24.f, position.y + 20.f));

    // Range Selection Area
    float rangeY = position.y + 60.f;
    
    // Dropdowns
    float dropH = 36.f;
    float dropW = 70.f;
    float gap = 10.f;
    float startX = position.x + 24.f;

    fromDayDropdown = make_unique<DropdownBox>(font, "", startX, rangeY, dropW, dropH);
    fromMonthDropdown = make_unique<DropdownBox>(font, "", startX + dropW + gap, rangeY, dropW, dropH);
    fromYearDropdown = make_unique<DropdownBox>(font, "", startX + (dropW + gap) * 2, rangeY, 90.f, dropH);

    float toX = startX + (dropW + gap) * 2 + 90.f + 30.f;
    
    toDayDropdown = make_unique<DropdownBox>(font, "", toX, rangeY, dropW, dropH);
    toMonthDropdown = make_unique<DropdownBox>(font, "", toX + dropW + gap, rangeY, dropW, dropH);
    toYearDropdown = make_unique<DropdownBox>(font, "", toX + (dropW + gap) * 2, rangeY, 90.f, dropH);

    float btnX = toX + (dropW + gap) * 2 + 90.f + 30.f;
    applyButton = make_unique<Button>(font, makeUtf8("Load dữ liệu"), btnX, dropH, 14);
    applyButton->setFillColor(accentColor); // Blue
    applyButton->setTextColor(Color::White);
    applyButton->setPosition(Vector2f(btnX, rangeY));

    exportButton = make_unique<Button>(font, makeUtf8("Xuất báo cáo"), btnX + 140.f, dropH, 14);
    exportButton->setFillColor(Color(46, 204, 113)); // Green
    exportButton->setTextColor(Color::White);
    exportButton->setPosition(Vector2f(btnX + 140.f, rangeY));

    // Charts
    float chartY = rangeY + 60.f;
    float chartH = (height - chartY - 250.f); // Leave 250 for table
    if (chartH < 200.f) chartH = 200.f;
    float chartW = (width - 48.f - 20.f) / 2.f;

    leftChartArea.setSize(Vector2f(chartW, chartH));
    leftChartArea.setPosition(Vector2f(position.x + 24.f, chartY));
    leftChartArea.setFillColor(Color::White);
    
    rightChartArea.setSize(Vector2f(chartW, chartH));
    rightChartArea.setPosition(Vector2f(position.x + 24.f + chartW + 20.f, chartY));
    rightChartArea.setFillColor(Color::White);

    // Table
    float tableY = chartY + chartH + 20.f;
    tableHeader.setSize(Vector2f(width - 48.f, 40.f));
    tableHeader.setPosition(Vector2f(position.x + 24.f, tableY));
    tableHeader.setFillColor(Color(250, 250, 250));
    tableHeader.setOutlineThickness(1.f);
    tableHeader.setOutlineColor(Color(230, 230, 230));

    tableBackground.setSize(Vector2f(width - 48.f, height - tableY - 20.f));
    tableBackground.setPosition(Vector2f(position.x + 24.f, tableY));
    tableBackground.setFillColor(Color::White);
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

void RevenuePanel::loadData() {
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

void RevenuePanel::populateDateDropdowns() {
    // Days: 01-31
    vector<string> dayOpts;
    for (int i = 1; i <= 31; ++i) {
        char buf[4]; snprintf(buf, sizeof(buf), "%02d", i);
        dayOpts.push_back(buf);
    }
    fromDayDropdown->setOptions(dayOpts);
    toDayDropdown->setOptions(dayOpts);

    // Months: 01-12
    vector<string> monthOpts;
    for (int i = 1; i <= 12; ++i) {
        char buf[4]; snprintf(buf, sizeof(buf), "%02d", i);
        monthOpts.push_back(buf);
    }
    fromMonthDropdown->setOptions(monthOpts);
    toMonthDropdown->setOptions(monthOpts);

    // Years: Extract from availableKeys
    vector<string> yearOpts;
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
    vector<int> sortedYears;
    Node<int>* yNode = uniqueYears.getHead();
    while (yNode) {
        sortedYears.push_back(yNode->data);
        yNode = yNode->next;
    }
    sort(sortedYears.begin(), sortedYears.end());
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

    if (k1 > k2) swap(k1, k2);

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
            if (exportButton && exportButton->isClicked(mousePos, true)) {
                // Export logic placeholder
            }
        }
    }
}

void RevenuePanel::update(Vector2f mousePos, bool mousePressed) {
    if (applyButton) {
        applyButton->update(mousePos, mousePressed, Color(30, 140, 200), accentColor);
    }
    if (exportButton) {
        exportButton->update(mousePos, mousePressed, Color(39, 174, 96), Color(46, 204, 113));
    }
}

void RevenuePanel::updateSummary() {
    movieStats.clear();
    
    Node<Ticket>* curr = filteredTickets.getHead();
    while (curr) {
        string title = curr->data.title;
        long long price = curr->data.price;
        
        bool found = false;
        Node<MovieStats>* mNode = movieStats.getHead();
        while (mNode) {
            if (mNode->data.title == title) {
                mNode->data.revenue += price;
                mNode->data.ticketCount++;
                found = true;
                break;
            }
            mNode = mNode->next;
        }
        
        if (!found) {
            movieStats.push_back({title, price, 1});
        }
        curr = curr->next;
    }
}

void RevenuePanel::drawLeftChart(RenderWindow& window) {
    // Ticket Count Chart
    if (movieStats.isEmpty()) return;

    // Find max count
    int maxCount = 0;
    Node<MovieStats>* mNode = movieStats.getHead();
    while (mNode) {
        if (mNode->data.ticketCount > maxCount) maxCount = mNode->data.ticketCount;
        mNode = mNode->next;
    }
    if (maxCount == 0) maxCount = 1;

    float chartW = leftChartArea.getSize().x - 60.f;
    float chartH = leftChartArea.getSize().y - 60.f;
    float startX = leftChartArea.getPosition().x + 40.f;
    float startY = leftChartArea.getPosition().y + leftChartArea.getSize().y - 30.f;

    int count = movieStats.getSize();
    float barWidth = min(40.f, chartW / count - 10.f);
    
    int idx = 0;
    mNode = movieStats.getHead();
    while (mNode) {
        float h = (float)mNode->data.ticketCount / maxCount * chartH;
        RectangleShape bar(Vector2f(barWidth, h));
        bar.setFillColor(Color(135, 206, 250)); // Light Blue
        bar.setPosition(Vector2f(startX + idx * (barWidth + 10.f), startY - h));
        window.draw(bar);

        // Truncated Label
        string title = mNode->data.title;
        if (title.length() > 8) title = title.substr(0, 6) + "..";
        Text lbl(font, makeUtf8(title), 10);
        lbl.setFillColor(Color::Black);
        lbl.setPosition(Vector2f(bar.getPosition().x, startY + 5.f));
        window.draw(lbl);

        // Value
        Text val(font, to_string(mNode->data.ticketCount), 10);
        val.setFillColor(Color::Black);
        val.setPosition(Vector2f(bar.getPosition().x + (barWidth - val.getLocalBounds().size.x)/2.f, bar.getPosition().y - 15.f));
        window.draw(val);

        mNode = mNode->next;
        idx++;
    }

    Text title(font, makeUtf8("Số vé bán ra theo phim"), 14);
    title.setFillColor(Color::Black);
    title.setPosition(Vector2f(leftChartArea.getPosition().x + leftChartArea.getSize().x/2.f - 60.f, leftChartArea.getPosition().y + 10.f));
    window.draw(title);
}

void RevenuePanel::drawRightChart(RenderWindow& window) {
    // Revenue Chart
    if (movieStats.isEmpty()) return;

    // Find max revenue
    long long maxRev = 0;
    Node<MovieStats>* mNode = movieStats.getHead();
    while (mNode) {
        if (mNode->data.revenue > maxRev) maxRev = mNode->data.revenue;
        mNode = mNode->next;
    }
    if (maxRev == 0) maxRev = 1;

    float chartW = rightChartArea.getSize().x - 60.f;
    float chartH = rightChartArea.getSize().y - 60.f;
    float startX = rightChartArea.getPosition().x + 40.f;
    float startY = rightChartArea.getPosition().y + rightChartArea.getSize().y - 30.f;

    int count = movieStats.getSize();
    float barWidth = min(40.f, chartW / count - 10.f);
    
    int idx = 0;
    mNode = movieStats.getHead();
    while (mNode) {
        float h = (float)mNode->data.revenue / maxRev * chartH;
        RectangleShape bar(Vector2f(barWidth, h));
        bar.setFillColor(Color(255, 182, 193)); // Light Pink
        bar.setPosition(Vector2f(startX + idx * (barWidth + 10.f), startY - h));
        window.draw(bar);

        // Truncated Label
        string title = mNode->data.title;
        if (title.length() > 8) title = title.substr(0, 6) + "..";
        Text lbl(font, makeUtf8(title), 10);
        lbl.setFillColor(Color::Black);
        lbl.setPosition(Vector2f(bar.getPosition().x, startY + 5.f));
        window.draw(lbl);

        mNode = mNode->next;
        idx++;
    }

    Text title(font, makeUtf8("Doanh thu theo phim"), 14);
    title.setFillColor(Color::Black);
    title.setPosition(Vector2f(rightChartArea.getPosition().x + rightChartArea.getSize().x/2.f - 60.f, rightChartArea.getPosition().y + 10.f));
    window.draw(title);
}

void RevenuePanel::drawTable(RenderWindow& window) {
    // Headers
    float y = tableHeader.getPosition().y + 10.f;
    float x1 = tableHeader.getPosition().x + 20.f;
    float x2 = x1 + 400.f;
    float x3 = x2 + 200.f;

    Text h1(font, makeUtf8("Tên phim"), 14); h1.setFillColor(Color::Black); h1.setPosition(Vector2f(x1, y));
    Text h2(font, makeUtf8("Tổng vé bán ra"), 14); h2.setFillColor(Color::Black); h2.setPosition(Vector2f(x2, y));
    Text h3(font, makeUtf8("Tổng doanh thu"), 14); h3.setFillColor(Color::Black); h3.setPosition(Vector2f(x3, y));
    
    window.draw(h1); window.draw(h2); window.draw(h3);

    // Rows
    float rowY = tableHeader.getPosition().y + 40.f;
    Node<MovieStats>* mNode = movieStats.getHead();
    int i = 0;
    while (mNode && i < 10) { // Limit rows for view
        Text t1(font, makeUtf8(mNode->data.title), 14); 
        t1.setFillColor(accentColor); 
        t1.setPosition(Vector2f(x1, rowY));
        
        Text t2(font, to_string(mNode->data.ticketCount), 14); 
        t2.setFillColor(Color::Black); 
        t2.setPosition(Vector2f(x2, rowY));
        
        Text t3(font, formatCurrency(mNode->data.revenue), 14); 
        t3.setFillColor(Color::Black); 
        t3.setPosition(Vector2f(x3, rowY));

        window.draw(t1); window.draw(t2); window.draw(t3);
        
        // Separator line
        RectangleShape line(Vector2f(tableHeader.getSize().x, 1.f));
        line.setFillColor(Color(240, 240, 240));
        line.setPosition(Vector2f(tableHeader.getPosition().x, rowY + 30.f));
        window.draw(line);

        rowY += 40.f;
        mNode = mNode->next;
        i++;
    }
}

void RevenuePanel::render(RenderWindow& window) {
    window.draw(background);
    window.draw(titleText);
    
    // Draw Charts Background
    window.draw(leftChartArea);
    window.draw(rightChartArea);
    
    // Draw Table Background
    window.draw(tableBackground);
    window.draw(tableHeader);

    // Draw Content
    drawLeftChart(window);
    drawRightChart(window);
    drawTable(window);

    // Draw Buttons
    if (applyButton) applyButton->draw(window);
    if (exportButton) exportButton->draw(window);

    // Draw Dropdowns LAST to ensure they are on top
    if (fromDayDropdown) fromDayDropdown->draw(window);
    if (fromMonthDropdown) fromMonthDropdown->draw(window);
    if (fromYearDropdown) fromYearDropdown->draw(window);
    if (toDayDropdown) toDayDropdown->draw(window);
    if (toMonthDropdown) toMonthDropdown->draw(window);
    if (toYearDropdown) toYearDropdown->draw(window);
}
