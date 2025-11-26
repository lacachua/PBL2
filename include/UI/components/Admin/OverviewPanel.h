#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "data-structures/BTree.h"
#include "data-structures/DLL.h"
#include "UI/components/Admin/AdminTicketRepository.h"
#include "UI/components/Admin/StatCard.h"

struct MonthlyRevenuePoint {
    int month;
    int year;
    long long revenue;
};

struct MovieTicketCount {
    std::string title;
    int tickets;
};

class OverviewPanel {
private:
    sf::Font& font;
    float width;
    float height;
    sf::Vector2f position;

    // Data paths
    std::string ticketsFilePath;
    std::string usersFilePath;

    // Data caches
    std::unordered_map<std::string, long long> registrationTimes;
    DLL<Ticket> ticketsCache;

    long long dailyRevenue = 0;
    int newCustomersThisMonth = 0;
    int totalTicketsThisMonth = 0;
    long long totalRevenueThisMonth = 0;

    DLL<MovieTicketCount> topMovies;
    DLL<MonthlyRevenuePoint> monthlyTrend;

    // UI elements
    sf::RectangleShape background;
    StatCard revenueTodayCard;
    StatCard newCustomersCard;
    StatCard ticketsCard;
    StatCard totalRevenueCard;

    sf::RectangleShape ticketChartCard;
    sf::RectangleShape revenueChartCard;

    sf::Color cardOutlineCyan = sf::Color(13, 148, 136);
    sf::Color cardOutlineGreen = sf::Color(34, 197, 94);
    sf::Color cardOutlineYellow = sf::Color(234, 179, 8);
    sf::Color cardOutlineRed = sf::Color(239, 68, 68);

    // Helpers
    std::string resolveDataPath(const std::string& relative) const;
    void initializeUI();
    void loadData();
    void loadTickets();
    void loadRegistrations();

    void calculateDailyRevenue();
    void calculateMonthlyStats();
    void calculateTopMovies();
    void calculateMonthlyTrend();

    int countSeats(const std::string& seatList) const;
    std::string toCurrency(long long amount) const;
    sf::String toSfString(const std::string& text) const;
    std::string ellipsize(const std::string& text, std::size_t maxChars) const;
    void sortTopMovies();
    void sortMonthlyTrend();

    void drawTicketChart(sf::RenderTarget& target) const;
    void drawRevenueChart(sf::RenderTarget& target) const;

public:
    OverviewPanel(sf::Font& font, float width, float height);

    void setPosition(const sf::Vector2f& pos);
    void render(sf::RenderTarget& target) const;
    void refresh();
};
