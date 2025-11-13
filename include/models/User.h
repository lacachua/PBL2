#pragma once
#include <string>
#include <ctime>
#include "core/AppRole.h"

using namespace std;

/**
 * @brief Model đại diện cho người dùng trong hệ thống
 * 
 * Responsibility (Single Responsibility Principle):
 * - Chứa thông tin người dùng
 * - Cung cấp getters/setters
 * - Không chứa logic nghiệp vụ (để cho AuthService, UserRepository)
 * 
 * Note: Email là khóa chính, KHÔNG ĐƯỢC PHÉP THAY ĐỔI
 */
class User {
private:
    string email;           // Primary key - IMMUTABLE
    string passwordHash;
    string fullName;
    string birthDate;       // Format: dd/mm/yyyy
    string phone;
    time_t registeredAt;    // Unix timestamp
    AppRole role;
    UserStatus status;

public:
    /**
     * @brief Constructor mặc định
     */
    User();

    /**
     * @brief Constructor đầy đủ
     */
    User(const string& email, 
         const string& passwordHash,
         const string& fullName,
         const string& birthDate,
         const string& phone,
         time_t registeredAt,
         AppRole role = AppRole::Customer,
         UserStatus status = UserStatus::Active);

    // ===== GETTERS =====
    
    /**
     * @brief Lấy email (primary key - IMMUTABLE)
     * @return Email của user
     */
    string getEmail() const { return email; }
    
    string getPasswordHash() const { return passwordHash; }
    string getFullName() const { return fullName; }
    string getBirthDate() const { return birthDate; }
    string getPhone() const { return phone; }
    time_t getRegisteredAt() const { return registeredAt; }
    AppRole getRole() const { return role; }
    UserStatus getStatus() const { return status; }
    
    /**
     * @brief Lấy username để hiển thị (từ fullName)
     * @return Tên hiển thị
     */
    string getUsername() const { return fullName; }
    
    /**
     * @brief Kiểm tra tài khoản có đang hoạt động không
     * @return true nếu status = Active
     */
    bool isActive() const { return status == UserStatus::Active; }
    
    /**
     * @brief Kiểm tra tài khoản có bị khóa không
     * @return true nếu status = Locked
     */
    bool isLocked() const { return status == UserStatus::Locked; }

    // ===== SETTERS (Email KHÔNG CÓ setter - IMMUTABLE) =====
    
    void setPasswordHash(const string& hash) { passwordHash = hash; }
    void setFullName(const string& name) { fullName = name; }
    void setBirthDate(const string& date) { birthDate = date; }
    void setPhone(const string& phoneNum) { phone = phoneNum; }
    void setRole(AppRole r) { role = r; }
    void setStatus(UserStatus s) { status = s; }
    
    /**
     * @brief Khóa tài khoản
     */
    void lock() { status = UserStatus::Locked; }
    
    /**
     * @brief Mở khóa tài khoản
     */
    void unlock() { status = UserStatus::Active; }
    
    // ===== HELPER METHODS =====
    
    /**
     * @brief Chuyển role thành chuỗi để lưu file
     * @return "guest", "customer", "staff", hoặc "admin"
     */
    string getRoleString() const;
    
    /**
     * @brief Chuyển status thành chuỗi để lưu file
     * @return "active" hoặc "locked"
     */
    string getStatusString() const;
    
    /**
     * @brief Parse role từ chuỗi
     */
    static AppRole parseRole(const string& roleStr);
    
    /**
     * @brief Parse status từ chuỗi
     */
    static UserStatus parseStatus(const string& statusStr);
};
