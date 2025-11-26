#include "UI/components/Admin/OverviewPanel.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>

using sf::Color;
using sf::FloatRect;
using sf::RenderTarget;
using sf::Vector2f;

OverviewPanel::OverviewPanel(sf::Font& fontRef, float w, float h)
    : font(fontRef),
      width(w),
      height(h),
      position(0.f, 0.f),
      ticketsFilePath(resolveDataPath("data/tickets.txt")),
      usersFilePath(resolveDataPath("data/users.txt")),
      background(Vector2f(w, h)),
      revenueTodayCard(font, {250.f, 110.f}, cardOutlineCyan),
      newCustomersCard(font, {250.f, 110.f}, cardOutlineGreen),
      ticketsCard(font, {250.f, 110.f}, cardOutlineYellow),
      totalRevenueCard(font, {250.f, 110.f}, cardOutlineRed),
      ticketChartCard(Vector2f(0.f, 0.f)),
      revenueChartCard(Vector2f(0.f, 0.f)) {
    initializeUI();
    loadData();
}

std::string OverviewPanel::resolveDataPath(const std::string& relative) const {
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

void OverviewPanel::initializeUI() {
    background.setFillColor(Color(244, 247, 252));
    background.setPosition(position);

    float margin = 24.f;
    float statWidth = (width - margin * 2.f - 24.f * 3.f) / 4.f;
    float statHeight = 120.f;
    Vector2f statSize(statWidth, statHeight);

    revenueTodayCard.setSize(statSize);
    newCustomersCard.setSize(statSize);
    ticketsCard.setSize(statSize);
    totalRevenueCard.setSize(statSize);

    revenueTodayCard.setPosition({position.x + margin, position.y + margin});
    newCustomersCard.setPosition({revenueTodayCard.getPosition().x + statWidth + 24.f, position.y + margin});
    ticketsCard.setPosition({newCustomersCard.getPosition().x + statWidth + 24.f, position.y + margin});
    totalRevenueCard.setPosition({ticketsCard.getPosition().x + statWidth + 24.f, position.y + margin});

    float chartTop = position.y + margin + statHeight + 40.f;
    float chartHeight = height - (chartTop - position.y) - margin;

    ticketChartCard.setSize({(width - margin * 2.f - 32.f) / 2.f, chartHeight});
    ticketChartCard.setPosition({position.x + margin, chartTop});
    ticketChartCard.setFillColor(Color::White);
    ticketChartCard.setOutlineThickness(1.f);
    ticketChartCard.setOutlineColor(Color(226, 232, 240));

    revenueChartCard.setSize(ticketChartCard.getSize());
    revenueChartCard.setPosition({ticketChartCard.getPosition().x + ticketChartCard.getSize().x + 32.f, chartTop});
    revenueChartCard.setFillColor(Color::White);
    revenueChartCard.setOutlineThickness(1.f);
    revenueChartCard.setOutlineColor(Color(226, 232, 240));
}

void OverviewPanel::loadData() {
    loadTickets();
    loadRegistrations();
    calculateDailyRevenue();
    calculateMonthlyStats();
    calculateTopMovies();
    calculateMonthlyTrend();
}

void OverviewPanel::loadTickets() {
    topMovies.clear();
    monthlyTrend.clear();

    AdminTicketRepository repo(ticketsFilePath);
    ticketsCache = repo.loadAll();
}

void OverviewPanel::loadRegistrations() {
    registrationTimes.clear();
    std::ifstream file(usersFilePath);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string email, passwordHash, fullName, birthDate, phone, registeredAt, role, status;
        std::getline(ss, email, '|');
        std::getline(ss, passwordHash, '|');
        std::getline(ss, fullName, '|');
        std::getline(ss, birthDate, '|');
        std::getline(ss, phone, '|');
        std::getline(ss, registeredAt, '|');
        std::getline(ss, role, '|');
        std::getline(ss, status, '|');

        if (!registeredAt.empty()) {
            registrationTimes[email] = std::stoll(registeredAt);
        }
    }
}

