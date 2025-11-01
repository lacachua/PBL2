#pragma once
#include <string>
#include <vector>

namespace models {

/**
 * @brief Ticket entity - Represents a booking ticket
 * POCO - Aggregate root for booking
 */
class Ticket {
private:
    std::string ticketId;
    std::string bookingCode;
    std::string userEmail;
    std::string showtimeId;
    int movieId;
    std::string movieTitle;
    std::string date;
    std::string time;
    std::string room;
    std::vector<std::string> seats;  // e.g., ["A1", "A2"]
    int ticketPrice;                 // Price per seat
    std::vector<std::pair<std::string, int>> foodItems; // (name, quantity)
    int foodTotal;
    int totalAmount;
    std::string status;              // "Pending", "Confirmed", "Cancelled"
    std::string createdAt;
    
public:
    Ticket() : movieId(0), ticketPrice(0), foodTotal(0), totalAmount(0) {}
    
    // Getters
    std::string getTicketId() const { return ticketId; }
    std::string getBookingCode() const { return bookingCode; }
    std::string getUserEmail() const { return userEmail; }
    std::string getShowtimeId() const { return showtimeId; }
    int getMovieId() const { return movieId; }
    std::string getMovieTitle() const { return movieTitle; }
    std::string getDate() const { return date; }
    std::string getTime() const { return time; }
    std::string getRoom() const { return room; }
    std::vector<std::string> getSeats() const { return seats; }
    int getSeatCount() const { return seats.size(); }
    int getTicketPrice() const { return ticketPrice; }
    std::vector<std::pair<std::string, int>> getFoodItems() const { return foodItems; }
    int getFoodTotal() const { return foodTotal; }
    int getTotalAmount() const { return totalAmount; }
    std::string getStatus() const { return status; }
    std::string getCreatedAt() const { return createdAt; }
    
    // Setters
    void setTicketId(const std::string& id) { ticketId = id; }
    void setBookingCode(const std::string& code) { bookingCode = code; }
    void setUserEmail(const std::string& email) { userEmail = email; }
    void setShowtimeId(const std::string& id) { showtimeId = id; }
    void setMovieId(int id) { movieId = id; }
    void setMovieTitle(const std::string& title) { movieTitle = title; }
    void setDate(const std::string& d) { date = d; }
    void setTime(const std::string& t) { time = t; }
    void setRoom(const std::string& r) { room = r; }
    void setSeats(const std::vector<std::string>& s) { seats = s; }
    void setTicketPrice(int price) { ticketPrice = price; }
    void setFoodItems(const std::vector<std::pair<std::string, int>>& items) { foodItems = items; }
    void setFoodTotal(int total) { foodTotal = total; }
    void setTotalAmount(int amount) { totalAmount = amount; }
    void setStatus(const std::string& s) { status = s; }
    void setCreatedAt(const std::string& timestamp) { createdAt = timestamp; }
    
    // Business logic
    void addSeat(const std::string& seatId) {
        seats.push_back(seatId);
    }
    
    void addFoodItem(const std::string& name, int quantity) {
        foodItems.push_back({name, quantity});
    }
    
    void calculateTotal() {
        totalAmount = (ticketPrice * seats.size()) + foodTotal;
    }
    
    // Validation
    bool isValid() const {
        return !ticketId.empty() &&
               !userEmail.empty() &&
               !seats.empty() &&
               ticketPrice >= 0 &&
               totalAmount >= 0;
    }
};

} // namespace models
