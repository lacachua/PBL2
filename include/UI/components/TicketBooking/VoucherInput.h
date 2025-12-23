#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "services/BookingService.h"
#include "UI/components/TicketBooking/LoginRequiredPopup.h"
#include "UI/components/TicketBooking/VoucherDisplayState.h"

using namespace sf;
using namespace std;

class VoucherInput {
private:
    Font& font;
    unique_ptr<BookingService> bookingService;
    unique_ptr<LoginRequiredPopup> loginRequiredPopup;
    
    // Trạng thái input
    string inputText;
    string appliedVoucherCode;
    string statusMessage;
    bool inputActive;
    bool voucherApplied;
    bool statusIsError;
    int discountValue;
    int subtotalSnapshot;
    
    // Hiệu ứng UI
    Clock caretClock;
    bool caretVisible;
    
    // Hằng số bố cục
    static constexpr size_t kMaxVoucherLength = 16;
    
    // Các hàm hỗ trợ
    void normalizeVoucherCode(string& code);
    void clearVoucherPreview();
    FloatRect getInputBounds() const;
    FloatRect getButtonBounds() const;
    
public:
    explicit VoucherInput(Font& f);
    void handleMouseClick(Vector2f mousePos, bool mousePressed, int currentSubtotal, 
                         const string& userEmail, bool isUserLoggedIn);
    void handleKeyboardInput(const Event& event, int currentSubtotal, 
                            const string& userEmail, bool isUserLoggedIn);
    void update();
    VoucherDisplayState getDisplayState() const;
    int getDiscountValue() const;
    bool isApplied() const;
    string getAppliedCode() const;
    void reset();
    bool hasLoginPopup() const;
    LoginRequiredPopup* getLoginPopup();
    void closeLoginPopup();
    bool refreshIfNeeded(int newSubtotal, const string& userEmail);
};
