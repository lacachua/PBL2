#pragma once
#include <SFML/Graphics.hpp>
#include "HomeScreen.h"
#include "DetailScreen.h"
#include "Showtime.h"
#include <vector>
#include "Button.h"

enum class BookingStep {
    SELECT_DATE,
    SELECT_SEAT,
    SELECT_SNACK,
    PAYMENT,
    CONFIRM
};

// ✅ Struct cho món ăn/combo
struct SnackItem {
    string name;
    int price;
    string imagePath;
    Texture texture;
    int quantity; // Số lượng đã chọn
    
    SnackItem(const string& n, int p, const string& img) 
        : name(n), price(p), imagePath(img), quantity(0) {
        // Texture sẽ được load sau
    }
    
    // Helper để vẽ sprite
    void draw(RenderWindow& window, float x, float y, float size) {
        Sprite sprite(texture);
        sprite.setPosition({x, y});
        float scaleX = size / texture.getSize().x;
        float scaleY = size / texture.getSize().y;
        sprite.setScale({scaleX, scaleY});
        window.draw(sprite);
    }
};

class BookingScreen : public HomeScreen {
private:
    Font buttons_font;
    Font detailFont;
    BookingStep current_step;
    Text suat_chieu, ghe_ngoi, food, thanh_toan, xac_nhan;
    RectangleShape buttons_box[5];
    Texture tex;
    Sprite sprite;
    RectangleShape content_area;

    vector<Showtime> allShowtimes;
    vector<Showtime> showtimesForSelectedDate;
    vector<string> availableDates;
    string selectedDate;
    int currentMovieId;
    int selectedShowtimeIndex;

    vector<Button> dateButtons;
    vector<Button> timeButtons;
    
    // ✅ Action buttons: Xác nhận & Quay lại
    Button confirmButton;
    Button backButton;
    bool hasConfirmedShowtime; // Đã xác nhận suất chiếu chưa
    bool shouldReturnHome; // ✅ Flag để quay về home
    string bookingCode; // ✅ Mã vé được tạo 1 lần duy nhất
    
    // ✅ Seat selection data
    vector<string> selectedSeats; // Danh sách ghế đã chọn (VD: "A1", "B5")
    static const int SEAT_ROWS = 9; // A-I
    static const int SEAT_COLS = 9; // 1-9
    vector<string> occupiedSeats; // Ghế đã được đặt (từ database/showtime)
    
    // ✅ Snack menu data
    vector<SnackItem> snackItems;
    vector<Button> plusButtons;  // Nút + cho mỗi combo
    vector<Button> minusButtons; // Nút - cho mỗi combo

    void drawStepContent(RenderWindow&);
    void drawDateSelection(RenderWindow&);
    void drawTimeSelection(RenderWindow&);
    void drawActionButtons(RenderWindow&); // Vẽ nút Xác nhận/Quay lại
    void drawSeatSelection(RenderWindow&); // Vẽ sơ đồ ghế
    void drawSeatSummary(RenderWindow&);   // Vẽ thông tin ghế đã chọn
    void drawSnackMenu(RenderWindow&);     // ✅ Vẽ menu đồ ăn
    void drawPaymentSummary(RenderWindow&); // ✅ Vẽ tổng hợp thanh toán
    void drawConfirmation(RenderWindow&);   // ✅ Vẽ xác nhận đặt vé thành công
    bool isSeatOccupied(const string& seat) const;
    bool isSeatSelected(const string& seat) const;
    void loadOccupiedSeatsFromSeatMap(const string& seat_map); // ✅ Load ghế đã đặt từ bitmap
    void saveSelectedSeatsToSeatMap(); // ✅ Lưu ghế đã chọn vào file
    void buildDateButtons();
    void buildTimeButtons();
    void initializeSnackMenu(); // ✅ Khởi tạo menu đồ ăn
    void resetBookingData(); // ✅ Reset toàn bộ dữ liệu đặt vé
    void updateShowtimesForSelectedDate(int, int, const string&);

    vector<Showtime> generateShowtimesForNext30Days(int);
public:
    BookingScreen(Font&);
    void handleEvent(const RenderWindow&, const Vector2f&, bool);
    void update(Vector2f, bool, AppState&); // ✅ Thêm tham số giống HomeScreen
    void draw(RenderWindow&);

    void loadFromDetail(const DetailScreen&);
};
