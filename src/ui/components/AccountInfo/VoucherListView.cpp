#include "UI/components/AccountInfo/VoucherListView.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

using sf::Color;
using sf::Event;
using sf::FloatRect;
using sf::RectangleShape;
using sf::RenderWindow;
using sf::Text;
using sf::Vector2f;

namespace {
	constexpr float kContentPadding = 36.f;
	constexpr float kListTopOffset = 120.f;
	constexpr float kCardHeight = 120.f;
	constexpr float kCardSpacing = 18.f;
	constexpr float kScrollStep = 40.f;

	inline sf::String toUtf8(const std::string& text) {
		return sf::String::fromUtf8(text.begin(), text.end());
	}
}

VoucherListView::VoucherListView(sf::Font& fontRef)
	: font(fontRef) {
}

void VoucherListView::setUser(const std::string& email) {
	if (email == currentUser) return;
	currentUser = email;
	refresh();
}

void VoucherListView::refresh() {
	if (currentUser.empty()) {
		vouchers.clear();
	} else {
		vouchers = manager.getVouchersByUser(currentUser);
	}
	scrollOffset = 0.f;
	clampScroll();
}

void VoucherListView::update(Vector2f mousePos, bool /*mousePressed*/, const Event* event,
							 Vector2f cardPos, Vector2f cardSize) {
	listArea = FloatRect(Vector2f(cardPos.x + kContentPadding,
								  cardPos.y + kListTopOffset),
						 Vector2f(std::max(0.f, cardSize.x - kContentPadding * 2.f),
								  std::max(0.f, cardSize.y - (kListTopOffset + 40.f))));

	if (!event) return;
	if (const auto* wheel = event->getIf<Event::MouseWheelScrolled>()) {
		if (listArea.contains(mousePos)) {
			scrollOffset -= wheel->delta * kScrollStep;
			clampScroll();
		}
	}
}

