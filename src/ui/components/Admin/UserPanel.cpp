#include "UI/components/Admin/UserPanel.h"
#include "UI/components/Admin/RoundedRectRenderer.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>

using sf::Event;
using sf::FloatRect;
using sf::RenderWindow;
using sf::Vector2f;

namespace {
    constexpr float BUTTON_SPACING = 16.f;
    constexpr float BUTTON_RADIUS = 6.f;
}

UserPanel::UserPanel(Font& font, float width, float height)
    : font(font), width(width), height(height) {
    repository = make_unique<UserRepository>();
    setupUI();
    refreshData();
}

void UserPanel::setupUI() {
    background.setSize(Vector2f(width, height));
    background.setFillColor(bgColor);

    const std::string titleUtf8 = "Quản lý khách hàng";
    titleText = make_unique<Text>(font, sf::String::fromUtf8(titleUtf8.begin(), titleUtf8.end()), 26);
    titleText->setFillColor(Color(27, 38, 59));
    titleText->setStyle(sf::Text::Bold);

    tableBodyBg.setSize(Vector2f(TABLE_WIDTH, TABLE_HEIGHT));
    tableBodyBg.setFillColor(Color::White);
    tableBodyBg.setOutlineColor(borderColor);
    tableBodyBg.setOutlineThickness(1.f);

    tableHeaderBg.setSize(Vector2f(TABLE_WIDTH, HEADER_HEIGHT));
    tableHeaderBg.setFillColor(headerColor);

    setupButton(btnViewInfo, "Xem thông tin", Color(20, 118, 172), Color(34, 156, 218), {172.f, 44.f});
    setupButton(btnLock, "Khóa tài khoản", Color(233, 164, 0), Color(247, 186, 40), {186.f, 44.f});
    setupButton(btnDelete, "Xóa tài khoản", Color(211, 47, 47), Color(226, 83, 83), {176.f, 44.f});
    setupButton(btnRefresh, "", Color(27, 38, 59), Color(40, 55, 79), {48.f, 48.f});

    if (!reloadTexture.loadFromFile("../assets/elements/reload.png")) {
        std::cerr << "[UserPanel] Warning: Failed to load reload icon.\n";
        const std::string fallback = "Refresh";
        btnRefresh.label = make_unique<Text>(font, sf::String::fromUtf8(fallback.begin(), fallback.end()), 14);
        btnRefresh.label->setFillColor(Color::White);
    } else {
        reloadTexture.setSmooth(true);
        reloadSprite.emplace(reloadTexture);
    }

    notificationBg.setSize(Vector2f(320.f, 52.f));
    notificationBg.setFillColor(notificationColor);
    notificationBg.setOutlineThickness(0.f);

    notificationText = make_unique<Text>(font, "", 16);
    notificationText->setFillColor(Color::White);

    layoutElements();
}

void UserPanel::setupButton(ActionButton& button, const std::string& labelUtf8, const Color& base, const Color& hover, Vector2f size) {
    button.box.setSize(size);
    button.box.setFillColor(base);
    button.box.setOutlineThickness(0.f);
    button.baseColor = base;
    button.hoverColor = hover;

    if (!labelUtf8.empty()) {
        button.label = make_unique<Text>(font, sf::String::fromUtf8(labelUtf8.begin(), labelUtf8.end()), 16);
        button.label->setFillColor(Color::White);
    } else {
        button.label.reset();
    }
}

