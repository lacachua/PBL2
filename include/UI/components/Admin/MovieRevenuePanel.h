#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "data-structures/BTree.h"
#include "data-structures/DLL.h"
#include "repositories/admin/AdminTicketRepository.h"
#include "UI/components/Admin/DropdownBox.h"
#include "UI/components/Admin/RoundRectButton.h"
#include "UI/components/Admin/StatCard.h"
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
    Revenue,
    MovieRevenue,
    ComboRevenue
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
    std::unordered_set<std::string> knownMovieTitles;

    DLL<Ticket> filteredTickets;
    std::vector<MovieRevenueEntry> movieStats;
    struct ComboRevenueEntry {
        std::string label;
        int quantity = 0;
        long long revenue = 0;
    };
    std::vector<ComboRevenueEntry> comboStats;
    std::vector<long long> availableDateKeys;

    long long selectedStartKey = 0;
    long long selectedEndKey = 0;

    long long totalRevenueValue = 0;
    long long movieRevenueValue = 0;
    long long comboRevenueValue = 0;

    bool suppressClicksUntilMouseRelease = false;

    // Paths
    std::string ticketsFilePath;
    std::string moviesFilePath;
    std::string showtimesFilePath;
    std::string combosFilePath;

    // UI Elements
    sf::RectangleShape background;
    sf::Text titleText;
    sf::Text fromLabel;
    sf::Text toLabel;

    StatCard totalRevenueCard;
    StatCard movieRevenueCard;
    StatCard comboRevenueCard;

    std::unique_ptr<DropdownBox> fromYearDropdown;
    std::unique_ptr<DropdownBox> fromMonthDropdown;
    std::unique_ptr<DropdownBox> fromDayDropdown;

    std::unique_ptr<DropdownBox> toYearDropdown;
    std::unique_ptr<DropdownBox> toMonthDropdown;
    std::unique_ptr<DropdownBox> toDayDropdown;

    RoundRectButton loadButton;

    ModeToggle ticketsToggle;
    ModeToggle revenueToggle;
    ModeToggle movieOnlyToggle;
    ModeToggle comboToggle;
    ChartMode currentChartMode = ChartMode::Tickets;

    sf::RectangleShape chartCard;

    // Helpers
    std::string resolveDataPath(const std::string& relative) const;
    void initializeUI();
    void layoutComponents();

    void loadData();
    void loadMovies();
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
    void updateComboStats();
    void updateSummaryCards();
    std::string currentRangeLabel() const;
    bool isAnyDropdownOpen() const;

    int countSeats(const std::string& seatList) const;
    std::string trim(const std::string& text) const;
    std::string ellipsize(const std::string& text, std::size_t maxChars) const;
    long long computeComboRevenue(const std::string& comboList) const;
    long long computeTicketRevenue(const Ticket& ticket, int seatCount, long long comboRevenue) const;

    void setChartMode(ChartMode mode);
    void sortStatsForCurrentMode();
    void sortComboStats();
    void drawChart(sf::RenderTarget& target) const;

public:
    MovieRevenuePanel(sf::Font& font, float width, float height);

    void setPosition(const sf::Vector2f& pos);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::Vector2f& mousePos, bool mouseDown);
    void render(sf::RenderTarget& target) const;
    void refresh();
};
