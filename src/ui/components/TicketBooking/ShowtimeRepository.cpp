#include "UI/components/TicketBooking/ShowtimeRepository.h"
#include <fstream>
#include <sstream>

DLL<Showtime> ShowtimeRepository::loadFromFile(const String& path) {
    DLL<Showtime> showtimes;
    ifstream file(path.toAnsiString());
    if (!file.is_open()) return showtimes;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Showtime s;

        getline(ss, token, '|');
        s.showtime_id = token;

        getline(ss, token, '|');
        s.movie_id = token;

        getline(ss, token, '|');
        s.room_id = token;

        getline(ss, token, '|');
        s.date = token;

        getline(ss, token, '|');
        s.time = token;

        showtimes.push_back(s);
    }

    file.close();
    return showtimes;
}