void UserPanel::layoutElements() {
    background.setPosition(position);

    if (titleText) {
        titleText->setPosition(Vector2f(position.x + 40.f, position.y + 20.f));
    }

    const float buttonRowY = position.y + 88.f;
    float nextX = position.x + TABLE_X;

    btnViewInfo.box.setPosition(Vector2f(nextX, buttonRowY));
    nextX += btnViewInfo.box.getSize().x + BUTTON_SPACING;

    btnLock.box.setPosition(Vector2f(nextX, buttonRowY));
    nextX += btnLock.box.getSize().x + BUTTON_SPACING;

    btnDelete.box.setPosition(Vector2f(nextX, buttonRowY));

    const float refreshX = position.x + TABLE_X + TABLE_WIDTH - btnRefresh.box.getSize().x;
    btnRefresh.box.setPosition(Vector2f(refreshX, buttonRowY - 4.f));

    auto centerLabel = [](ActionButton& btn) {
        if (!btn.label) return;
        const FloatRect bounds = btn.label->getLocalBounds();
        btn.label->setPosition(Vector2f(
            btn.box.getPosition().x + (btn.box.getSize().x - bounds.size.x) / 2.f - bounds.position.x,
            btn.box.getPosition().y + (btn.box.getSize().y - bounds.size.y) / 2.f - bounds.position.y
        ));
    };

    centerLabel(btnViewInfo);
    centerLabel(btnLock);
    centerLabel(btnDelete);
    centerLabel(btnRefresh);

    if (reloadSprite) {
        const FloatRect bounds = reloadSprite->getLocalBounds();
        const float maxDim = std::max(bounds.size.x, bounds.size.y);
        const float available = btnRefresh.box.getSize().x - 16.f;
        const float scale = (maxDim > 0.f) ? (available / maxDim) : 1.f;
        reloadSprite->setScale(Vector2f(scale, scale));
        reloadSprite->setPosition(Vector2f(
            btnRefresh.box.getPosition().x + (btnRefresh.box.getSize().x - bounds.size.x * scale) / 2.f,
            btnRefresh.box.getPosition().y + (btnRefresh.box.getSize().y - bounds.size.y * scale) / 2.f
        ));
        reloadSprite->setColor(Color::White);
    }

    tableBodyBg.setPosition(Vector2f(position.x + TABLE_X, position.y + TABLE_Y));
    tableHeaderBg.setPosition(tableBodyBg.getPosition());
}

void UserPanel::setPosition(Vector2f pos) {
    position = pos;
    layoutElements();
}

void UserPanel::refreshData() {
    repository->loadFromFile();
    userList = repository->getAllUsers();
    selectedRow = -1;
    hoveredRow = -1;
    scrollOffset = 0;
}

void UserPanel::updateButton(ActionButton& button, Vector2f mousePos) {
    button.hovered = button.box.getGlobalBounds().contains(mousePos);
    button.box.setFillColor(button.hovered ? button.hoverColor : button.baseColor);
}

string UserPanel::formatDate(time_t timestamp) const {
    if (timestamp <= 0) return "-";

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &timestamp);
#else
    localtime_r(&timestamp, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d/%m/%Y");
    return oss.str();
}

