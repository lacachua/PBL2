#include "UI/components/Admin/MovieRevenuePanel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <unordered_set>

using sf::Color;
using sf::Event;
using sf::FloatRect;
using sf::RenderTarget;
using sf::RenderWindow;
using sf::Vector2f;

namespace {
    std::string formatCurrency(long long amount) {
        std::string digits = std::to_string(amount);
        int insertPos = static_cast<int>(digits.length()) - 3;
        while (insertPos > 0) {
            digits.insert(static_cast<std::size_t>(insertPos), ".");
            insertPos -= 3;
        }
        return digits + "đ";
    }

    sf::String toSfString(const std::string& text) {
        return sf::String::fromUtf8(text.begin(), text.end());
    }

    // Làm tròn lên thành số đẹp
    long long roundUpNice(long long value) {
        if (value <= 0) return 100;
        if (value <= 100) return 100;
        if (value <= 500) return 500;
        if (value <= 1000) return 1000;
        
        long long magnitude = 1;
        long long temp = value;
        while (temp >= 10) {
            temp /= 10;
            magnitude *= 10;
        }
        
        long long rounded = ((value + magnitude - 1) / magnitude) * magnitude;
        long long digit = rounded / magnitude;
        if (digit <= 1) rounded = magnitude;
        else if (digit <= 2) rounded = 2 * magnitude;
        else if (digit <= 5) rounded = 5 * magnitude;
        else rounded = 10 * magnitude;
        
        return rounded;
    }
}

MovieRevenuePanel::MovieRevenuePanel(sf::Font& fontRef, float w, float h)
        : font(fontRef),
            width(w),
            height(h),
            position(0.f, 0.f),
            ticketsFilePath(resolveDataPath("data/tickets.txt")),
            showtimesFilePath(resolveDataPath("data/showtimes.txt")),
            combosFilePath(resolveDataPath("data/combo.txt")),
            background(Vector2f(w, h)),
            titleText(fontRef, sf::String(), 20),
            fromLabel(fontRef, sf::String(), 14),
            toLabel(fontRef, sf::String(), 14),
            loadButton(fontRef, "Tải dữ liệu", sf::Vector2f(140.f, 38.f), 8.f),
            ticketsToggle(fontRef),
            revenueToggle(fontRef) {
        initializeUI();
        loadData();
}

std::string MovieRevenuePanel::resolveDataPath(const std::string& relative) const {
    namespace fs = std::filesystem;
    const std::array<std::string, 4> prefixes = {"../", "./", "../../", ""};
    for (const auto& prefix : prefixes) {
        fs::path candidate = fs::path(prefix) / relative;
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }
    return relative;
}

