#include "UI/components/AccountInfo/VoucherListView.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

using sf::Color;
using sf::FloatRect;
using sf::RectangleShape;
using sf::RenderWindow;
using sf::Text;
using sf::Vector2f;
using sf::String;
using std::string;
using std::vector;

namespace {
	inline sf::String toUtf8(const std::string& text) {
		return sf::String::fromUtf8(text.begin(), text.end());
	}
}

VoucherListView::VoucherListView(sf::Font& fontRef)
	: font(fontRef),
	  voucherService(std::make_shared<VoucherService>()),
	  titleText(fontRef, toUtf8("VOUCHER CỦA TÔI"), 22),
	  prevButtonText(fontRef, toUtf8("← Trước"), 16),
	  nextButtonText(fontRef, toUtf8("Tiếp →"), 16),
	  pageInfoText(fontRef, toUtf8(""), 16) {
	
	titleText.setFillColor(Color(238, 238, 238));
	titleText.setStyle(Text::Bold);
	
	scrollableArea.setFillColor(Color(15, 30, 50, 200));
	scrollableArea.setOutlineColor(Color(50, 80, 120));
	scrollableArea.setOutlineThickness(1.f);
	
	// Setup pagination buttons
	prevButton.setSize({120.f, 40.f});
	nextButton.setSize({120.f, 40.f});
	prevButton.setFillColor(Color(0, 153, 255));
	nextButton.setFillColor(Color(0, 153, 255));
	
	prevButtonText.setFillColor(Color::White);
	nextButtonText.setFillColor(Color::White);
	pageInfoText.setFillColor(Color(200, 200, 200));
}

void VoucherListView::setUser(const string& email) {
	if (email == currentUser) return;
	currentUser = email;
	refresh();
}

void VoucherListView::refresh() {
	voucherService->cleanupExpiredVouchers();
	if (currentUser.empty()) {
		vouchers.clear();
	} else {
		vouchers = voucherService->getVouchersByUser(currentUser);
	}
	currentPage = 0;
}

int VoucherListView::getTotalPages() const {
	if (vouchers.empty()) return 1;
	return (vouchers.size() + itemsPerPage - 1) / itemsPerPage;
}

vector<VoucherDisplay> VoucherListView::getCurrentPageVouchers() const {
	vector<VoucherDisplay> pageVouchers;
	int startIdx = currentPage * itemsPerPage;
	int endIdx = std::min(startIdx + itemsPerPage, static_cast<int>(vouchers.size()));
	
	for (int i = startIdx; i < endIdx; i++) {
		pageVouchers.push_back(vouchers[i]);
	}
	
	return pageVouchers;
}

void VoucherListView::update(Vector2f mousePos, bool mousePressed, Vector2f cardPos, Vector2f /*cardSize*/) {
	// Update button positions
	float buttonY = cardPos.y + 650.f;
	prevButton.setPosition({cardPos.x + 26.f, buttonY});
	nextButton.setPosition({cardPos.x + 860.f, buttonY});
	
	prevButtonText.setPosition({cardPos.x + 45.f, buttonY + 8.f});
	nextButtonText.setPosition({cardPos.x + 885.f, buttonY + 8.f});
	
	// Page info
	int totalPages = getTotalPages();
	currentPage = std::clamp(currentPage, 0, std::max(0, totalPages - 1));
	string pageInfo = "Trang " + std::to_string(currentPage + 1) + " / " + std::to_string(totalPages);
	pageInfoText.setString(toUtf8(pageInfo));
	FloatRect textBounds = pageInfoText.getLocalBounds();
	pageInfoText.setPosition({cardPos.x + 505.f - textBounds.size.x / 2.f, buttonY + 10.f});
	
	// Handle button clicks
	if (mousePressed) {
		bool clickedPrev = prevButton.getGlobalBounds().contains(mousePos) && currentPage > 0;
		bool clickedNext = nextButton.getGlobalBounds().contains(mousePos) && currentPage < totalPages - 1;
		
		if (clickedPrev) currentPage--;
		else if (clickedNext) currentPage++;
	}
	
	// Button hover effects
	if (prevButton.getGlobalBounds().contains(mousePos) && currentPage > 0) {
		prevButton.setFillColor(Color(0, 191, 255));
	} else {
		prevButton.setFillColor(currentPage > 0 ? Color(0, 153, 255) : Color(100, 100, 100));
	}
	
	if (nextButton.getGlobalBounds().contains(mousePos) && currentPage < totalPages - 1) {
		nextButton.setFillColor(Color(0, 191, 255));
	} else {
		nextButton.setFillColor(currentPage < totalPages - 1 ? Color(0, 153, 255) : Color(100, 100, 100));
	}
}

