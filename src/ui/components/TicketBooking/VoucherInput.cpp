#include "UI/components/TicketBooking/VoucherInput.h"
#include "UI/components/TicketBooking/OrderSummary.h"
#include "UI/screens/BaseScreen.h"
#include <algorithm>
#include <cctype>
#include <cmath>

VoucherInput::VoucherInput(Font& f)
    : font(f),
      bookingService(make_unique<BookingService>()),
      inputActive(false),
      voucherApplied(false),
      statusIsError(false),
      discountValue(0),
      subtotalSnapshot(0),
      caretVisible(true)
{
    caretClock.restart();
}

void VoucherInput::normalizeVoucherCode(string& code) {
    // Remove whitespace
    code.erase(remove_if(code.begin(), code.end(), [](unsigned char ch) { 
        return isspace(ch); 
    }), code.end());
    
    // Convert to uppercase
    for (auto& ch : code) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
}

void VoucherInput::clearVoucherPreview() {
    appliedVoucherCode.clear();
    voucherApplied = false;
    discountValue = 0;
    subtotalSnapshot = 0;
    statusMessage.clear();
    statusIsError = false;
}

FloatRect VoucherInput::getInputBounds() const {
    return FloatRect(
        Vector2f(OrderSummaryLayout::VoucherLabelX, OrderSummaryLayout::VoucherSectionY + 25.f),
        Vector2f(OrderSummaryLayout::VoucherInputWidth, OrderSummaryLayout::VoucherInputHeight)
    );
}

FloatRect VoucherInput::getButtonBounds() const {
    float x = OrderSummaryLayout::VoucherLabelX
            + OrderSummaryLayout::VoucherInputWidth + OrderSummaryLayout::VoucherButtonSpacing;
    float y = OrderSummaryLayout::VoucherSectionY + 25.f;
    return FloatRect(
        Vector2f(x, y),
        Vector2f(OrderSummaryLayout::VoucherButtonWidth, OrderSummaryLayout::VoucherButtonHeight)
    );
}

void VoucherInput::handleMouseClick(Vector2f mousePos, bool mousePressed, int currentSubtotal,
                                    const string& userEmail, bool isUserLoggedIn) {
    if (!mousePressed || loginRequiredPopup) return;
    
    FloatRect inputBounds = getInputBounds();
    FloatRect buttonBounds = getButtonBounds();
    
    // Click vào input box -> activate
    if (inputBounds.contains(mousePos)) {
        inputActive = true;
        return;
    }
    
    // Click vào nút Apply -> apply voucher
    if (buttonBounds.contains(mousePos)) {
        // Validate subtotal
        if (currentSubtotal <= 0) {
            statusMessage = "Bạn cần chọn ghế hoặc combo trước khi áp dụng.";
            statusIsError = true;
            return;
        }
        
        // Normalize code
        string code = inputText;
        normalizeVoucherCode(code);
        inputText = code;
        
        if (code.empty()) {
            statusMessage = "Vui lòng nhập mã voucher.";
            statusIsError = true;
            return;
        }
        
        // Check login - chỉ hiển thị thông báo, không tạo popup
        if (!isUserLoggedIn) {
            statusMessage = "Bạn cần đăng nhập để có thể sử dụng voucher.";
            statusIsError = true;
            return;
        }
        
        // Apply voucher
        clearVoucherPreview();
        double discount = bookingService->applyVoucher(userEmail, code, currentSubtotal, false);
        
        if (discount <= 0.0) {
            statusMessage = "Voucher không hợp lệ hoặc chưa đáp ứng điều kiện.";
            statusIsError = true;
            return;
        }
        
        voucherApplied = true;
        appliedVoucherCode = code;
        discountValue = static_cast<int>(round(discount));
        subtotalSnapshot = currentSubtotal;
        statusMessage = "Đã áp dụng mã " + code + ".";
        statusIsError = false;
        return;
    }
    
    // Click ra ngoài -> deactivate
    if (!inputBounds.contains(mousePos)) {
        inputActive = false;
    }
}

void VoucherInput::handleKeyboardInput(const Event& event, int currentSubtotal,
                                       const string& userEmail, bool isUserLoggedIn) {
    if (loginRequiredPopup) return;
    
    // Text input - cho phép nhập dù chưa đăng nhập
    if (inputActive && event.is<Event::TextEntered>()) {
        auto unicode = event.getIf<Event::TextEntered>()->unicode;
        if (unicode >= 32 && unicode < 128 && inputText.size() < kMaxVoucherLength) {
            char ch = static_cast<char>(unicode);
            if (isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_') {
                inputText.push_back(static_cast<char>(toupper(static_cast<unsigned char>(ch))));
                clearVoucherPreview();
            }
        }
    }
    
    // Key pressed
    if (inputActive && event.is<Event::KeyPressed>()) {
        auto key = event.getIf<Event::KeyPressed>()->code;
        
        if (key == Keyboard::Key::Backspace) {
            if (!inputText.empty()) {
                inputText.pop_back();
                clearVoucherPreview();
            }
        }
        else if (key == Keyboard::Key::Enter) {
            // Apply voucher khi nhấn Enter - sẽ validate login trong hàm apply
            handleMouseClick(getButtonBounds().position, true, currentSubtotal, userEmail, isUserLoggedIn);
        }
    }
}

void VoucherInput::update() {
    // Update caret blink
    if (caretClock.getElapsedTime().asSeconds() >= 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }
}

VoucherDisplayState VoucherInput::getDisplayState() const {
    VoucherDisplayState state;
    state.inputText = inputText;
    state.appliedCode = appliedVoucherCode;
    state.statusMessage = statusMessage;
    state.inputActive = inputActive;
    state.caretVisible = caretVisible;
    state.userLoggedIn = BaseScreen::isUserLoggedIn();
    state.statusIsError = statusIsError;
    return state;
}

void VoucherInput::reset() {
    inputText.clear();
    inputActive = false;
    caretVisible = true;
    caretClock.restart();
    clearVoucherPreview();
    loginRequiredPopup.reset();
}

bool VoucherInput::refreshIfNeeded(int newSubtotal, const string& userEmail) {
    if (!voucherApplied) return true;
    
    // Nếu subtotal thay đổi, refresh voucher
    if (newSubtotal != subtotalSnapshot) {
        if (userEmail.empty()) {
            statusMessage = "Bạn cần đăng nhập để có thể sử dụng voucher.";
            statusIsError = true;
            clearVoucherPreview();
            return false;
        }
        
        double refreshed = bookingService->applyVoucher(userEmail, appliedVoucherCode, newSubtotal, false);
        if (refreshed <= 0.0) {
            statusMessage = "Voucher không còn hợp lệ cho đơn hàng hiện tại.";
            statusIsError = true;
            clearVoucherPreview();
            return false;
        }
        
        discountValue = static_cast<int>(round(refreshed));
        subtotalSnapshot = newSubtotal;
    }
    
    return true;
}