void MovieRevenuePanel::initializeUI() {
    background.setFillColor(Color(244, 247, 252));
    background.setPosition(position);

    titleText.setString(toSfString("Doanh thu theo phim"));
    titleText.setFillColor(Color(30, 41, 59));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition({position.x + 24.f, position.y + 18.f});

    fromLabel.setString(toSfString("Từ"));
    fromLabel.setFillColor(Color(71, 85, 105));

    toLabel.setString(toSfString("Đến"));
    toLabel.setFillColor(Color(71, 85, 105));

    const float margin = 24.f;
    const float controlY = position.y + 74.f;
    const float dropdownHeight = 38.f;
    const float yearWidth = 100.f;
    const float monthWidth = 85.f;
    const float dayWidth = 85.f;
    const float fieldGap = 8.f;
    const float labelGap = 12.f;

    fromLabel.setPosition({position.x + margin, controlY - 26.f});
    const FloatRect fromLabelBounds = fromLabel.getLocalBounds();
    const float fromFieldsX = fromLabel.getPosition().x + fromLabelBounds.position.x + fromLabelBounds.size.x + labelGap;

    toLabel.setPosition({fromFieldsX + yearWidth + monthWidth + dayWidth + fieldGap * 2.f + 24.f, controlY - 26.f});
    const FloatRect toLabelBounds = toLabel.getLocalBounds();
    const float toFieldsX = toLabel.getPosition().x + toLabelBounds.position.x + toLabelBounds.size.x + labelGap;

    fromYearDropdown = std::make_unique<DropdownBox>(font, "", fromFieldsX, controlY, yearWidth, dropdownHeight);
    fromMonthDropdown = std::make_unique<DropdownBox>(font, "", fromFieldsX + yearWidth + fieldGap, controlY, monthWidth, dropdownHeight);
    fromDayDropdown = std::make_unique<DropdownBox>(font, "", fromFieldsX + yearWidth + monthWidth + fieldGap * 2.f, controlY, dayWidth, dropdownHeight);

    toYearDropdown = std::make_unique<DropdownBox>(font, "", toFieldsX, controlY, yearWidth, dropdownHeight);
    toMonthDropdown = std::make_unique<DropdownBox>(font, "", toFieldsX + yearWidth + fieldGap, controlY, monthWidth, dropdownHeight);
    toDayDropdown = std::make_unique<DropdownBox>(font, "", toFieldsX + yearWidth + monthWidth + fieldGap * 2.f, controlY, dayWidth, dropdownHeight);

    for (auto* dropdown : {fromYearDropdown.get(), fromMonthDropdown.get(), fromDayDropdown.get(),
                           toYearDropdown.get(), toMonthDropdown.get(), toDayDropdown.get()}) {
        if (dropdown) dropdown->setMaxVisibleOptions(8);
    }

    if (fromMonthDropdown) fromMonthDropdown->setEnabled(false);
    if (fromDayDropdown) fromDayDropdown->setEnabled(false);
    if (toMonthDropdown) toMonthDropdown->setEnabled(false);
    if (toDayDropdown) toDayDropdown->setEnabled(false);

    loadButton.setSize(sf::Vector2f(140.f, dropdownHeight));
    loadButton.setRadius(8.f);
    loadButton.setFillColor(Color(59, 130, 246));
    loadButton.setOutline(0.f, Color::Transparent);
    loadButton.setTextColor(Color::White);

    auto configureToggle = [&](ModeToggle& toggle, const std::string& label) {
        toggle.shape.setSize(sf::Vector2f(210.f, dropdownHeight));
        toggle.shape.setFillColor(Color(229, 231, 235));
        toggle.shape.setOutlineThickness(1.f);
        toggle.shape.setOutlineColor(Color(209, 213, 219));
        toggle.label.setFont(font);
        toggle.label.setCharacterSize(14);
        toggle.label.setString(toSfString(label));
        toggle.label.setFillColor(Color(71, 85, 105));
        toggle.label.setStyle(sf::Text::Bold);
    };

    configureToggle(ticketsToggle, "Số vé bán ra theo phim");
    configureToggle(revenueToggle, "Doanh thu theo phim");

    const float row2Y = controlY + dropdownHeight + 12.f;
    const float toggleStartX = position.x + margin;
    const float toggleGap = 10.f;
    ticketsToggle.shape.setPosition(sf::Vector2f(toggleStartX, row2Y));
    revenueToggle.shape.setPosition(sf::Vector2f(toggleStartX + ticketsToggle.shape.getSize().x + toggleGap, row2Y));

    const FloatRect buttonBounds = loadButton.getGlobalBounds();
    const float idealButtonX = revenueToggle.shape.getPosition().x + revenueToggle.shape.getSize().x + toggleGap * 2.f;
    const float buttonX = std::min(position.x + width - margin - buttonBounds.size.x, idealButtonX);
    loadButton.setPosition(sf::Vector2f(buttonX, row2Y));

    chartCard.setSize(sf::Vector2f(width - margin * 2.f, std::max(260.f, height - (row2Y + dropdownHeight + 20.f))));
    chartCard.setPosition(sf::Vector2f(position.x + margin, row2Y + dropdownHeight + 16.f));
    chartCard.setFillColor(Color::White);
    chartCard.setOutlineThickness(1.f);
    chartCard.setOutlineColor(Color(226, 232, 240));

    setChartMode(currentChartMode);
}

void MovieRevenuePanel::loadData() {
    loadShowtimePrices();
    loadComboPrices();
    loadTickets();
    populateDropdowns();
    applySelection();
}

