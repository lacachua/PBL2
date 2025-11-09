#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <SFML/System/String.hpp>   // cần include
using namespace std;
using namespace sf;

struct Ticket {
    string ticketId;
    string showtimeId;
    string title;
    string date;
    string time;
    string roomName;
    string booked;
    string comboName;
    int price;
    string email;
    string fullName;
    string bookedDate;
    string bookedTime;
};

class TicketRepository {
private:
    string filename;

    string generateTicketId() {
        ifstream file(filename);
        int maxId = 0;
        if (file.is_open()) {
            string line;
            getline(file, line);
            while (getline(file, line)) {
                if (line.empty()) continue;
                stringstream ss(line);
                string ticketId;
                getline(ss, ticketId, '|');
                if (ticketId.length() > 1 && ticketId[0] == 'T') {
                    try {
                        int id = stoi(ticketId.substr(1));
                        if (id > maxId) maxId = id;
                    } catch (...) {}
                }
            }
            file.close();
        }
        maxId++;
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "T%04d", maxId);
        return string(buffer);
    }

    string getCurrentDate() {
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&now_c);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y", localTime);
        return string(buffer);
    }

    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        tm* localTime = localtime(&now_c);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
        return string(buffer);
    }

public:
    TicketRepository(const string& filepath = "../data/tickets.txt")
        : filename(filepath) {}

    Ticket createTicket(
        const string& showtimeId,
        const string& title,
        const string& date,
        const string& time,
        const string& roomName,
        const string& booked,
        const string& comboName,
        int price,
        const string& email,
        const string& fullName
    ) {
        Ticket ticket;
        ticket.ticketId = generateTicketId();
        ticket.showtimeId = showtimeId;
        ticket.title = title;
        ticket.date = date;
        ticket.time = time;
        ticket.roomName = roomName;
        ticket.booked = booked;
        ticket.comboName = comboName;
        ticket.price = price;
        ticket.email = email;
        ticket.fullName = fullName;
        ticket.bookedDate = getCurrentDate();
        ticket.bookedTime = getCurrentTime();
        saveTicket(ticket);
        return ticket;
    }

    void saveTicket(const Ticket& ticket) {
        // Mở file với binary mode để đảm bảo UTF-8 được lưu đúng
        ofstream file(filename, ios::app | ios::binary);
        if (!file.is_open()) return;    
        
        // Nếu file trống, ghi header với UTF-8 BOM
        file.seekp(0, ios::end);
        if (file.tellp() == 0) {
            // Write UTF-8 BOM
            const char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF };
            file.write(bom, 3);
            file << "ticketId|showtimeId|title|date|time|roomName|booked|comboName|price|email|fullName|bookedDate|bookedTime\n";
        }
        
        file << ticket.ticketId << "|"
             << ticket.showtimeId << "|"
             << ticket.title << "|"
             << ticket.date << "|"
             << ticket.time << "|"
             << ticket.roomName << "|"
             << ticket.booked << "|"
             << ticket.comboName << "|"
             << ticket.price << "|"
             << ticket.email << "|"
             << ticket.fullName << "|"
             << ticket.bookedDate << "|"
             << ticket.bookedTime << "\n";
        file.close();
    }

    // =======================
    // Getters trả về dạng UTF-8 (sf::String)
    // =======================
    static String getTitleUtf8(const Ticket& t) { return String::fromUtf8(t.title.begin(), t.title.end()); }
    static String getRoomUtf8(const Ticket& t) { return String::fromUtf8(t.roomName.begin(), t.roomName.end()); }
    static String getFullNameUtf8(const Ticket& t) { return String::fromUtf8(t.fullName.begin(), t.fullName.end()); }
    static String getComboUtf8(const Ticket& t) { return String::fromUtf8(t.comboName.begin(), t.comboName.end()); }
    static String getDateUtf8(const Ticket& t) { return String::fromUtf8(t.date.begin(), t.date.end()); }
    static String getTimeUtf8(const Ticket& t) { return String::fromUtf8(t.time.begin(), t.time.end()); }
    static String getBookedUtf8(const Ticket& t) { return String::fromUtf8(t.booked.begin(), t.booked.end()); }
};