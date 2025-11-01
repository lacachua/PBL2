#include "TicketRepository.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <iostream>

namespace fs = std::filesystem;

// Static counters
int TicketRepository::ticketCounter = 1;
int TicketRepository::bookingCounter = 1;

// Helper: Escape CSV field
std::string TicketRepository::escapeCsvField(const std::string& field) {
    bool needsQuotes = field.find(',') != std::string::npos ||
                       field.find('"') != std::string::npos ||
                       field.find('\n') != std::string::npos ||
                       field.find(';') != std::string::npos;
    
    if (!needsQuotes) return field;
    
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') escaped += "\"\"";  // Escape quotes
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

std::string TicketRepository::makeTicketId() {
    std::ostringstream oss;
    oss << "TKT" << std::setfill('0') << std::setw(7) << ticketCounter++;
    return oss.str();
}

std::string TicketRepository::makeBookingCode() {
    // Format: CX + YYYYMMDD + 3-digit counter
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    
    std::ostringstream oss;
    oss << "CX"
        << std::setfill('0')
        << std::setw(4) << (ltm->tm_year + 1900)
        << std::setw(2) << (ltm->tm_mon + 1)
        << std::setw(2) << ltm->tm_mday
        << std::setw(3) << (bookingCounter++ % 1000);
    
    return oss.str();
}

bool TicketRepository::appendTicket(const std::string& path, const TicketRow& ticket) {
    std::cout << "[TicketRepo] appendTicket called for: " << ticket.ticket_id << std::endl;
    
    // ✅ Ensure parent directory exists
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    bool fileExists = fs::exists(path);
    
    // If file doesn't exist or is empty, write header first
    if (!fileExists || fs::file_size(path) == 0) {
        std::ofstream headerFile(path, std::ios::out | std::ios::binary);
        if (!headerFile.is_open()) return false;
        
        headerFile << "ticket_id,booking_code,user_email,movie_id,movie_title,"
                   << "date,time,room_id,seats,seat_count,price_each,food_total,"
                   << "ticket_total,grand_total,status,created_at_epoch,showtime_key,"
                   << "booked_at_epoch,booked_at_local\n";
        headerFile.close();
    }
    
    // Prepare CSV line
    std::ostringstream oss;
    oss << ticket.ticket_id << ","
        << ticket.booking_code << ","
        << ticket.user_email << ","
        << ticket.movie_id << ","
        << escapeCsvField(ticket.movie_title) << ","
        << ticket.date << ","
        << ticket.time << ","
        << ticket.room_id << ","
        << escapeCsvField(ticket.seats) << ","
        << ticket.seat_count << ","
        << ticket.price_each << ","
        << ticket.food_total << ","
        << ticket.ticket_total << ","
        << ticket.grand_total << ","
        << ticket.status << ","
        << ticket.created_at_epoch << ","
        << ticket.showtime_key << ","
        << ticket.booked_at_epoch << ","
        << escapeCsvField(ticket.booked_at_local);
    
    // Atomic append: write to temp file, then append its content to main file
    std::string tempPath = path + ".append.tmp";
    std::ofstream tempFile(tempPath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!tempFile.is_open()) return false;
    
    tempFile << oss.str() << "\n";
    tempFile.close();
    
    // Append temp file content to main file
    std::ofstream mainFile(path, std::ios::out | std::ios::app | std::ios::binary);
    if (!mainFile.is_open()) {
        fs::remove(tempPath);
        return false;
    }
    
    std::ifstream tempRead(tempPath, std::ios::in | std::ios::binary);
    mainFile << tempRead.rdbuf();
    tempRead.close();
    mainFile.close();
    
    // Clean up temp file
    fs::remove(tempPath);
    
    std::cout << "[TicketRepo] Ticket appended successfully: " << ticket.ticket_id << std::endl;
    return true;
}