void MovieRevenuePanel::loadTickets() {
    ticketTree.clear();
    availableDateKeys.clear();

    AdminTicketRepository repo(ticketsFilePath);
    DLL<Ticket> tickets = repo.loadAll();

    Node<Ticket>* node = tickets.getHead();
    while (node) {
        long long key = dateToKey(node->data.bookedDate);
        if (key > 0) {
            ticketTree.insert(key, node->data);
            availableDateKeys.push_back(key);
        }
        node = node->next;
    }

    std::sort(availableDateKeys.begin(), availableDateKeys.end());
    availableDateKeys.erase(std::unique(availableDateKeys.begin(), availableDateKeys.end()), availableDateKeys.end());
}

void MovieRevenuePanel::loadShowtimePrices() {
    showtimeSeatPrices.clear();

    ShowtimeRepository repo;
    DLL<Showtime> showtimes = repo.loadFromFile(toSfString(showtimesFilePath));
    Node<Showtime>* node = showtimes.getHead();
    while (node) {
        showtimeSeatPrices[node->data.showtime_id.toAnsiString()] = node->data.price;
        node = node->next;
    }
}

void MovieRevenuePanel::loadComboPrices() {
    comboPrices.clear();

    std::ifstream file(combosFilePath);
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string id, name, priceStr;
        std::getline(ss, id, '|');
        std::getline(ss, name, '|');
        std::getline(ss, priceStr, '|');
        try {
            comboPrices[name] = std::stoi(priceStr);
        } catch (...) {
            comboPrices[name] = 0;
        }
    }
}

void MovieRevenuePanel::populateDropdowns() {
    populateYears(*fromYearDropdown);
    populateYears(*toYearDropdown);

    populateMonths(*fromMonthDropdown, *fromYearDropdown);
    populateMonths(*toMonthDropdown, *toYearDropdown);
    populateDays(*fromDayDropdown, *fromYearDropdown, *fromMonthDropdown);
    populateDays(*toDayDropdown, *toYearDropdown, *toMonthDropdown);

    if (!availableDateKeys.empty()) {
        const auto applyKey = [&](long long key,
                                  DropdownBox& yearDropdown,
                                  DropdownBox& monthDropdown,
                                  DropdownBox& dayDropdown) {
            if (key <= 0) return;
            const int year = static_cast<int>(key / 10000LL);
            const int month = static_cast<int>((key / 100LL) % 100LL);
            const int day = static_cast<int>(key % 100LL);

            const std::string yearStr = std::to_string(year);
            char monthBuffer[4];
            std::snprintf(monthBuffer, sizeof(monthBuffer), "%02d", month);
            char dayBuffer[4];
            std::snprintf(dayBuffer, sizeof(dayBuffer), "%02d", day);

            yearDropdown.setSelectedValue(yearStr);
            populateMonths(monthDropdown, yearDropdown);
            monthDropdown.setSelectedValue(monthBuffer);
            populateDays(dayDropdown, yearDropdown, monthDropdown);
            dayDropdown.setSelectedValue(dayBuffer);
        };

        applyKey(availableDateKeys.front(), *fromYearDropdown, *fromMonthDropdown, *fromDayDropdown);
        applyKey(availableDateKeys.back(), *toYearDropdown, *toMonthDropdown, *toDayDropdown);
    }

    updateDropdownEnabling();
}

void MovieRevenuePanel::populateYears(DropdownBox& dropdown) const {
    std::vector<std::string> options = {"Năm"};
    std::unordered_set<int> uniqueYears;
    for (long long key : availableDateKeys) {
        uniqueYears.insert(static_cast<int>(key / 10000LL));
    }

    std::vector<int> years(uniqueYears.begin(), uniqueYears.end());
    std::sort(years.begin(), years.end());
    for (int year : years) {
        options.push_back(std::to_string(year));
    }

    if (options.size() == 1) {
        std::time_t now = std::time(nullptr);
        if (const std::tm* local = std::localtime(&now)) {
            options.push_back(std::to_string(local->tm_year + 1900));
        }
    }

    dropdown.setOptions(options);
    dropdown.setSelectedIndex(0);
}

void MovieRevenuePanel::populateMonths(DropdownBox& dropdown, const DropdownBox& yearDropdown) const {
    const std::string selectedYear = yearDropdown.getSelectedValue();
    std::vector<std::string> options = {"Tháng"};

    if (selectedYear != "Năm" && !selectedYear.empty()) {
        for (int month = 1; month <= 12; ++month) {
            char buffer[4];
            std::snprintf(buffer, sizeof(buffer), "%02d", month);
            options.emplace_back(buffer);
        }
    }

    dropdown.setOptions(options);
    dropdown.setSelectedIndex(0);
}

