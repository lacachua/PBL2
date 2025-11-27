#pragma once

#include <string>
#include <vector>
#include <memory>
#include "UI/components/TicketBooking/TicketRepository.h"
#include "UI/components/TicketBooking/ShowtimeSeatRepository.h"
#include "services/VoucherService.h"
#include "services/UserRepository.h"

/**
 * @brief Data Transfer Object for booking information
 * 
 * Follows Single Responsibility: Only holds booking data
 */
struct BookingInfo {
    // Customer info
    std::string customerEmail;
    std::string customerName;
    std::string customerPhone;
    
    // Showtime info
    std::string showtimeId;
    std::string movieId;
    std::string movieName;
    std::string roomId;
    std::string roomName;
    std::string date;
    std::string time;
    int ticketPrice = 0;
    
    // Seats
    std::vector<std::string> selectedSeats;
    std::string seatsDisplay;
    int totalSeats = 0;
    
    // Combos
    struct ComboItem {
        std::string comboName;
        int price = 0;
        int quantity = 0;
    };
    std::vector<ComboItem> selectedCombos;
    int comboTotalPrice = 0;
    
    // Voucher & Total
    std::string voucherCode;
    int voucherDiscount = 0;
    int totalPrice = 0;
};

/**
 * @brief Service class handling all booking business logic
 * 
 * Follows:
 * - Single Responsibility: Only handles booking operations
 * - Dependency Inversion: Depends on abstractions (repositories)
 * - Open/Closed: Can extend without modifying
 */
class BookingService {
private:
    std::unique_ptr<UserRepository> userRepository;
    std::shared_ptr<VoucherService> voucherService;
    TicketRepository ticketRepository;
    ShowtimeSeatRepository seatRepository;

public:
    BookingService(const std::string& usersPath = "../data/users.txt",
                   const std::string& ticketsPath = "../data/tickets.txt",
                   const std::string& seatsPath = "../data/RoomStatusAtShowtime.txt");
    
    /**
     * @brief Get user info by email
     * @param email User's email
     * @param fullName Output: user's full name
     * @param phone Output: user's phone
     */
    void getUserInfo(const std::string& email, std::string& fullName, std::string& phone);
    
    /**
     * @brief Apply voucher to booking
     * @param email User's email
     * @param voucherCode Voucher code to apply
     * @param subtotal Current subtotal
     * @param commit If true, mark voucher as used
     * @return Discount amount (0 if invalid)
     */
    double applyVoucher(const std::string& email, const std::string& voucherCode, 
                        int subtotal, bool commit = false);
    
    /**
     * @brief Get vouchers available for user
     */
    std::vector<VoucherDisplay> getUserVouchers(const std::string& email);
    
    /**
     * @brief Create and save ticket
     * @param info Booking information
     * @return Created ticket
     */
    Ticket createTicket(const BookingInfo& info);
    
    /**
     * @brief Save booked seats to repository
     */
    void saveBookedSeats(const std::string& showtimeId, const std::string& roomId,
                         const std::vector<std::string>& seats);
    
    /**
     * @brief Get booked seats for a showtime
     */
    DLL<std::string> getBookedSeats(const std::string& showtimeId, const std::string& roomId);
    
    /**
     * @brief Calculate total price with discount
     */
    int calculateTotal(int subtotal, int voucherDiscount);
    
    /**
     * @brief Format combo list for storage
     */
    static std::string formatCombosForStorage(const std::vector<BookingInfo::ComboItem>& combos);
};