void OverviewPanel::calculateDailyRevenue() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);

    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y", local);
    std::string today(buffer);

    long long total = 0;
    Node<Ticket>* node = ticketsCache.getHead();
    while (node) {
        if (node->data.bookedDate == today) {
            total += node->data.price;
        }
        node = node->next;
    }

    dailyRevenue = total;
    revenueTodayCard.setTitle("Doanh thu trong ngày");
    revenueTodayCard.setOutlineThickness(3.f);
    revenueTodayCard.setOutlineColor(Color::Blue);
    revenueTodayCard.setValue(toCurrency(dailyRevenue));
    revenueTodayCard.setSubtitle(today);
}

void OverviewPanel::calculateMonthlyStats() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);

    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%m/%Y", local);
    std::string currentMonth(buffer);

    long long revenue = 0;
    int ticketsCount = 0;
    std::time_t monthStart;
    {
        std::tm temp = *local;
        temp.tm_mday = 1;
        temp.tm_hour = 0;
        temp.tm_min = 0;
        temp.tm_sec = 0;
        monthStart = std::mktime(&temp);
    }

    Node<Ticket>* node = ticketsCache.getHead();
    while (node) {
        const Ticket& ticket = node->data;
        if (ticket.bookedDate.size() >= 7) {
            std::string month = ticket.bookedDate.substr(3);
            if (month == currentMonth) {
                revenue += ticket.price;
                ticketsCount += std::max(1, countSeats(ticket.booked));
            }
        }
        node = node->next;
    }

    int newCustomers = 0;
    for (const auto& entry : registrationTimes) {
        if (entry.second >= monthStart) {
            newCustomers++;
        }
    }

    totalRevenueThisMonth = revenue;
    totalTicketsThisMonth = ticketsCount;
    newCustomersThisMonth = newCustomers;

    newCustomersCard.setTitle("Khách hàng mới");
    newCustomersCard.setOutlineThickness(3.f);
    newCustomersCard.setOutlineColor(Color::Green);
    newCustomersCard.setValue(std::to_string(newCustomersThisMonth));
    newCustomersCard.setSubtitle("T" + currentMonth);

    ticketsCard.setTitle("Tổng vé bán ra");
    ticketsCard.setOutlineThickness(3.f);
    ticketsCard.setOutlineColor(Color::Yellow);
    ticketsCard.setValue(std::to_string(totalTicketsThisMonth));
    ticketsCard.setSubtitle("T" + currentMonth);

    totalRevenueCard.setTitle("Tổng doanh thu");
    totalRevenueCard.setOutlineThickness(3.f);
    totalRevenueCard.setOutlineColor(Color::Red);
    totalRevenueCard.setValue(toCurrency(totalRevenueThisMonth));
    totalRevenueCard.setSubtitle("T" + currentMonth);
}

void OverviewPanel::calculateTopMovies() {
    topMovies.clear();

    std::unordered_map<std::string, int> ticketCounts;
    Node<Ticket>* node = ticketsCache.getHead();
    while (node) {
        ticketCounts[node->data.title] += std::max(1, countSeats(node->data.booked));
        node = node->next;
    }

    for (const auto& entry : ticketCounts) {
        topMovies.push_back({entry.first, entry.second});
    }

    sortTopMovies();
}

void OverviewPanel::calculateMonthlyTrend() {
    monthlyTrend.clear();

    std::map<std::pair<int, int>, long long> revenueByMonth; // key: {year, month}
    Node<Ticket>* node = ticketsCache.getHead();
    while (node) {
        if (node->data.bookedDate.size() >= 10) {
            int month = std::stoi(node->data.bookedDate.substr(3, 2));
            int year = std::stoi(node->data.bookedDate.substr(6, 4));
            revenueByMonth[{year, month}] += node->data.price;
        }
        node = node->next;
    }

    for (const auto& entry : revenueByMonth) {
        MonthlyRevenuePoint point{};
        point.year = entry.first.first;
        point.month = entry.first.second;
        point.revenue = entry.second;
        monthlyTrend.push_back(point);
    }

    sortMonthlyTrend();
}

