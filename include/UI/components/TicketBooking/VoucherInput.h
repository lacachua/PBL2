#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "services/BookingService.h"
#include "UI/components/TicketBooking/LoginRequiredPopup.h"
#include "UI/components/TicketBooking/VoucherDisplayState.h"

using namespace sf;
using namespace std;

/**
 * @brief Component xử lý UI/UX cho voucher input trong booking flow
 * 
 * Chịu trách nhiệm:
 * - Vẽ input box và nút Apply
 * - Xử lý sự kiện nhập liệu (keyboard, mouse)
 * - Tương tác với BookingService để validate và apply voucher
 * - Hiển thị thông báo success/error
 * - Quản lý trạng thái (active, applied, error)
 */
class VoucherInput {
private:
    Font& font;
    unique_ptr<BookingService> bookingService;
    unique_ptr<LoginRequiredPopup> loginRequiredPopup;
    
    // Input state
    string inputText;
    string appliedVoucherCode;
    string statusMessage;
    bool inputActive;
    bool voucherApplied;
    bool statusIsError;
    int discountValue;
    int subtotalSnapshot;
    
    // UI animation
    Clock caretClock;
    bool caretVisible;
    
    // Layout constants
    static constexpr size_t kMaxVoucherLength = 16;
    
    // Helper methods
    void normalizeVoucherCode(string& code);
    void clearVoucherPreview();
    FloatRect getInputBounds() const;
    FloatRect getButtonBounds() const;
    
public:
    /**
     * @brief Constructor
     * @param f Font for rendering text
     */
    explicit VoucherInput(Font& f);
    
    /**
     * @brief Handle mouse interactions
     * @param mousePos Current mouse position
     * @param mousePressed Whether mouse button is pressed
     * @param currentSubtotal Current order subtotal
     * @param userEmail User email (for validation)
     * @param isUserLoggedIn Whether user is logged in
     */
    void handleMouseClick(Vector2f mousePos, bool mousePressed, int currentSubtotal, 
                         const string& userEmail, bool isUserLoggedIn);
    
    /**
     * @brief Handle keyboard input events
     * @param event SFML event
     * @param currentSubtotal Current order subtotal
     * @param userEmail User email (for validation)
     * @param isUserLoggedIn Whether user is logged in
     */
    void handleKeyboardInput(const Event& event, int currentSubtotal, 
                            const string& userEmail, bool isUserLoggedIn);
    
    /**
     * @brief Update voucher state (caret blink, etc.)
     */
    void update();
    
    /**
     * @brief Get current voucher display state for OrderSummary
     */
    VoucherDisplayState getDisplayState() const;
    
    /**
     * @brief Get current discount value
     */
    int getDiscountValue() const { return voucherApplied ? discountValue : 0; }
    
    /**
     * @brief Check if voucher is applied
     */
    bool isApplied() const { return voucherApplied; }
    
    /**
     * @brief Get applied voucher code
     */
    string getAppliedCode() const { return appliedVoucherCode; }
    
    /**
     * @brief Reset all voucher state
     */
    void reset();
    
    /**
     * @brief Check if login popup is showing
     */
    bool hasLoginPopup() const { return loginRequiredPopup != nullptr; }
    
    /**
     * @brief Get login popup for rendering/interaction
     */
    LoginRequiredPopup* getLoginPopup() { return loginRequiredPopup.get(); }
    
    /**
     * @brief Close login popup
     */
    void closeLoginPopup() { loginRequiredPopup.reset(); }
    
    /**
     * @brief Refresh voucher discount if subtotal changed
     * @param newSubtotal New order subtotal
     * @param userEmail User email
     * @return true if voucher is still valid, false if invalidated
     */
    bool refreshIfNeeded(int newSubtotal, const string& userEmail);
};