void VoucherListView::draw(RenderWindow& window, Vector2f cardPos, Vector2f /*cardSize*/) {
	// Draw title
	titleText.setPosition({cardPos.x + 26.f, cardPos.y + 26.f});
	window.draw(titleText);
	
	// Draw scrollable area
	scrollableArea.setSize({958.f, 560.f});
	scrollableArea.setPosition({cardPos.x + 26.f, cardPos.y + 70.f});
	window.draw(scrollableArea);
	
	// Check login
	if (currentUser.empty()) {
		Text loginPrompt(font, toUtf8("Vui lòng đăng nhập để xem voucher."), 18);
		loginPrompt.setFillColor(Color(150, 150, 150));
		FloatRect bounds = loginPrompt.getLocalBounds();
		loginPrompt.setPosition({
			cardPos.x + 505.f - bounds.size.x / 2.f,
			cardPos.y + 350.f
		});
		window.draw(loginPrompt);
		return;
	}
	
	// Draw vouchers
	vector<VoucherDisplay> pageVouchers = getCurrentPageVouchers();
	
	if (pageVouchers.empty()) {
		Text noVoucherText(font, toUtf8("Bạn chưa có voucher nào."), 18);
		noVoucherText.setFillColor(Color(150, 150, 150));
		FloatRect bounds = noVoucherText.getLocalBounds();
		noVoucherText.setPosition({
			cardPos.x + 505.f - bounds.size.x / 2.f,
			cardPos.y + 350.f
		});
		window.draw(noVoucherText);
	} else {
		float startY = cardPos.y + 90.f;
		float itemHeight = 105.f;
		
		for (size_t i = 0; i < pageVouchers.size(); i++) {
			const VoucherDisplay& voucher = pageVouchers[i];
			float itemY = startY + i * itemHeight;
			
			// Draw voucher card
			RectangleShape voucherCard;
			voucherCard.setSize({918.f, 95.f});
			voucherCard.setPosition({cardPos.x + 46.f, itemY});
			voucherCard.setFillColor(Color(25, 45, 70, 255));
			voucherCard.setOutlineColor(Color(60, 100, 140));
			voucherCard.setOutlineThickness(1.f);
			window.draw(voucherCard);
			
			// Mã voucher (vị trí của Mã vé và tên phim)
			Text codeText(font, toUtf8(voucher.code), 18);
			codeText.setPosition({cardPos.x + 60.f, itemY + 10.f});
			codeText.setFillColor(Color(100, 200, 255));
			codeText.setStyle(Text::Bold);
			window.draw(codeText);
			
			// Mô tả voucher (vị trí của dòng ngày giờ chiếu)
			Text descText(font, toUtf8(voucher.description), 14);
			descText.setFillColor(Color(200, 200, 200));
			descText.setPosition({cardPos.x + 60.f, itemY + 38.f});
			window.draw(descText);
			
			// Chi tiết giảm giá và điều kiện (vị trí của dòng Ghế và Combo)
			string detailStr = formatValue(voucher) + " " + formatMinBill(voucher);
			Text detailText(font, toUtf8(detailStr), 14);
			detailText.setFillColor(Color(180, 220, 255));
			detailText.setPosition({cardPos.x + 60.f, itemY + 60.f});
			window.draw(detailText);
			
			// Số lượng còn lại (bên phải, vị trí của giá)
			string qtyStr = "Số lượng: " + std::to_string(voucher.quantity);
			Text qtyText(font, toUtf8(qtyStr), 16);
			qtyText.setFillColor(Color(100, 255, 100));
			qtyText.setStyle(Text::Bold);
			FloatRect qtyBounds = qtyText.getLocalBounds();
			qtyText.setPosition({cardPos.x + 940.f - qtyBounds.size.x, itemY + 10.f});
			window.draw(qtyText);
			
			// Hạn sử dụng (vị trí của "Đặt: ngày giờ")
			string expiryLabel = "HSD: " + formatDate(voucher.expiry);
			Text expiryText(font, toUtf8(expiryLabel), 12);
			expiryText.setFillColor(Color(120, 120, 120));
			FloatRect expiryBounds = expiryText.getLocalBounds();
			expiryText.setPosition({cardPos.x + 940.f - expiryBounds.size.x, itemY + 70.f});
			window.draw(expiryText);
		}
	}
	
	// Draw pagination buttons
	window.draw(prevButton);
	window.draw(nextButton);
	window.draw(prevButtonText);
	window.draw(nextButtonText);
	window.draw(pageInfoText);
}

string VoucherListView::formatValue(const VoucherDisplay& voucher) const {
	if (voucher.type == 1) {
		return "Giảm " + formatCurrency(voucher.value);
	}
	std::ostringstream ss;
	ss << "Giảm " << std::fixed << std::setprecision(0) << voucher.value << "%";
	return ss.str();
}

string VoucherListView::formatMinBill(const VoucherDisplay& voucher) const {
	const VoucherDef* def = voucherService->getDefinition(voucher.code);
	if (!def) return "";
	
	if (def->minBill > 0) {
		return "cho đơn từ " + formatCurrency(def->minBill);
	}
	return "cho mọi đơn hàng";
}

string VoucherListView::formatDate(const string& raw) const {
	if (raw.size() != 8) return raw;
	return raw.substr(6, 2) + "/" + raw.substr(4, 2) + "/" + raw.substr(0, 4);
}

string VoucherListView::formatCurrency(double amount) const {
	long long value = static_cast<long long>(std::round(amount));
	string digits = std::to_string(std::abs(value));
	string formatted;
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