int OverviewPanel::countSeats(const std::string& seatList) const {
    if (seatList.empty()) return 0;

    int count = 0;
    std::size_t start = 0;
    while (start < seatList.size()) {
        std::size_t end = seatList.find(',', start);
        std::string token = (end == std::string::npos)
            ? seatList.substr(start)
            : seatList.substr(start, end - start);

        // Trim whitespace
        std::size_t first = token.find_first_not_of(" \t\n\r");
        if (first != std::string::npos) {
            std::size_t last = token.find_last_not_of(" \t\n\r");
            token = token.substr(first, last - first + 1);
            if (!token.empty()) {
                count++;
            }
        }

        if (end == std::string::npos) break;
        start = end + 1;
    }
    return count;
}

std::string OverviewPanel::toCurrency(long long amount) const {
    std::string digits = std::to_string(amount);
    int insertPosition = static_cast<int>(digits.length()) - 3;
    while (insertPosition > 0) {
        digits.insert(static_cast<std::size_t>(insertPosition), ".");
        insertPosition -= 3;
    }
    return digits;
}

sf::String OverviewPanel::toSfString(const std::string& text) const {
    return sf::String::fromUtf8(text.begin(), text.end());
}

std::string OverviewPanel::ellipsize(const std::string& text, std::size_t maxChars) const {
    if (text.size() <= maxChars) {
        return text;
    }
    if (maxChars <= 3) {
        return text.substr(0, maxChars);
    }
    return text.substr(0, maxChars - 3) + "...";
}

void OverviewPanel::sortTopMovies() {
    std::vector<MovieTicketCount> items;
    Node<MovieTicketCount>* node = topMovies.getHead();
    while (node) {
        items.push_back(node->data);
        node = node->next;
    }

    std::sort(items.begin(), items.end(), [](const MovieTicketCount& a, const MovieTicketCount& b) {
        return a.tickets > b.tickets;
    });

    topMovies.clear();
    for (const auto& item : items) {
        topMovies.push_back(item);
    }
}

void OverviewPanel::sortMonthlyTrend() {
    std::vector<MonthlyRevenuePoint> items;
    Node<MonthlyRevenuePoint>* node = monthlyTrend.getHead();
    while (node) {
        items.push_back(node->data);
        node = node->next;
    }

    std::sort(items.begin(), items.end(), [](const MonthlyRevenuePoint& a, const MonthlyRevenuePoint& b) {
        if (a.year == b.year) return a.month < b.month;
        return a.year < b.year;
    });

    monthlyTrend.clear();
    for (const auto& item : items) {
        monthlyTrend.push_back(item);
    }
}

