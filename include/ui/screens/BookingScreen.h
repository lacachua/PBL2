#pragma once
#include <SFML/Graphics.hpp>
#include "ui/screens/HomeScreen.h"
#include "ui/screens/DetailScreen.h"
#include "models/Showtime.h"
#include "ui/components/Button.h"
#include "ui/components/SeatSelector.h"
#include "ui/components/SnackMenu.h"
#include "ui/components/PaymentSummary.h"
#include "ui/components/BookingConfirmation.h"
#include <vector>

using namespace sf;
using namespace std;

enum class BookingStep {
    SELECT_DATE,
    SELECT_SEAT,
    SELECT_SNACK,
    PAYMENT,
    CONFIRM
};

class BookingScreen : public HomeScreen {
private:
    // Fonts
    Font buttons_font;
    Font detailFont;
    
    // Current step
    BookingStep current_step;
    
    // Step indicator UI
    Text suat_chieu, ghe_ngoi, food, thanh_toan, xac_nhan;
    RectangleShape buttons_box[5];
    
    // Background
    Texture tex;
    Sprite sprite;
    RectangleShape content_area;

    // Showtime data
    vector<Showtime> allShowtimes;
    vector<Showtime> showtimesForSelectedDate;
    vector<string> availableDates;
    string selectedDate;
    int currentMovieId;
    int selectedShowtimeIndex;

    // Date/Time selection buttons
    vector<Button> dateButtons;
    vector<Button> timeButtons;
    
    // Action buttons (Confirm & Back)
    Button confirmButton;
    Button backButton;
    
    // State flags
    bool hasConfirmedShowtime;
    bool shouldReturnHome;
    string bookingCode;
    
    // Components (NEW!)
    SeatSelector seatSelector;
    SnackMenu snackMenu;
    PaymentSummary paymentSummary;
    BookingConfirmation confirmation;
    
    // Helper methods
    void drawStepContent(RenderWindow&);
    void drawDateSelection(RenderWindow&);
    void drawTimeSelection(RenderWindow&);
    void drawActionButtons(RenderWindow&);
    
    void buildDateButtons();
    void buildTimeButtons();
    void resetBookingData();
    void updateShowtimesForSelectedDate(int currentHour, int currentMinute, const string& todayStr);
    vector<Showtime> generateShowtimesForNext30Days(int movieId);

public:
    BookingScreen(Font&);
    
    void handleEvent(const RenderWindow&, const Vector2f&, bool);
    void update(Vector2f, bool, AppState&);
    void draw(RenderWindow&);
    void loadFromDetail(const DetailScreen&);
};