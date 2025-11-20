#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "data-structures/BTree.h"
#include "data-structures/DLL.h"
#include "UI/components/Admin/AdminTicketRepository.h"
#include "UI/components/Admin/DropdownBox.h"
#include "UI/components/Button.h"
#include "UI/components/TicketBooking/ShowtimeRepository.h"
#include "UI/components/TicketBooking/ComboRepository.h"

enum class StatsMode {
    Revenue,
    Tickets
};

struct MovieStats {
    string title;
    long long revenue;
    int ticketCount;
};

class RevenuePanel {
private:
    Font& font;
    float width;
    float height;
    Vector2f position;
    StatsMode mode;

    // Data
    BTree<long long, Ticket> ticketTree;
    DLL<Ticket> filteredTickets;
    DLL<long long> availableKeys;
    
    // Date selection data
    DLL<string> days;
    DLL<string> months;
    DLL<string> years;

    DLL<MovieStats> movieStats; // Added this

    long long startKey = 0;
    long long endKey = 99999999;

    // Paths
    string ticketsFilePath;
    string showtimesFilePath;
    string combosFilePath;
    std::unordered_map<std::string, int> showtimeSeatPrices;
    std::unordered_map<std::string, int> comboPrices;

    // UI Elements
    RectangleShape background;
    Text titleText;
    Text fromLabelText;
    Text toLabelText;

    // Date Selection UI
    unique_ptr<DropdownBox> fromDayDropdown;
    unique_ptr<DropdownBox> fromMonthDropdown;
    unique_ptr<DropdownBox> fromYearDropdown;
    
    unique_ptr<DropdownBox> toDayDropdown;
    unique_ptr<DropdownBox> toMonthDropdown;
    unique_ptr<DropdownBox> toYearDropdown;

    unique_ptr<Button> applyButton;

    // Cards
    RectangleShape chartCard;
    RectangleShape tableCard;
    RectangleShape tableHeader;

    // Colors
    Color accentColor = Color(52, 152, 219); // Blue
    Color secondaryColor = Color(231, 76, 60); // Red/Pink
    Color cardColor = Color(255, 255, 255);

    // Helpers
    string resolveDataPath(const string& relative) const;
    void initializeUI();
    void loadData();
    void loadShowtimePrices();
    void loadComboPrices();
    void populateDateDropdowns();
    void applySelection();
    long long dateToKey(const string& date) const;
    string keyToDisplay(long long key) const;
    string formatCurrency(long long amount) const;
    String makeUtf8(const string& text) const;
    string ellipsize(const string& text, size_t maxChars) const;
    string trim(const string& text) const;
    int countSeats(const string& seatList) const;
    int parseComboQuantity(const string& item) const;
    long long computeComboCost(const string& comboList) const;
    long long computeTicketRevenue(const Ticket& ticket) const;

    void updateSummary();
    void sortMovieStats();
    void drawChart(RenderWindow& window);
    void drawTable(RenderWindow& window);

public:
    RevenuePanel(Font& font, float width, float height, StatsMode mode = StatsMode::Revenue);

    void setPosition(Vector2f pos);
    void handleEvent(const Event& event, const RenderWindow& window);
    void update(Vector2f mousePos, bool mousePressed);
    void render(RenderWindow& window);
    void refreshData();
};