void OverviewPanel::drawTicketChart(sf::RenderTarget& target) const {
    sf::Text heading(font, toSfString("Phim bán chạy nhất"), 16);
    heading.setFillColor(Color(30, 41, 59));
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(sf::Vector2f(ticketChartCard.getPosition().x + 20.f, ticketChartCard.getPosition().y + 16.f));
    target.draw(heading);

    if (topMovies.isEmpty()) {
        sf::Text empty(font, toSfString("Không có dữ liệu"), 14);
        empty.setFillColor(Color(120, 130, 140));
        empty.setPosition(sf::Vector2f(ticketChartCard.getPosition().x + 20.f, ticketChartCard.getPosition().y + 70.f));
        target.draw(empty);
        return;
    }

    std::vector<MovieTicketCount> items;
    const Node<MovieTicketCount>* node = topMovies.getHead();
    while (node && items.size() < 8) {
        items.push_back(node->data);
        node = node->next;
    }

    int maxTickets = 0;
    for (const auto& item : items) {
        maxTickets = std::max(maxTickets, item.tickets);
    }
    if (maxTickets == 0) maxTickets = 1;

    float padding = 40.f;
    float chartWidth = ticketChartCard.getSize().x - padding * 2.f;
    float chartHeight = ticketChartCard.getSize().y - padding * 2.f - 40.f;
    float originX = ticketChartCard.getPosition().x + padding;
    float originY = ticketChartCard.getPosition().y + ticketChartCard.getSize().y - padding;

    int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        float y = originY - (chartHeight / gridLines) * static_cast<float>(i);
        sf::RectangleShape line({chartWidth, 1.f});
        line.setFillColor(Color(229, 233, 240));
        line.setPosition(sf::Vector2f(originX, y));
        target.draw(line);

        int labelValue = static_cast<int>(std::round(maxTickets * i / static_cast<float>(gridLines)));
        sf::Text label(font, toSfString(std::to_string(labelValue)), 12);
        label.setFillColor(Color(120, 130, 140));
        FloatRect bounds = label.getLocalBounds();
        label.setPosition(sf::Vector2f(originX - bounds.size.x - 8.f - bounds.position.x, y - bounds.size.y / 2.f - bounds.position.y));
        target.draw(label);
    }

    float gap = 16.f;
    float barWidth = (chartWidth - gap * (items.size() + 1)) / static_cast<float>(items.size());
    if (barWidth < 28.f) {
        gap = 10.f;
        barWidth = (chartWidth - gap * (items.size() + 1)) / static_cast<float>(items.size());
    }
    float startX = originX + gap;

    for (std::size_t i = 0; i < items.size(); ++i) {
        float ratio = items[i].tickets / static_cast<float>(maxTickets);
        float barHeight = ratio * chartHeight;
        if (barHeight < 4.f) barHeight = 4.f;

        float barX = startX + i * (barWidth + gap);
        float barY = originY - barHeight;

        sf::RectangleShape bar({barWidth, barHeight});
        bar.setFillColor(Color(59, 130, 246));
        bar.setPosition(sf::Vector2f(barX, barY));
        target.draw(bar);

        sf::Text value(font, toSfString(std::to_string(items[i].tickets)), 12);
        value.setFillColor(Color(30, 41, 59));
        FloatRect valueBounds = value.getLocalBounds();
        value.setPosition(sf::Vector2f(
            barX + (barWidth - valueBounds.size.x) / 2.f - valueBounds.position.x,
            barY - valueBounds.size.y - 6.f - valueBounds.position.y
        ));
        target.draw(value);

        const std::string name = ellipsize(items[i].title, 22);
        sf::Text label(font, toSfString(name), 12);
        label.setFillColor(Color(94, 106, 123));
        FloatRect labelBounds = label.getLocalBounds();
        label.setOrigin(sf::Vector2f(labelBounds.position.x, labelBounds.position.y + labelBounds.size.y));
        label.setPosition(sf::Vector2f(barX + barWidth * 0.5f, originY + 14.f));
        label.setRotation(sf::degrees(-38.f));
        target.draw(label);
    }
}