void VoucherListView::draw(RenderWindow& window, Vector2f cardPos, Vector2f cardSize) {
	Text heading(font, L"Voucher của tôi", 28);
	heading.setFillColor(Color::White);
	heading.setPosition(cardPos + Vector2f(kContentPadding, 36.f));
	window.draw(heading);

	if (currentUser.empty()) {
		RectangleShape emptyState(Vector2f(cardSize.x - kContentPadding * 2.f, cardSize.y - 200.f));
		emptyState.setPosition(cardPos + Vector2f(kContentPadding, 110.f));
		emptyState.setFillColor(Color(18, 42, 78, 230));
		emptyState.setOutlineThickness(1.f);
		emptyState.setOutlineColor(Color(40, 80, 140));
		window.draw(emptyState);

		Text emptyText(font, L"Vui lòng đăng nhập để xem voucher.", 20);
		emptyText.setFillColor(Color(190, 205, 230));
		emptyText.setPosition(emptyState.getPosition() + Vector2f(24.f, 48.f));
		window.draw(emptyText);

		Text hint(font, L"Nhấn Đăng nhập ở góc trên bên phải để tiếp tục.", 16);
		hint.setFillColor(Color(150, 170, 205));
		hint.setPosition(emptyState.getPosition() + Vector2f(24.f, 92.f));
		window.draw(hint);
		return;
	}

	int activeCount = static_cast<int>(std::count_if(vouchers.begin(), vouchers.end(), [](const VoucherDisplay& v) {
		return v.status == 1;
	}));

	RectangleShape summary({240.f, 70.f});
	summary.setPosition(cardPos + Vector2f(kContentPadding, 78.f));
	summary.setFillColor(Color(32, 70, 128, 235));
	summary.setOutlineThickness(1.f);
	summary.setOutlineColor(Color(60, 120, 200));
	window.draw(summary);

	Text summaryTitle(font, L"Đang khả dụng", 16);
	summaryTitle.setFillColor(Color(190, 210, 240));
	summaryTitle.setPosition(summary.getPosition() + Vector2f(18.f, 10.f));
	window.draw(summaryTitle);

	Text summaryValue(font, std::to_string(activeCount), 30);
	summaryValue.setFillColor(Color::White);
	summaryValue.setPosition(summary.getPosition() + Vector2f(18.f, 30.f));
	window.draw(summaryValue);

	Vector2f listPos = listArea.position;
	float listBottom = listArea.position.y + listArea.size.y;

	if (vouchers.empty()) {
		RectangleShape emptyState(Vector2f(listArea.size.x, 200.f));
		emptyState.setPosition(listPos);
		emptyState.setFillColor(Color(18, 42, 78, 230));
		emptyState.setOutlineThickness(1.f);
		emptyState.setOutlineColor(Color(40, 80, 140));
		window.draw(emptyState);

		Text emptyText(font, L"Bạn chưa có voucher nào.", 20);
		emptyText.setFillColor(Color(190, 205, 230));
		emptyText.setPosition(emptyState.getPosition() + Vector2f(24.f, 48.f));
		window.draw(emptyText);

		Text hint(font, L"Nhận voucher bằng cách đăng ký hoặc tham gia khuyến mãi.", 16);
		hint.setFillColor(Color(150, 170, 205));
		hint.setPosition(emptyState.getPosition() + Vector2f(24.f, 92.f));
		window.draw(hint);
		return;
	}

	float contentHeight = static_cast<float>(vouchers.size()) * (kCardHeight + kCardSpacing) - kCardSpacing;
	maxScroll = std::max(0.f, contentHeight - listArea.size.y);
	clampScroll();

	float startY = listPos.y - scrollOffset;
	for (size_t i = 0; i < vouchers.size(); ++i) {
		float cardY = startY + static_cast<float>(i) * (kCardHeight + kCardSpacing);
		if (cardY + kCardHeight < listArea.position.y - 5.f) continue;
		if (cardY > listBottom + 5.f) break;

		const auto& voucher = vouchers[i];

		RectangleShape card(Vector2f(listArea.size.x, kCardHeight));
		card.setPosition({listPos.x, cardY});
		card.setFillColor(Color(18, 48, 86, 240));
		card.setOutlineThickness(1.f);
		card.setOutlineColor(Color(42, 88, 150));
		window.draw(card);

		RectangleShape accent({6.f, kCardHeight});
		accent.setPosition(card.getPosition());
		accent.setFillColor(voucher.status == 1 ? Color(68, 214, 159) : Color(128, 135, 150));
		window.draw(accent);

		Text codeText(font, voucher.code, 20);
		codeText.setFillColor(Color::White);
		codeText.setPosition(card.getPosition() + Vector2f(20.f, 14.f));
		window.draw(codeText);

		Text descText(font, toUtf8(voucher.description), 16);
		descText.setFillColor(Color(195, 210, 235));
		descText.setPosition(card.getPosition() + Vector2f(20.f, 46.f));
		window.draw(descText);

		Text valueText(font, toUtf8(formatValue(voucher)), 24);
		valueText.setFillColor(Color(255, 214, 94));
		valueText.setPosition(card.getPosition() + Vector2f(listArea.size.x - 260.f, 18.f));
		window.draw(valueText);

		std::string expiryLabel = "HSD: " + formatDate(voucher.expiry);
		Text expiryText(font, toUtf8(expiryLabel), 16);
		expiryText.setFillColor(Color(150, 175, 210));
		expiryText.setPosition(card.getPosition() + Vector2f(20.f, kCardHeight - 34.f));
		window.draw(expiryText);

		RectangleShape statusPill(Vector2f(150.f, 34.f));
		statusPill.setPosition(card.getPosition() + Vector2f(listArea.size.x - 190.f, kCardHeight - 44.f));
		statusPill.setFillColor(statusColor(voucher.status));
		window.draw(statusPill);

		Text statusText(font, voucher.status == 1 ? L"Sẵn sàng" : L"Đã khóa", 16);
		statusText.setFillColor(Color::Black);
		statusText.setPosition(statusPill.getPosition() + Vector2f(18.f, 6.f));
		window.draw(statusText);
	}
}

std::string VoucherListView::formatValue(const VoucherDisplay& voucher) const {
	if (voucher.type == 1) {
		return formatCurrency(voucher.value);
	}
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(0) << voucher.value << "%";
	return ss.str();
}

std::string VoucherListView::formatDate(const std::string& raw) const {
	if (raw.size() != 8) return raw;
	return raw.substr(6, 2) + "/" + raw.substr(4, 2) + "/" + raw.substr(0, 4);
}

std::string VoucherListView::formatCurrency(double amount) const {
	long long value = static_cast<long long>(std::round(amount));
	std::string digits = std::to_string(std::abs(value));
	std::string formatted;
	for (size_t i = 0; i < digits.size(); ++i) {
		if (i > 0 && (digits.size() - i) % 3 == 0) {
			formatted.push_back('.');
		}
		formatted.push_back(digits[i]);
	}
	if (value < 0) formatted.insert(formatted.begin(), '-');
	formatted += " đ";
	return formatted;
}

Color VoucherListView::statusColor(int status) const {
	return status == 1 ? Color(104, 241, 198) : Color(130, 135, 145);
}

void VoucherListView::clampScroll() {
	if (scrollOffset < 0.f) scrollOffset = 0.f;
	if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}
