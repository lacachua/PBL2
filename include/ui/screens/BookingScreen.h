// // #pragma once
// // #include <SFML/Graphics.hpp>
// // #include "ui/screens/HomeScreen.h"
// // #include "ui/screens/DetailScreen.h"
// // #include "models/Showtime.h"
// // #include "ui/components/Button.h"
// // #include "ui/components/SeatSelector.h"
// // #include "ui/components/SnackMenu.h"
// // #include "ui/components/PaymentSummary.h"
// // #include "ui/components/BookingConfirmation.h"
// // #include <vector>

// // using namespace sf;
// // using namespace std;

// // enum class BookingStep {
// //     SELECT_DATE,
// //     SELECT_SEAT,
// //     SELECT_SNACK,
// //     PAYMENT,
// //     CONFIRM
// // };

// // class BookingScreen : public HomeScreen {
// // private:
// //     // Fonts
// //     Font buttons_font;
// //     Font detailFont;
    
// //     // Current step
// //     BookingStep current_step;
    
// //     // Step indicator UI
// //     Text suat_chieu, ghe_ngoi, food, thanh_toan, xac_nhan;
// //     RectangleShape buttons_box[5];
    
// //     // Background
// //     Texture tex;
// //     Sprite sprite;
// //     RectangleShape content_area;

// //     // Showtime data
// //     vector<Showtime> allShowtimes;
// //     vector<Showtime> showtimesForSelectedDate;
// //     vector<string> availableDates;
// //     string selectedDate;
// //     int currentMovieId;
// //     int selectedShowtimeIndex;

// //     // Date/Time selection buttons
// //     vector<Button> dateButtons;
// //     vector<Button> timeButtons;
    
// //     // Action buttons (Confirm & Back)
// //     Button confirmButton;
// //     Button backButton;
    
// //     // State flags
// //     bool hasConfirmedShowtime;
// //     bool shouldReturnHome;
// //     string bookingCode;
    
// //     // Components (NEW!)
// //     SeatSelector seatSelector;
// //     SnackMenu snackMenu;
// //     PaymentSummary paymentSummary;
// //     BookingConfirmation confirmation;
    
// //     // Helper methods
// //     void drawStepContent(RenderWindow&);
// //     void drawDateSelection(RenderWindow&);
// //     void drawTimeSelection(RenderWindow&);
// //     void drawActionButtons(RenderWindow&);
    
// //     void buildDateButtons();
// //     void buildTimeButtons();
// //     void resetBookingData();
// //     void updateShowtimesForSelectedDate(int currentHour, int currentMinute, const string& todayStr);
// //     vector<Showtime> generateShowtimesForNext30Days(int movieId);

// // public:
// //     BookingScreen(Font&);
    
// //     void handleEvent(const RenderWindow&, const Vector2f&, bool);
// //     void update(Vector2f, bool, AppState&);
// //     void draw(RenderWindow&);
// //     void loadFromDetail(const DetailScreen&);
// // };

// #pragma once
// #include <SFML/Graphics.hpp>
// #include "data-structures/DLL.h"
// #include "core/AppState.h"
// #include "BaseScreen.h"
// #include "UI/components/TicketBooking/HeaderBar.h"
// #include "UI/components/TicketBooking/BookingState.h"
// #include "UI/components/TicketBooking/ShowtimeRepository.h"
// #include "UI/components/TicketBooking/SummaryPanel.h"
// #include <ctime>

// class BookingScreen : public BaseScreen {
//     private:
//         Font& font;
//         HeaderBar headerBar;
//         BookingState currentState;

//         DLL<Showtime> showtimes;
//         String filterMovieId;
        
//         DLL<String> availableDates;
//         int selectedDateIndex;
//         int selectedShowtimeIndex;
//         DLL<Showtime> showtimesForDate;
        
//         wstring selectedMovieTitle;
//         String selectedRoomName;
//         String selectedDate;
//         String selectedTime;
//         int totalPrice;
        
//         DLL<Button> dateButtons;
//         DLL<Button> timeButtons;
        
//         // Helper methods
//         void initializeDates();
//         void loadShowtimesForDate(int dateIndex);
//         void drawShowtimeSelection(RenderWindow&);
//         void drawSummaryPanel(RenderWindow&);
//         void handleShowtimeClick(Vector2f mousePos, bool mousePressed);
//         String getMovieTitle(const String& movieId);
//         String getRoomName(const String& roomId);
        
//     public:
//         BookingScreen(Font&, const String& movieId);  // Constructor with movie filter
//         void update(Vector2f, bool, AppState&) override;
//         void draw(RenderWindow&) override;
//         void setMovieFilter(const String& movieId);
// };  

// #pragma once
// #include "UI/screens/BaseScreen.h"
// #include "UI/components/TicketBooking/HeaderBar.h"
// #include "UI/components/TicketBooking/ShowtimeSection.h"
// #include "UI/components/TicketBooking/SeatSelection.h"
// #include "UI/components/TicketBooking/SummaryPanel.h"
// #include "UI/components/TicketBooking/ShowtimeRepository.h"
// #include "UI/components/TicketBooking/ShowtimeSeatRepository.h"
// #include "UI/components/TicketBooking/BookingState.h"
// #include "core/AppState.h"

// class BookingScreen : public BaseScreen {
// private:
//     Font& font;
//     HeaderBar header;
//     ShowtimeSection showtimeSection;
//     SeatSelection seatSelection;
//     SummaryPanel summary;
//     ShowtimeSeatRepository seatRepo;  // Repository quản lý ghế đã đặt
//     BookingState currentState;

// public:
//     BookingScreen(Font&, const String&);
//     void update(Vector2f, bool, AppState&) override;
//     void draw(RenderWindow&) override;
// };

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

    // TODO: chuyển sang smart pointer
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
