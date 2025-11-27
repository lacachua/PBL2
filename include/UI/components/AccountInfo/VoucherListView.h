#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "services/VoucherService.h"

class VoucherListView {
public:
    explicit VoucherListView(sf::Font& font);

    void setUser(const std::string& email);
    void refresh();
    void update(sf::Vector2f mousePos, bool mousePressed,
                sf::Vector2f cardPos, sf::Vector2f cardSize);
    void draw(sf::RenderWindow& window, sf::Vector2f cardPos, sf::Vector2f cardSize);

private:
    sf::Font& font;
    std::shared_ptr<VoucherService> voucherService;
    std::string currentUser;
    std::vector<VoucherDisplay> vouchers;

    // UI Components
    sf::Text titleText;
    sf::RectangleShape scrollableArea;
    
    // Pagination
    int currentPage = 0;
    int itemsPerPage = 5;
    sf::RectangleShape prevButton, nextButton;
    sf::Text prevButtonText, nextButtonText, pageInfoText;

    std::string formatValue(const VoucherDisplay& voucher) const;
    std::string formatDate(const std::string& raw) const;
    std::string formatCurrency(double amount) const;
    std::string formatMinBill(const VoucherDisplay& voucher) const;
    int getTotalPages() const;
    std::vector<VoucherDisplay> getCurrentPageVouchers() const;
};
