#include "UI/components/Admin/StatCard.h"

StatCard::StatCard(sf::Font& fontRef, const sf::Vector2f& size, const sf::Color& outlineColor)
    : font(fontRef),
      box(size),
      titleText(font, sf::String(), 14),
      valueText(font, sf::String(), 28),
      subtitleText(font, sf::String(), 12) {
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2.f);
    box.setOutlineColor(outlineColor);

    titleText.setFillColor(sf::Color(71, 85, 105));
    valueText.setFillColor(sf::Color(15, 23, 42));
    valueText.setStyle(sf::Text::Bold);
    subtitleText.setFillColor(sf::Color(100, 116, 139));
}

void StatCard::setPosition(const sf::Vector2f& position) {
    box.setPosition(position);

    titleText.setPosition(sf::Vector2f(position.x + 16.f, position.y + 14.f));
    subtitleText.setPosition(sf::Vector2f(position.x + 16.f, position.y + box.getSize().y - 28.f));
    centerValue();
}

void StatCard::setSize(const sf::Vector2f& size) {
    box.setSize(size);
    centerValue();
}

void StatCard::setOutlineThickness(float thickness) {
    box.setOutlineThickness(thickness);
}

void StatCard::setOutlineColor(const Color& color) {
    box.setOutlineColor(color);
}

void StatCard::setTitle(const std::string& title) {
    titleText.setString(sf::String::fromUtf8(title.begin(), title.end()));
}

void StatCard::setValue(const std::string& value) {
    valueText.setString(sf::String::fromUtf8(value.begin(), value.end()));
    centerValue();
}

void StatCard::setSubtitle(const std::string& subtitle) {
    subtitleText.setString(sf::String::fromUtf8(subtitle.begin(), subtitle.end()));
}

void StatCard::centerValue() {
    sf::FloatRect bounds = valueText.getLocalBounds();
    sf::Vector2f pos = box.getPosition();
    sf::Vector2f size = box.getSize();

    valueText.setPosition(sf::Vector2f(
        pos.x + (size.x - bounds.size.x) / 2.f - bounds.position.x,
        pos.y + (size.y - bounds.size.y) / 2.f - bounds.position.y
    ));
}

void StatCard::render(sf::RenderTarget& target) const {
    target.draw(box);
    target.draw(titleText);
    target.draw(valueText);
    target.draw(subtitleText);
}
