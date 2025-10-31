#pragma once
#include <string>
#include <ctime>

struct TicketRow {
    std::string ticket_id;
    std::string booking_code;
    std::string user_email;
    int movie_id;
    std::string movie_title;
    std::string date;
    std::string time;
    std::string room_id;         // Room ID like "Phong1", "Phong2"
    std::string seats;           // e.g., "A5;A6;B3"
    int seat_count;
    int price_each;
    int food_total;
    int ticket_total;
    int grand_total;
    std::string status;          // "PAID", "CANCELLED", etc.
    long long created_at_epoch;
    std::string showtime_key;    // ✅ Added: deterministic showtime identifier
    long long booked_at_epoch;   // ✅ Thời gian đặt vé thực tế (Unix timestamp)
    std::string booked_at_local; // ✅ Thời gian địa phương (YYYY-MM-DD HH:MM:SS+07:00)
    
    TicketRow()
        : movie_id(0), seat_count(0), price_each(0),
          food_total(0), ticket_total(0), grand_total(0),
          created_at_epoch(0), showtime_key(""),
          booked_at_epoch(0), booked_at_local("") {}
};

class TicketRepository {
public:
    /**
     * Generate unique ticket ID (e.g., TKT0000001)
     * @return Ticket ID string
     */
    static std::string makeTicketId();
    
    /**
     * Generate booking code (e.g., CX20251030001)
     * Format: CX + YYYYMMDD + 3-digit counter
     * @return Booking code string
     */
    static std::string makeBookingCode();
    
    /**
     * Append ticket to tickets.csv (atomic write)
     * Creates file with header if doesn't exist
     * 
     * @param path Path to tickets.csv
     * @param ticket Ticket data to append
     * @return true if successful, false otherwise
     */
    static bool appendTicket(const std::string& path, const TicketRow& ticket);

private:
    // Helper: Escape CSV field (wrap in quotes if contains comma/quote)
    static std::string escapeCsvField(const std::string& field);
    
    // Counter for ticket/booking code generation
    static int ticketCounter;
    static int bookingCounter;
};
