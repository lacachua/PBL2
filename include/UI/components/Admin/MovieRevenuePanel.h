#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "data-structures/BTree.h"
#include "data-structures/DLL.h"
#include "repositories/admin/AdminTicketRepository.h"
#include "UI/components/Admin/DropdownBox.h"
#include "UI/components/Admin/RoundRectButton.h"
#include "repositories/booking/ShowtimeRepository.h"

struct MovieRevenueEntry {
    std::string title;
    long long ticketRevenue = 0;
    long long comboRevenue = 0;
    int ticketCount = 0;

    long long totalRevenue() const {
        return ticketRevenue + comboRevenue;
    }
};

enum class ChartMode {
    Tickets,
    Revenue
};

struct ModeToggle {
    sf::RectangleShape shape;
    sf::Text label;
    bool active = false;

    explicit ModeToggle(sf::Font& font, unsigned int characterSize = 13)
        : label(font, sf::String(), characterSize) {}

    ModeToggle(const ModeToggle&) = default;
    ModeToggle& operator=(const ModeToggle&) = default;
};

class MovieRevenuePanel {
private:
    sf::Font& font;
    float width;
    float height;
    sf::Vector2f position;

    // Data
    BTree<long long, Ticket> ticketTree;
    std::unordered_map<std::string, int> showtimeSeatPrices;
    std::unordered_map<std::string, int> comboPrices;

    DLL<Ticket> filteredTickets;
    std::vector<MovieRevenueEntry> movieStats;
    std::vector<long long> availableDateKeys;

    // Paths
    std::string ticketsFilePath;
    std::string showtimesFilePath;
    std::string combosFilePath;

    // UI Elements
    sf::RectangleShape background;
    sf::Text titleText;
    sf::Text fromLabel;
    sf::Text toLabel;

    std::unique_ptr<DropdownBox> fromYearDropdown;
    std::unique_ptr<DropdownBox> fromMonthDropdown;
    std::unique_ptr<DropdownBox> fromDayDropdown;

    std::unique_ptr<DropdownBox> toYearDropdown;
    std::unique_ptr<DropdownBox> toMonthDropdown;
    std::unique_ptr<DropdownBox> toDayDropdown;

    RoundRectButton loadButton;

    ModeToggle ticketsToggle;
    ModeToggle revenueToggle;
    ChartMode currentChartMode = ChartMode::Tickets;

    sf::RectangleShape chartCard;

    // Helpers
    std::string resolveDataPath(const std::string& relative) const;
    void initializeUI();

    void loadData();
    void loadTickets();
    void loadShowtimePrices();
    void loadComboPrices();

    void populateDropdowns();
    void populateYears(DropdownBox& dropdown) const;
    void populateMonths(DropdownBox& dropdown, const DropdownBox& yearDropdown) const;
    void populateDays(DropdownBox& dropdown, const DropdownBox& yearDropdown, const DropdownBox& monthDropdown) const;
    void updateDropdownEnabling();

    bool isLeapYear(int year) const;
    int daysInMonth(int month, int year) const;

    long long dateToKey(const std::string& displayDate) const; // expects dd/mm/yyyy
    std::string keyToDisplay(long long key) const;

    void applySelection();
    void updateMovieStats();

    int countSeats(const std::string& seatList) const;
    std::string trim(const std::string& text) const;
    std::string ellipsize(const std::string& text, std::size_t maxChars) const;
    long long computeComboRevenue(const std::string& comboList) const;
    long long computeTicketRevenue(const Ticket& ticket, int seatCount, long long comboRevenue) const;

    void setChartMode(ChartMode mode);
    void sortStatsForCurrentMode();
    void drawChart(sf::RenderTarget& target) const;

public:
    MovieRevenuePanel(sf::Font& font, float width, float height);

    void setPosition(const sf::Vector2f& pos);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::Vector2f& mousePos, bool mouseDown);
    void render(sf::RenderTarget& target) const;
    void refresh();
};
