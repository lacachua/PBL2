#pragma once
#include <string>

namespace models {

/**
 * @brief User entity - Represents a cinema user
 * Plain Old C++ Object (POCO) - No dependencies
 */
class User {
private:
    std::string email;
    std::string passwordHash;
    std::string fullName;
    std::string phoneNumber;
    std::string address;
    std::string createdAt;
    
public:
    User() = default;
    
    User(const std::string& email, 
         const std::string& passwordHash,
         const std::string& fullName,
         const std::string& phone,
         const std::string& address)
        : email(email), 
          passwordHash(passwordHash),
          fullName(fullName),
          phoneNumber(phone),
          address(address) {}
    
    // Getters
    std::string getEmail() const { return email; }
    std::string getPasswordHash() const { return passwordHash; }
    std::string getFullName() const { return fullName; }
    std::string getPhoneNumber() const { return phoneNumber; }
    std::string getAddress() const { return address; }
    std::string getCreatedAt() const { return createdAt; }
    
    // Setters
    void setEmail(const std::string& e) { email = e; }
    void setPasswordHash(const std::string& hash) { passwordHash = hash; }
    void setFullName(const std::string& name) { fullName = name; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    void setAddress(const std::string& addr) { address = addr; }
    void setCreatedAt(const std::string& timestamp) { createdAt = timestamp; }
    
    // Validation (basic business rules)
    bool isValid() const {
        return !email.empty() && 
               !passwordHash.empty() && 
               !fullName.empty();
    }
};

} // namespace models