void UserPanel::renderTable(RenderWindow& window) {
    window.draw(tableBodyBg);
    window.draw(tableHeaderBg);

    static const std::array<std::string, 6> headers = {
        "STT", "Email", "Họ và tên", "Số điện thoại", "Ngày đăng ký", "Trạng thái"
    };
    static const std::array<float, 6> columnWidths = {
        72.f, 260.f, 228.f, 168.f, 220.f, 204.f
    };

    std::array<float, 6> columnLefts{};
    columnLefts[0] = tableHeaderBg.getPosition().x;
    for (size_t i = 1; i < columnLefts.size(); ++i) {
        columnLefts[i] = columnLefts[i - 1] + columnWidths[i - 1];
    }

    for (size_t i = 0; i < headers.size(); ++i) {
        Text headerText(font, sf::String::fromUtf8(headers[i].begin(), headers[i].end()), 16);
        headerText.setFillColor(Color::White);
        headerText.setStyle(sf::Text::Bold);

        const FloatRect bounds = headerText.getLocalBounds();
        headerText.setPosition(Vector2f(
            columnLefts[i] + (columnWidths[i] - bounds.size.x) / 2.f - bounds.position.x,
            tableHeaderBg.getPosition().y + (HEADER_HEIGHT - bounds.size.y) / 2.f - bounds.position.y
        ));
        window.draw(headerText);
    }

    RectangleShape separator(Vector2f(1.f, TABLE_HEIGHT - 8.f));
    separator.setFillColor(borderColor);
    for (size_t i = 1; i < headers.size(); ++i) {
        separator.setPosition(Vector2f(columnLefts[i], tableBodyBg.getPosition().y + 8.f));
        window.draw(separator);
    }

    const float rowsStartY = tableHeaderBg.getPosition().y + HEADER_HEIGHT;
    const int maxRows = static_cast<int>((TABLE_HEIGHT - HEADER_HEIGHT) / ROW_HEIGHT);
    const int maxScroll = std::max(0, static_cast<int>(userList.size()) - maxRows);
    if (scrollOffset > maxScroll) {
        scrollOffset = maxScroll;
    }

    const int endIndex = std::min<int>(static_cast<int>(userList.size()), scrollOffset + maxRows);
    for (int idx = scrollOffset; idx < endIndex; ++idx) {
        const float rowY = rowsStartY + static_cast<float>(idx - scrollOffset) * ROW_HEIGHT;
        RectangleShape rowBg(Vector2f(TABLE_WIDTH, ROW_HEIGHT));
        rowBg.setPosition(Vector2f(tableHeaderBg.getPosition().x, rowY));

        if (idx == selectedRow) {
            rowBg.setFillColor(selectedColor);
        } else if (idx == hoveredRow) {
            rowBg.setFillColor(hoverColor);
        } else {
            rowBg.setFillColor(idx % 2 == 0 ? Color::White : Color(248, 250, 252));
        }
        window.draw(rowBg);

        RectangleShape rowBottom(Vector2f(TABLE_WIDTH, 1.f));
        rowBottom.setPosition(Vector2f(tableHeaderBg.getPosition().x, rowY + ROW_HEIGHT));
        rowBottom.setFillColor(borderColor);
        window.draw(rowBottom);

        const User& user = userList[idx];
        std::array<std::string, 6> values = {
            std::to_string(idx + 1),
            user.getEmail(),
            user.getFullName(),
            user.getPhone(),
            formatDate(user.getRegisteredAt()),
            user.isLocked() ? "Khóa" : "Hoạt động"
        };

        for (size_t col = 0; col < values.size(); ++col) {
            Color cellColor = textColor;
            if (col == values.size() - 1) {
                cellColor = user.isLocked() ? Color(220, 53, 69) : Color(40, 167, 69);
            }

            Text cell(font, sf::String::fromUtf8(values[col].begin(), values[col].end()), 14);
            cell.setFillColor(cellColor);

            auto textBounds = cell.getLocalBounds();
            float textX = columnLefts[col] + 14.f - textBounds.position.x;
            if (textBounds.size.x > columnWidths[col] - 28.f) {
                cell.setCharacterSize(13);
                textBounds = cell.getLocalBounds();
            }

            cell.setPosition(Vector2f(
                textX,
                rowY + (ROW_HEIGHT - textBounds.size.y) / 2.f - textBounds.position.y
            ));
            window.draw(cell);
        }
    }
}

void UserPanel::renderButtons(RenderWindow& window) {
    auto drawButton = [&](ActionButton& button) {
        RoundedRectRenderer::draw(window, button.box.getPosition(), button.box.getSize(), BUTTON_RADIUS, button.box.getFillColor());
        if (button.label) {
            window.draw(*button.label);
        }
    };

    drawButton(btnViewInfo);
    drawButton(btnLock);
    drawButton(btnDelete);

    RoundedRectRenderer::draw(window, btnRefresh.box.getPosition(), btnRefresh.box.getSize(), BUTTON_RADIUS, btnRefresh.box.getFillColor());
    if (reloadSprite) {
        window.draw(*reloadSprite);
    } else if (btnRefresh.label) {
        window.draw(*btnRefresh.label);
    }
}

void UserPanel::renderNotification(RenderWindow& window) {
    if (!notificationVisible) return;

    if (notificationClock.getElapsedTime().asSeconds() > 3.0f) {
        notificationVisible = false;
        return;
    }

    const Vector2f size = notificationBg.getSize();
    Vector2f pos(window.getSize().x - size.x - 32.f, position.y + 24.f);
    notificationBg.setPosition(pos);
    notificationBg.setFillColor(notificationColor);

    window.draw(notificationBg);

    if (notificationText) {
        const FloatRect bounds = notificationText->getLocalBounds();
        notificationText->setPosition(Vector2f(
            pos.x + (size.x - bounds.size.x) / 2.f - bounds.position.x,
            pos.y + (size.y - bounds.size.y) / 2.f - bounds.position.y
        ));
        window.draw(*notificationText);
    }
}

