#pragma once
#include "models/User.h"
#include "data-structures/HashTable.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Repository quản lý CRUD operations cho User
 * 
 * Responsibility (Single Responsibility Principle):
 * - Đọc/ghi file users.txt
 * - CRUD operations với email làm khóa chính
 * - Validate email uniqueness
 * 
 * Design Pattern: Repository Pattern
 * - Tách biệt logic truy xuất dữ liệu khỏi business logic
 * - Dễ test, dễ thay đổi storage (từ file sang database)
 * 
 * Note: Email là PRIMARY KEY - IMMUTABLE
 */
class UserRepository {
private:
    string filePath;
    HashTable<string, User> users;  // key = email (lowercase)
    
    /**
     * @brief Normalize email thành lowercase để so sánh
     * @param email Email gốc
     * @return Email viết thường
     */
    string normalizeEmail(const string& email) const;
    
    /**
     * @brief Parse một dòng từ file thành User object
     * Format: email|passwordHash|fullName|birthDate|phone|registeredAt|role|status
     */
    User parseLine(const string& line) const;
    
    /**
     * @brief Chuyển User object thành dòng text để ghi file
     */
    string serializeLine(const User& user) const;

public:
    /**
     * @brief Constructor
     * @param filepath Đường dẫn đến file users.txt
     */
    UserRepository(const string& filepath = "../data/users.txt");
    
    /**
     * @brief Destructor - tự động save khi destroy
     */
    ~UserRepository();
    
    // ===== CRUD OPERATIONS =====
    
    /**
     * @brief Load tất cả users từ file
     */
    void loadFromFile();
    
    /**
     * @brief Save tất cả users ra file
     */
    void saveToFile();
    
    /**
     * @brief Kiểm tra email đã tồn tại chưa
     * @param email Email cần kiểm tra (case-insensitive)
     * @return true nếu email đã tồn tại
     */
    bool exists(const string& email) const;
    
    /**
     * @brief Tìm user theo email
     * @param email Email (case-insensitive)
     * @return Pointer to User, hoặc nullptr nếu không tìm thấy
     */
    User* findByEmail(const string& email);
    
    /**
     * @brief Thêm user mới
     * @param user User object
     * @return true nếu thành công, false nếu email đã tồn tại
     */
    bool addUser(const User& user);
    
    /**
     * @brief Cập nhật thông tin user (trừ email)
     * @param user User object với email không đổi
     * @return true nếu thành công, false nếu không tìm thấy
     */
    bool updateUser(const User& user);
    
    /**
     * @brief Xóa user (soft delete - set status = locked)
     * @param email Email của user
     * @return true nếu thành công
     */
    bool lockUser(const string& email);
    
    /**
     * @brief Mở khóa user
     * @param email Email của user
     * @return true nếu thành công
     */
    bool unlockUser(const string& email);
    
    /**
     * @brief Xóa user hoàn toàn khỏi hệ thống (hard delete)
     * @param email Email của user
     * @return true nếu thành công
     */
    bool deleteUser(const string& email);
    
    /**
     * @brief Lấy danh sách tất cả users
     * @return Vector chứa tất cả users
     */
    vector<User> getAllUsers() const;
    
    /**
     * @brief Lấy danh sách users theo role
     * @param role Role cần lọc
     * @return Vector chứa users có role tương ứng
     */
    vector<User> getUsersByRole(AppRole role) const;
    
    /**
     * @brief Đếm số lượng users theo role
     * @param role Role cần đếm
     * @return Số lượng users
     */
    int countByRole(AppRole role) const;
    
    /**
     * @brief Đếm tổng số users trong hệ thống
     * @return Tổng số users
     */
    int getTotalUsers() const;
};
