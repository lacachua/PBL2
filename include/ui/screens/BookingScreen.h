#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "BaseScreen.h"
#include "UI/components/TicketBooking/HeaderBar.h"
#include "UI/components/TicketBooking/ShowtimeSection.h"
#include "UI/components/TicketBooking/SeatSelection.h"
#include "UI/components/TicketBooking/SummaryPanel.h"
#include "repositories/booking/ShowtimeSeatRepository.h"
#include "repositories/booking/ComboRepository.h"
#include "UI/components/TicketBooking/ComboSelection.h"
#include "UI/components/TicketBooking/OrderSummary.h"
#include "UI/components/TicketBooking/LoginRequiredPopup.h"
#include "UI/components/TicketBooking/ConfirmationView.h"
#include "repositories/booking/TicketRepository.h"
#include "services/BookingService.h"
#include "core/AppState.h"

using namespace sf;
using namespace std;

class BookingScreen : public BaseScreen {
private:
    Font& font;
    HeaderBar header;
    SummaryPanel summary;

    BookingState currentState;
    
    // Booking data - lưu trữ toàn bộ thông tin booking qua các state
    BookingData bookingData;
    
    ShowtimeSection showtimeSection;

    unique_ptr<SeatSelection> seatSelection;

    unique_ptr<ComboSelection> comboSelection;
    ComboRepository comboRepo;

    unique_ptr<OrderSummary> orderSummary;

    ShowtimeSeatRepository seatRepo;
    
    // Business logic service (SOLID: Dependency Inversion)
    std::unique_ptr<BookingService> bookingService;
    
    // Popup yêu cầu đăng nhập (unique_ptr để quản lý lifecycle)
    unique_ptr<LoginRequiredPopup> loginPopup;
    unique_ptr<LoginRequiredPopup> voucherLoginPopup;
    
    // Confirmation view cho state xacnhan
    unique_ptr<ConfirmationView> confirmationView;
    
    // Repository để tạo và lưu vé
    TicketRepository ticketRepo;

    // Voucher UI state
    std::string voucherInput;
    std::string appliedVoucherCode;
    std::string voucherStatusMessage;
    bool voucherStatusIsError = false;
    bool voucherInputActive = false;
    bool voucherApplied = false;
    int voucherDiscountValue = 0;
    int voucherSubtotalSnapshot = 0;
    sf::Clock voucherCaretClock;
    bool voucherCaretVisible = true;
    
    // Helper để lấy thông tin user từ TXT
    void getUserInfo(const string& email, string& fullName, string& phone);
    void handleVoucherInteractions(Vector2f mousePos, bool mousePressed, int currentSubtotal);
    void applyVoucherCode(int currentSubtotal);
    void clearVoucherPreview();
    void resetVoucherState();
    FloatRect getVoucherInputBounds() const;
    FloatRect getVoucherButtonBounds() const;

public:
    BookingScreen(Font& f, const String& movieId);

    void update(Vector2f mousePos, bool mousePressed, AppState& state) override;
    void draw(RenderWindow& window) override;
    void handleEvent(const Event& event) override;
};