void UserPanel::showNotification(const string& message, const Color& color) {
    notificationMessage = message;
    notificationColor = color;
    notificationVisible = true;
    notificationClock.restart();

    if (notificationText) {
        notificationText->setString(sf::String::fromUtf8(message.begin(), message.end()));
    }
}

void UserPanel::handleEvent(const Event& event, const RenderWindow& window) {
    if (const auto* scroll = event.getIf<Event::MouseWheelScrolled>()) {
        if (scroll->wheel == sf::Mouse::Wheel::Vertical) {
            const int maxRows = static_cast<int>((TABLE_HEIGHT - HEADER_HEIGHT) / ROW_HEIGHT);
            const int maxScroll = std::max(0, static_cast<int>(userList.size()) - maxRows);
            if (scroll->delta > 0) {
                scrollOffset = std::max(0, scrollOffset - 1);
            } else {
                scrollOffset = std::min(maxScroll, scrollOffset + 1);
            }
        }
        return;
    }

    if (const auto* click = event.getIf<Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            Vector2f mousePos(static_cast<float>(click->position.x), static_cast<float>(click->position.y));

            if (btnViewInfo.box.getGlobalBounds().contains(mousePos)) {
                if (selectedRow >= 0 && selectedRow < static_cast<int>(userList.size())) {
                    showNotification("Tính năng xem chi tiết sẽ sớm khả dụng.", Color(20, 118, 172));
                } else {
                    showNotification("Vui lòng chọn một khách hàng trước.", Color(211, 47, 47));
                }
                return;
            }

            if (btnLock.box.getGlobalBounds().contains(mousePos)) {
                if (selectedRow >= 0 && selectedRow < static_cast<int>(userList.size())) {
                    showNotification("Tính năng khóa/mở khóa đang được phát triển.", Color(233, 164, 0));
                } else {
                    showNotification("Chưa chọn khách hàng nào.", Color(211, 47, 47));
                }
                return;
            }

            if (btnDelete.box.getGlobalBounds().contains(mousePos)) {
                if (selectedRow >= 0 && selectedRow < static_cast<int>(userList.size())) {
                    showNotification("Chức năng xóa tài khoản sẽ được bổ sung sau.", Color(211, 47, 47));
                } else {
                    showNotification("Chưa chọn khách hàng nào.", Color(211, 47, 47));
                }
                return;
            }

            if (btnRefresh.box.getGlobalBounds().contains(mousePos)) {
                refreshData();
                showNotification("Đã làm mới danh sách khách hàng.", Color(33, 150, 83));
                return;
            }

            const FloatRect tableBounds(tableBodyBg.getPosition(), tableBodyBg.getSize());
            if (tableBounds.contains(mousePos)) {
                const float localY = mousePos.y - (tableHeaderBg.getPosition().y + HEADER_HEIGHT);
                if (localY >= 0.f) {
                    const int rowIndex = static_cast<int>(localY / ROW_HEIGHT) + scrollOffset;
                    if (rowIndex >= 0 && rowIndex < static_cast<int>(userList.size())) {
                        selectedRow = rowIndex;
                    }
                }
            }
        }
    }
}

void UserPanel::update(Vector2f mousePos, bool mousePressed) {
    (void)mousePressed;
    updateButton(btnViewInfo, mousePos);
    updateButton(btnLock, mousePos);
    updateButton(btnDelete, mousePos);
    updateButton(btnRefresh, mousePos);

    hoveredRow = -1;
    const FloatRect tableBounds(Vector2f(tableBodyBg.getPosition().x, tableBodyBg.getPosition().y + HEADER_HEIGHT),
                                Vector2f(tableBodyBg.getSize().x, tableBodyBg.getSize().y - HEADER_HEIGHT));
    if (tableBounds.contains(mousePos)) {
        const float localY = mousePos.y - tableBounds.position.y;
        const int rowIndex = static_cast<int>(localY / ROW_HEIGHT) + scrollOffset;
        if (rowIndex >= 0 && rowIndex < static_cast<int>(userList.size())) {
            hoveredRow = rowIndex;
        }
    }
}

void UserPanel::render(RenderWindow& window) {
    window.draw(background);
    if (titleText) {
        window.draw(*titleText);
    }

    renderButtons(window);
    renderTable(window);
    renderNotification(window);
}