void MovieRevenuePanel::populateDays(DropdownBox& dropdown, const DropdownBox& yearDropdown, const DropdownBox& monthDropdown) const {
    const std::string selectedYear = yearDropdown.getSelectedValue();
    const std::string selectedMonth = monthDropdown.getSelectedValue();
    std::vector<std::string> options = {"Ngày"};

    if (selectedYear != "Năm" && !selectedYear.empty() &&
        selectedMonth != "Tháng" && !selectedMonth.empty()) {
        const int year = std::stoi(selectedYear);
        const int month = std::stoi(selectedMonth);
        const int totalDays = daysInMonth(month, year);
        for (int day = 1; day <= totalDays; ++day) {
            char buffer[4];
            std::snprintf(buffer, sizeof(buffer), "%02d", day);
            options.emplace_back(buffer);
        }
    }

    dropdown.setOptions(options);
    dropdown.setSelectedIndex(0);
}

void MovieRevenuePanel::updateDropdownEnabling() {
    auto updateGroup = [](DropdownBox& yearDropdown, DropdownBox& monthDropdown, DropdownBox& dayDropdown) {
        const bool yearSelected = yearDropdown.getSelectedIndex() > 0;
        monthDropdown.setEnabled(yearSelected);
        if (!yearSelected) {
            monthDropdown.setSelectedIndex(0);
            dayDropdown.setSelectedIndex(0);
            dayDropdown.setEnabled(false);
            return;
        }

        const bool monthSelected = monthDropdown.getSelectedIndex() > 0;
        dayDropdown.setEnabled(monthSelected);
        if (!monthSelected) {
            dayDropdown.setSelectedIndex(0);
        }
    };

    if (fromYearDropdown && fromMonthDropdown && fromDayDropdown) {
        updateGroup(*fromYearDropdown, *fromMonthDropdown, *fromDayDropdown);
    }
    if (toYearDropdown && toMonthDropdown && toDayDropdown) {
        updateGroup(*toYearDropdown, *toMonthDropdown, *toDayDropdown);
    }
}

bool MovieRevenuePanel::isLeapYear(int year) const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int MovieRevenuePanel::daysInMonth(int month, int year) const {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
        case 4: case 6: case 9: case 11: return 30;
        case 2: return isLeapYear(year) ? 29 : 28;
        default: return 30;
    }
}

long long MovieRevenuePanel::dateToKey(const std::string& displayDate) const {
    if (displayDate.size() != 10) return 0;
    try {
        int day = std::stoi(displayDate.substr(0, 2));
        int month = std::stoi(displayDate.substr(3, 2));
        int year = std::stoi(displayDate.substr(6, 4));
        return static_cast<long long>(year) * 10000LL + static_cast<long long>(month) * 100LL + day;
    } catch (...) {
        return 0;
    }
}

std::string MovieRevenuePanel::keyToDisplay(long long key) const {
    int year = static_cast<int>(key / 10000LL);
    int month = static_cast<int>((key / 100LL) % 100LL);
    int day = static_cast<int>(key % 100LL);
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", day, month, year);
    return std::string(buffer);
}

void MovieRevenuePanel::applySelection() {
    std::string fromYear = fromYearDropdown->getSelectedValue();
    std::string fromMonth = fromMonthDropdown->getSelectedValue();
    std::string fromDay = fromDayDropdown->getSelectedValue();

    std::string toYear = toYearDropdown->getSelectedValue();
    std::string toMonth = toMonthDropdown->getSelectedValue();
    std::string toDay = toDayDropdown->getSelectedValue();

    if (fromYear == "Năm" || fromMonth == "Tháng" || fromDay == "Ngày" ||
        toYear == "Năm" || toMonth == "Tháng" || toDay == "Ngày") {
        filteredTickets.clear();
        movieStats.clear();
        return;
    }

    long long startKey = dateToKey(fromDay + "/" + fromMonth + "/" + fromYear);
    long long endKey = dateToKey(toDay + "/" + toMonth + "/" + toYear);
    if (startKey == 0 || endKey == 0) {
        return;
    }
    if (startKey > endKey) std::swap(startKey, endKey);

    filteredTickets = ticketTree.rangeQuery(startKey, endKey);
    updateMovieStats();
    sortStatsForCurrentMode();
}

