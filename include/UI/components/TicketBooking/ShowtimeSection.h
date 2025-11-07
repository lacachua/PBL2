#pragma once
#include <SFML/Graphics.hpp>
#include "UI/components/Button.h"
#include "UI/components/TicketBooking/ShowtimeRepository.h"
#include "data-structures/DLL.h"
using namespace sf;

class ShowtimeSection {
private:
    Font& font;
    DLL<String> availableDates;
    DLL<Button> dateButtons;
    DLL<Button> timeButtons;
    DLL<Showtime> allShowtimes;
    DLL<Showtime> filteredShowtimes;

    String filterMovieId;
    int selectedDateIndex = 0;
    int selectedShowtimeIndex = -1;

public:
    ShowtimeSection(Font&, const String&, const DLL<Showtime>&);
    void initializeDates();
    void loadShowtimesForDate(int);
    void handleClick(Vector2f, bool);
    void draw(RenderWindow&);

    String getSelectedMovieName() const;
    String getSelectedMovieId() const;  // Thêm method để lấy movie_id
    String getSelectedShowtimeId() const;  // Lấy showtime_id
    String getSelectedRoomId() const;      // Lấy room_id (không format)
    String getSelectedDate() const;
    String getSelectedTime() const;
    String getSelectedRoomName() const;
    int getSelectedPrice() const;
    bool hasSelectedShowtime() const;  // Kiểm tra đã chọn suất chiếu chưa
};