void OverviewPanel::drawRevenueChart(sf::RenderTarget& target) const {
    sf::Text heading(font, toSfString("Doanh thu theo tháng"), 16);
    heading.setFillColor(Color(30, 41, 59));
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(sf::Vector2f(revenueChartCard.getPosition().x + 20.f, revenueChartCard.getPosition().y + 16.f));
    target.draw(heading);

    if (monthlyTrend.isEmpty()) {
        sf::Text empty(font, toSfString("Không có dữ liệu"), 14);
        empty.setFillColor(Color(120, 130, 140));
        empty.setPosition(sf::Vector2f(revenueChartCard.getPosition().x + 20.f, revenueChartCard.getPosition().y + 70.f));
        target.draw(empty);
        return;
    }

    std::vector<MonthlyRevenuePoint> points;
    const Node<MonthlyRevenuePoint>* node = monthlyTrend.getHead();
    while (node) {
        points.push_back(node->data);
        node = node->next;
    }

    long long maxRevenue = 0;
    for (const auto& point : points) {
        maxRevenue = std::max(maxRevenue, point.revenue);
    }
    if (maxRevenue == 0) maxRevenue = 1;

    float padding = 50.f;
    float chartWidth = revenueChartCard.getSize().x - padding * 2.f;
    float chartHeight = revenueChartCard.getSize().y - padding * 2.f - 40.f;
    float originX = revenueChartCard.getPosition().x + padding;
    float originY = revenueChartCard.getPosition().y + revenueChartCard.getSize().y - padding;

    int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        float y = originY - (chartHeight / gridLines) * static_cast<float>(i);
        sf::RectangleShape line({chartWidth, 1.f});
        line.setFillColor(Color(229, 233, 240));
        line.setPosition(sf::Vector2f(originX, y));
        target.draw(line);

        long long labelValue = static_cast<long long>(std::round(maxRevenue * i / static_cast<float>(gridLines)));
        sf::Text label(font, toSfString(toCurrency(labelValue)), 12);
        label.setFillColor(Color(120, 130, 140));
        FloatRect bounds = label.getLocalBounds();
        label.setPosition(sf::Vector2f(originX - bounds.size.x - 8.f - bounds.position.x, y - bounds.size.y / 2.f - bounds.position.y));
        target.draw(label);
    }

    sf::VertexArray lineStrip(sf::PrimitiveType::LineStrip, points.size());
    std::vector<sf::CircleShape> dots;
    std::vector<sf::Text> monthLabels;
    dots.reserve(points.size());
    monthLabels.reserve(points.size());

    float step = points.size() > 1 ? chartWidth / static_cast<float>(points.size() - 1) : 0.f;
    for (std::size_t i = 0; i < points.size(); ++i) {
        float x = originX + step * static_cast<float>(i);
        float ratio = points[i].revenue / static_cast<float>(maxRevenue);
        float y = originY - ratio * chartHeight;

        lineStrip[i].position = {x, y};
        lineStrip[i].color = Color(236, 72, 153);

        sf::CircleShape dot(4.f);
        dot.setFillColor(Color(236, 72, 153));
        dot.setOrigin(sf::Vector2f(4.f, 4.f));
        dot.setPosition(sf::Vector2f(x, y));
        dots.push_back(dot);

        char labelBuffer[16];
        std::snprintf(labelBuffer, sizeof(labelBuffer), "%02d/%04d", points[i].month, points[i].year);
        sf::Text monthLabel(font, toSfString(labelBuffer), 12);
        monthLabel.setFillColor(Color(94, 106, 123));
        FloatRect monthBounds = monthLabel.getLocalBounds();
        monthLabel.setPosition(sf::Vector2f(
            x - monthBounds.size.x / 2.f - monthBounds.position.x,
            originY + 8.f - monthBounds.position.y
        ));
        monthLabels.push_back(monthLabel);
    }

    target.draw(lineStrip);
    for (const auto& dot : dots) {
        target.draw(dot);
    }
    for (const auto& label : monthLabels) {
        target.draw(label);
    }
}

void OverviewPanel::setPosition(const Vector2f& pos) {
    position = pos;
    initializeUI();
}

void OverviewPanel::render(RenderTarget& target) const {
    target.draw(background);
    revenueTodayCard.render(target);
    newCustomersCard.render(target);
    ticketsCard.render(target);
    totalRevenueCard.render(target);

    target.draw(ticketChartCard);
    target.draw(revenueChartCard);

    drawTicketChart(target);
    drawRevenueChart(target);
}

void OverviewPanel::refresh() {
    loadData();
}
