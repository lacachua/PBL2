#pragma once
#include <string>
#include <ctime>
#include <memory>
#include "models/User.h"
#include "services/UserRepository.h"
#include "services/VoucherManager.h"
#include "utils/PasswordHasher.h"
#include "utils/Validator.h"
#include "core/AppRole.h"
using namespace std;

/**
 * @brief Service xử lý authentication và authorization
 * 
 * Responsibility (Single Responsibility Principle):
 * - Xác thực đăng nhập (authentication)
 * - Kiểm tra quyền truy cập (authorization)
 * - Quản lý session hiện tại
 * - Validate password và user input
 * 
 * Dependencies:
 * - UserRepository: CRUD operations
 * - PasswordHasher: Hash/verify passwords
 * - Validator: Validate inputs
 * 
 * Note: Email là khóa chính - IMMUTABLE
 */
class AuthService {
public:
    /**
     * @brief Constructor
     * @param filePath Path to users.txt
     */
    explicit AuthService(const string& filePath);
    ~AuthService();

    // ===== AUTHENTICATION =====
    
    /**
     * @brief Đăng ký user mới
     * @param email Email (primary key - IMMUTABLE)
     * @param password Plain password (will be hashed)
     * @param fullName Họ tên đầy đủ
     * @param birthDate Ngày sinh (dd/mm/yyyy)
     * @param phone Số điện thoại
     * @param role Vai trò (mặc định Customer)
     * @return true nếu thành công, false nếu email đã tồn tại
     */
    bool registerUser(const string& email, 
                     const string& password, 
                     const string& fullName = "", 
                     const string& birthDate = "", 
                     const string& phone = "",
                     AppRole role = AppRole::Customer);

    /**
     * @brief Verify credentials (không login)
     * @param email Email
     * @param password Plain password
     * @return true nếu đúng
     */
    bool verify(const string& email, const string& password);
    
    /**
     * @brief Login với role-based access control
     * @param email Email
     * @param password Plain password
     * @return true nếu thành công, false nếu sai password hoặc bị locked
     */
    bool login(const string& email, const string& password);
    
    /**
     * @brief Logout và clear session
     */
    void logout();
    
    /**
     * @brief Kiểm tra đã đăng nhập chưa
     */
    bool isLoggedIn() const;
    
    /**
     * @brief Lấy email của user hiện tại
     */
    string getCurrentUserEmail() const;
    
    /**
     * @brief Lấy User object hiện tại
     * @return Pointer to User, hoặc nullptr nếu chưa login
     */
    User* getCurrentUser();

    // ===== AUTHORIZATION =====
    
    /**
     * @brief Kiểm tra user hiện tại có role cụ thể không
     * @param role Role cần kiểm tra
     * @return true nếu match
     */
    bool hasRole(AppRole role) const;
    
    /**
     * @brief Kiểm tra user hiện tại có quyền admin không
     */
    bool isAdmin() const;
    
    /**
     * @brief Kiểm tra user hiện tại có quyền staff hoặc cao hơn không
     */
    bool isStaffOrAbove() const;

    // ===== USER MANAGEMENT (Admin only) =====
    
    /**
     * @brief Lấy user theo email
     */
    User* getUser(const string& email);
    
    /**
     * @brief Kiểm tra email đã tồn tại chưa
     */
    bool emailExists(const string& email);
    
    /**
     * @brief Khóa tài khoản user
     * @param email Email của user cần khóa
     * @return true nếu thành công
     */
    bool lockUserAccount(const string& email);
    
    /**
     * @brief Mở khóa tài khoản user
     * @param email Email của user cần mở khóa
     * @return true nếu thành công
     */
    bool unlockUserAccount(const string& email);
    
    /**
     * @brief Đổi role của user (Admin only)
     * @param email Email của user
     * @param newRole Role mới
     * @return true nếu thành công
     */
    bool changeUserRole(const string& email, AppRole newRole);
    
    // ===== REPOSITORY ACCESS =====
    
    /**
     * @brief Lấy repository để thao tác trực tiếp (cẩn thận!)
     */
    UserRepository* getRepository() { return repository.get(); }
    
    // ===== UTILITIES =====
    
    /**
     * @brief Tạo admin mặc định nếu chưa có
     */
    void ensureSampleUser();
    
    /**
     * @brief Tạo admin và staff mẫu
     */
    void ensureDefaultAdminAndStaff();

private:
    unique_ptr<UserRepository> repository;
    unique_ptr<VoucherManager> voucherManager;
    
    // Current session
    string currentUserEmail;
    bool loggedIn;
};
