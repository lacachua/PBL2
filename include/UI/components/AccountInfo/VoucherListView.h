#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "services/VoucherManager.h"

class VoucherListView {
public:
    explicit VoucherListView(sf::Font& font);

    void setUser(const std::string& email);
    void refresh();
    void update(sf::Vector2f mousePos, bool mousePressed, const sf::Event* event,
                sf::Vector2f cardPos, sf::Vector2f cardSize);
    void draw(sf::RenderWindow& window, sf::Vector2f cardPos, sf::Vector2f cardSize);

private:
    sf::Font& font;
    VoucherManager manager;
    std::string currentUser;
    std::vector<VoucherDisplay> vouchers;

    float scrollOffset = 0.f;
    float maxScroll = 0.f;
    sf::FloatRect listArea;

    std::string formatValue(const VoucherDisplay& voucher) const;
    std::string formatDate(const std::string& raw) const;
    std::string formatCurrency(double amount) const;
    sf::Color statusColor(int status) const;
    void clampScroll();
};