void MovieRevenuePanel::updateMovieStats() {
    movieStats.clear();
    std::unordered_map<std::string, std::size_t> indexByTitle;

    Node<Ticket>* node = filteredTickets.getHead();
    while (node) {
        const Ticket& ticket = node->data;
        int seats = countSeats(ticket.booked);
        long long comboRevenue = computeComboRevenue(ticket.comboName);
        long long ticketRevenue = computeTicketRevenue(ticket, seats, comboRevenue);

        auto it = indexByTitle.find(ticket.title);
        if (it == indexByTitle.end()) {
            MovieRevenueEntry entry;
            entry.title = ticket.title;
            entry.ticketCount = seats;
            entry.comboRevenue = comboRevenue;
            entry.ticketRevenue = ticketRevenue;
            movieStats.push_back(entry);
            indexByTitle[ticket.title] = movieStats.size() - 1;
        } else {
            MovieRevenueEntry& entry = movieStats[it->second];
            entry.ticketCount += seats;
            entry.comboRevenue += comboRevenue;
            entry.ticketRevenue += ticketRevenue;
        }
        node = node->next;
    }
}

int MovieRevenuePanel::countSeats(const std::string& seatList) const {
    if (seatList.empty()) return 0;
    int count = 0;
    std::size_t start = 0;
    while (start < seatList.size()) {
        std::size_t end = seatList.find(',', start);
        std::string token = (end == std::string::npos)
            ? seatList.substr(start)
            : seatList.substr(start, end - start);
        if (!trim(token).empty()) {
            count++;
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return count;
}

std::string MovieRevenuePanel::trim(const std::string& text) const {
    std::size_t first = text.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    std::size_t last = text.find_last_not_of(" \t\n\r");
    return text.substr(first, last - first + 1);
}

std::string MovieRevenuePanel::ellipsize(const std::string& text, std::size_t maxChars) const {
    if (text.size() <= maxChars) {
        return text;
    }
    if (maxChars <= 3) {
        return text.substr(0, maxChars);
    }
    return text.substr(0, maxChars - 3) + "...";
}

long long MovieRevenuePanel::computeComboRevenue(const std::string& comboList) const {
    if (comboList.empty()) return 0;
    long long total = 0;
    std::size_t start = 0;
    while (start < comboList.size()) {
        std::size_t end = comboList.find(',', start);
        std::string item = (end == std::string::npos)
            ? comboList.substr(start)
            : comboList.substr(start, end - start);
        item = trim(item);
        if (!item.empty()) {
            int quantity = 1;
            std::size_t xPos = item.rfind('x');
            if (xPos != std::string::npos) {
                try {
                    quantity = std::max(1, std::stoi(trim(item.substr(xPos + 1))));
                } catch (...) {
                    quantity = 1;
                }
                item = trim(item.substr(0, xPos));
            }
            auto it = comboPrices.find(item);
            if (it != comboPrices.end()) {
                total += static_cast<long long>(it->second) * quantity;
            }
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return total;
}

long long MovieRevenuePanel::computeTicketRevenue(const Ticket& ticket, int seatCount, long long comboRevenue) const {
    if (seatCount <= 0) return 0;
    auto it = showtimeSeatPrices.find(ticket.showtimeId);
    if (it != showtimeSeatPrices.end() && it->second > 0) {
        return static_cast<long long>(it->second) * seatCount;
    }

    long long base = static_cast<long long>(ticket.price) - comboRevenue;
    if (base <= 0) {
        return static_cast<long long>(ticket.price);
    }
    return base;
}

void MovieRevenuePanel::setChartMode(ChartMode mode) {
    currentChartMode = mode;

    auto activate = [](ModeToggle& toggle, bool active) {
        toggle.active = active;
        if (active) {
            toggle.shape.setFillColor(Color(59, 130, 246));
            toggle.label.setFillColor(Color::White);
        } else {
            toggle.shape.setFillColor(Color(229, 231, 235));
            toggle.label.setFillColor(Color(71, 85, 105));
        }
    };

    activate(ticketsToggle, mode == ChartMode::Tickets);
    activate(revenueToggle, mode == ChartMode::Revenue);

    auto centerLabel = [](ModeToggle& toggle) {
        FloatRect bounds = toggle.label.getLocalBounds();
        toggle.label.setPosition(sf::Vector2f(
            toggle.shape.getPosition().x + (toggle.shape.getSize().x - bounds.size.x) / 2.f - bounds.position.x,
            toggle.shape.getPosition().y + (toggle.shape.getSize().y - bounds.size.y) / 2.f - bounds.position.y
        ));
    };

    centerLabel(ticketsToggle);
    centerLabel(revenueToggle);

    sortStatsForCurrentMode();
}

void MovieRevenuePanel::sortStatsForCurrentMode() {
    if (movieStats.empty()) {
        return;
    }

    if (currentChartMode == ChartMode::Revenue) {
        std::stable_sort(movieStats.begin(), movieStats.end(), [](const MovieRevenueEntry& a, const MovieRevenueEntry& b) {
            const long long aValue = a.totalRevenue();
            const long long bValue = b.totalRevenue();
            if (aValue == bValue) {
                if (a.ticketCount == b.ticketCount) {
                    return a.title < b.title;
                }
                return a.ticketCount > b.ticketCount;
            }
            return aValue > bValue;
        });
    } else {
        std::stable_sort(movieStats.begin(), movieStats.end(), [](const MovieRevenueEntry& a, const MovieRevenueEntry& b) {
            if (a.ticketCount == b.ticketCount) {
                const long long aValue = a.totalRevenue();
                const long long bValue = b.totalRevenue();
                if (aValue == bValue) {
                    return a.title < b.title;
                }
                return aValue > bValue;
            }
            return a.ticketCount > b.ticketCount;
        });
    }
}

void MovieRevenuePanel::drawChart(RenderTarget& target) const {
    target.draw(chartCard);

    const std::string headingText = (currentChartMode == ChartMode::Revenue)
        ? "Doanh thu theo phim"
        : "Số vé bán ra theo phim";
    sf::Text heading(font, toSfString(headingText), 16);
    heading.setFillColor(Color(30, 41, 59));
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(sf::Vector2f(chartCard.getPosition().x + 24.f, chartCard.getPosition().y + 14.f));
    target.draw(heading);

    if (movieStats.empty()) {
        sf::Text empty(font, toSfString("Không có dữ liệu"), 14);
        empty.setFillColor(Color(120, 130, 140));
        empty.setPosition(sf::Vector2f(chartCard.getPosition().x + 24.f, chartCard.getPosition().y + 70.f));
        target.draw(empty);
        return;
    }

    const std::size_t barCount = std::min<std::size_t>(movieStats.size(), 12);
    
    // Tính maxValue động từ dữ liệu thực
    double actualMaxValue = 0.0;
    for (std::size_t i = 0; i < barCount; ++i) {
        double value = (currentChartMode == ChartMode::Revenue)
            ? static_cast<double>(movieStats[i].totalRevenue())
            : static_cast<double>(movieStats[i].ticketCount);
        if (value > actualMaxValue) {
            actualMaxValue = value;
        }
    }
    
    // Làm tròn maxValue thành số đẹp
    double maxValue = static_cast<double>(roundUpNice(static_cast<long long>(actualMaxValue)));
    if (maxValue <= 0.0) {
        maxValue = (currentChartMode == ChartMode::Revenue) ? 1000000.0 : 100.0;
    }

    const float paddingLeft = (currentChartMode == ChartMode::Revenue) ? 100.f : 60.f;
    const float paddingRight = 30.f;
    const float paddingTop = 50.f;
    const float paddingBottom = 80.f;
    const float chartWidth = std::max(60.f, chartCard.getSize().x - paddingLeft - paddingRight);
    const float chartHeight = std::max(80.f, chartCard.getSize().y - paddingTop - paddingBottom);
    const float originX = chartCard.getPosition().x + paddingLeft;
    const float originY = chartCard.getPosition().y + chartCard.getSize().y - paddingBottom;

    const int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        const float ratio = static_cast<float>(i) / static_cast<float>(gridLines);
        const float y = originY - ratio * chartHeight;

        sf::RectangleShape line({chartWidth, 1.f});
        line.setFillColor(Color(229, 233, 240));
        line.setPosition(sf::Vector2f(originX, y));
        target.draw(line);

        const double rawValue = maxValue * ratio;
        std::string labelStr;
        if (currentChartMode == ChartMode::Revenue) {
            long long value = static_cast<long long>(std::round(rawValue));
            labelStr = formatCurrency(value);
        } else {
            labelStr = std::to_string(static_cast<int>(std::round(rawValue)));
        }

        sf::Text label(font, toSfString(labelStr), 10);
        label.setFillColor(Color(120, 130, 140));
        FloatRect bounds = label.getLocalBounds();
        label.setPosition(sf::Vector2f(
            originX - bounds.size.x - 8.f - bounds.position.x,
            y - bounds.size.y / 2.f - bounds.position.y
        ));
        target.draw(label);
    }

    sf::RectangleShape axis({chartWidth, 2.f});
    axis.setFillColor(Color(203, 213, 225));
    axis.setPosition(sf::Vector2f(originX, originY));
    target.draw(axis);

    float gap = 16.f;
    float barWidth = (chartWidth - gap * (static_cast<float>(barCount) + 1.f)) / static_cast<float>(barCount);
    if (barWidth < 28.f) {
        gap = 8.f;
        barWidth = (chartWidth - gap * (static_cast<float>(barCount) + 1.f)) / static_cast<float>(barCount);
    }
    if (barWidth > 70.f) barWidth = 70.f;
    if (barWidth < 20.f) barWidth = 20.f;

    float totalBarsWidth = barCount * barWidth + (barCount + 1) * gap;
    float startX = originX + (chartWidth - totalBarsWidth) / 2.f + gap;

    for (std::size_t i = 0; i < barCount; ++i) {
        const float x = startX + static_cast<float>(i) * (barWidth + gap);
        const double rawValue = (currentChartMode == ChartMode::Revenue)
            ? static_cast<double>(movieStats[i].totalRevenue())
            : static_cast<double>(movieStats[i].ticketCount);
        float barHeight = static_cast<float>((rawValue / maxValue) * chartHeight);
        if (barHeight < 4.f) {
            barHeight = 4.f;
        }

        sf::RectangleShape bar({barWidth, barHeight});
        bar.setFillColor(currentChartMode == ChartMode::Revenue ? Color(236, 72, 153) : Color(59, 130, 246));
        bar.setPosition(sf::Vector2f(x, originY - barHeight));
        target.draw(bar);

        // Giá trị trên cột
        std::string valueStr;
        if (currentChartMode == ChartMode::Revenue) {
            long long val = static_cast<long long>(std::round(rawValue));
            valueStr = formatCurrency(val);
        } else {
            valueStr = std::to_string(static_cast<int>(std::round(rawValue)));
        }

        sf::Text valueText(font, toSfString(valueStr), 9);
        valueText.setStyle(sf::Text::Bold);
        FloatRect valueBounds = valueText.getLocalBounds();
        
        float valueY;
        if (barHeight > valueBounds.size.y + 20.f) {
            valueY = originY - barHeight + 6.f - valueBounds.position.y;
            valueText.setFillColor(Color::White);
        } else {
            valueY = originY - barHeight - valueBounds.size.y - 4.f - valueBounds.position.y;
            valueText.setFillColor(Color(30, 41, 59));
            const float minY = chartCard.getPosition().y + paddingTop;
            if (valueY < minY) {
                valueY = originY - barHeight + 6.f - valueBounds.position.y;
                valueText.setFillColor(Color::White);
            }
        }
        
        valueText.setPosition(sf::Vector2f(
            x + (barWidth - valueBounds.size.x) / 2.f - valueBounds.position.x,
            valueY
        ));
        target.draw(valueText);

        // Nhãn phim
        const std::string labelText = ellipsize(movieStats[i].title, 16);
        sf::Text label(font, toSfString(labelText), 10);
        label.setFillColor(Color(94, 106, 123));
        FloatRect labelBounds = label.getLocalBounds();
        label.setOrigin(sf::Vector2f(
            labelBounds.position.x + labelBounds.size.x * 0.5f,
            labelBounds.position.y + labelBounds.size.y
        ));
        label.setPosition(sf::Vector2f(x + barWidth * 0.5f, originY + 10.f));
        label.setRotation(sf::degrees(-35.f));
        target.draw(label);
    }
}

void MovieRevenuePanel::setPosition(const sf::Vector2f& pos) {
    position = pos;
    initializeUI();
}

void MovieRevenuePanel::handleEvent(const Event& event, const RenderWindow& window) {
    const sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    auto handleDropdownGroup = [&](DropdownBox* year, DropdownBox* month, DropdownBox* day, bool& changed) {
        if (!year || !month || !day) {
            return;
        }

        const std::string prevYear = year->getSelectedValue();
        const std::string prevMonth = month->getSelectedValue();
        const std::string prevDay = day->getSelectedValue();

        year->handleEvent(event, mousePos);
        if (year->getSelectedValue() != prevYear) {
            populateMonths(*month, *year);
            populateDays(*day, *year, *month);
            changed = true;
        }

        month->handleEvent(event, mousePos);
        if (month->getSelectedValue() != prevMonth) {
            populateDays(*day, *year, *month);
            changed = true;
        }

        day->handleEvent(event, mousePos);
        if (day->getSelectedValue() != prevDay) {
            changed = true;
        }
    };

    bool dropdownsChanged = false;
    handleDropdownGroup(fromYearDropdown.get(), fromMonthDropdown.get(), fromDayDropdown.get(), dropdownsChanged);
    handleDropdownGroup(toYearDropdown.get(), toMonthDropdown.get(), toDayDropdown.get(), dropdownsChanged);

    if (dropdownsChanged) {
        updateDropdownEnabling();
    }

    if (const auto* mousePressed = event.getIf<Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            if (ticketsToggle.shape.getGlobalBounds().contains(mousePos)) {
                setChartMode(ChartMode::Tickets);
            } else if (revenueToggle.shape.getGlobalBounds().contains(mousePos)) {
                setChartMode(ChartMode::Revenue);
            }
        }
    }
}

void MovieRevenuePanel::update(const sf::Vector2f& mousePos, bool mouseDown) {
    const sf::Color primary = Color(59, 130, 246);
    const sf::Color hover = Color(37, 99, 235);
    const sf::Color active = Color(29, 78, 216);
    loadButton.update(mousePos, mouseDown, hover, active);
    if (loadButton.isClicked(mousePos, mouseDown)) {
        applySelection();
    }

    auto updateToggle = [&](ModeToggle& toggle, bool isActive) {
        const bool hovered = toggle.shape.getGlobalBounds().contains(mousePos);
        if (isActive) {
            toggle.shape.setFillColor(primary);
            toggle.label.setFillColor(Color::White);
        } else {
            toggle.shape.setFillColor(hovered ? Color(241, 245, 249) : Color(229, 231, 235));
            toggle.label.setFillColor(Color(71, 85, 105));
        }
    };

    updateToggle(ticketsToggle, currentChartMode == ChartMode::Tickets);
    updateToggle(revenueToggle, currentChartMode == ChartMode::Revenue);

    if (fromYearDropdown) fromYearDropdown->update();
    if (fromMonthDropdown) fromMonthDropdown->update();
    if (fromDayDropdown) fromDayDropdown->update();
    if (toYearDropdown) toYearDropdown->update();
    if (toMonthDropdown) toMonthDropdown->update();
    if (toDayDropdown) toDayDropdown->update();
}

void MovieRevenuePanel::render(RenderTarget& target) const {
    target.draw(background);
    target.draw(titleText);
    target.draw(fromLabel);
    target.draw(toLabel);

    target.draw(ticketsToggle.shape);
    target.draw(ticketsToggle.label);
    target.draw(revenueToggle.shape);
    target.draw(revenueToggle.label);

    loadButton.draw(target);
    drawChart(target);

    if (auto* window = dynamic_cast<RenderWindow*>(&target)) {
        if (fromYearDropdown) fromYearDropdown->draw(*window);
        if (fromMonthDropdown) fromMonthDropdown->draw(*window);
        if (fromDayDropdown) fromDayDropdown->draw(*window);
        if (toYearDropdown) toYearDropdown->draw(*window);
        if (toMonthDropdown) toMonthDropdown->draw(*window);
        if (toDayDropdown) toDayDropdown->draw(*window);
    }
}

void MovieRevenuePanel::refresh() {
    loadData();
}
