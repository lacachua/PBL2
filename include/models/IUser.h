#pragma once
#include <string>
#include <ctime>
#include "core/AppRole.h"

using namespace std;

/**
 * @brief Interface (Abstract Base Class) cho tất cả người dùng trong hệ thống
 * 
 * Đây là base class trừu tượng định nghĩa các phương thức chung
 * mà tất cả loại người dùng (Guest, Customer, Admin) phải implement.
 * 
 * Design Pattern: Template Method + Strategy Pattern
 * - Các derived class implement các phương thức virtual
 * - Cho phép polymorphism khi xử lý người dùng
 */
class IUser {
protected:
    string email;           // Primary key - IMMUTABLE
    string passwordHash;
    string fullName;
    string birthDate;       // Format: dd/mm/yyyy
    string phone;
    time_t registeredAt;    // Unix timestamp
    AppRole role;

public:
    // ===== CONSTRUCTORS & DESTRUCTOR =====
    
    IUser() 
        : email(""), passwordHash(""), fullName(""), birthDate(""),
                    phone(""), registeredAt(0), role(AppRole::Guest) {}
    
    IUser(const string& email, const string& passwordHash,
          const string& fullName, const string& birthDate,
          const string& phone, time_t registeredAt,
                    AppRole role = AppRole::Guest)
        : email(email), passwordHash(passwordHash), fullName(fullName),
          birthDate(birthDate), phone(phone), registeredAt(registeredAt),
                    role(role) {}
    
    virtual ~IUser() = default;

    // ===== PURE VIRTUAL METHODS (Must be implemented by derived classes) =====
    
    /**
     * @brief Lấy tên loại người dùng
     * @return "Guest", "Customer", hoặc "Admin"
     */
    virtual string getUserType() const = 0;
    
    /**
     * @brief Kiểm tra quyền truy cập vào một chức năng
     * @param feature Tên chức năng cần kiểm tra
     * @return true nếu có quyền truy cập
     */
    virtual bool hasPermission(const string& feature) const = 0;
    
    /**
     * @brief Lấy danh sách các quyền của người dùng
     * @return Chuỗi mô tả các quyền
     */
    virtual string getPermissions() const = 0;

    // ===== COMMON GETTERS (Non-virtual - same for all) =====
    
    string getEmail() const { return email; }
    string getPasswordHash() const { return passwordHash; }
    string getFullName() const { return fullName; }
    string getBirthDate() const { return birthDate; }
    string getPhone() const { return phone; }
    time_t getRegisteredAt() const { return registeredAt; }
    AppRole getRole() const { return role; }
    string getUsername() const { return fullName; }

    // ===== COMMON SETTERS (Email is IMMUTABLE - no setter) =====
    
    void setPasswordHash(const string& hash) { passwordHash = hash; }
    void setFullName(const string& name) { fullName = name; }
    void setBirthDate(const string& date) { birthDate = date; }
    void setPhone(const string& phoneNum) { phone = phoneNum; }

    // ===== HELPER METHODS =====
    
    string getRoleString() const {
        switch (role) {
            case AppRole::Guest: return "guest";
            case AppRole::Customer: return "customer";
            case AppRole::Admin: return "admin";
            default: return "guest";
        }
    }
    
    static AppRole parseRole(const string& roleStr);
};
