#pragma once
#include <memory>
#include <string>
#include "models/IUser.h"
#include "models/Guest.h"
#include "models/Customer.h"
#include "models/Admin.h"

using namespace std;

/**
 * @brief Factory class để tạo các đối tượng User
 * 
 * Design Pattern: Factory Method Pattern
 * - Tập trung logic tạo object vào một nơi
 * - Dễ dàng mở rộng thêm loại user mới
 * - Client code không cần biết concrete class
 * 
 * Usage:
 *   auto user = UserFactory::createUser(AppRole::Customer, email, ...);
 *   auto guest = UserFactory::createGuest();
 */
class UserFactory {
public:
    /**
     * @brief Tạo user mới theo role
     * @param role Vai trò của user
     * @param email Email (primary key)
     * @param passwordHash Mật khẩu đã hash
     * @param fullName Họ tên
     * @param birthDate Ngày sinh
     * @param phone Số điện thoại
     * @param registeredAt Thời điểm đăng ký
     * @param status Trạng thái tài khoản
     * @return unique_ptr<IUser> pointing to the correct derived class
     */
    static unique_ptr<IUser> createUser(
        AppRole role,
        const string& email,
        const string& passwordHash,
        const string& fullName,
        const string& birthDate,
        const string& phone,
        time_t registeredAt,
        UserStatus status = UserStatus::Active
    ) {
        switch (role) {
            case AppRole::Admin:
                return make_unique<Admin>(email, passwordHash, fullName, 
                                          birthDate, phone, registeredAt, status);
            
            case AppRole::Customer:
                return make_unique<Customer>(email, passwordHash, fullName, 
                                             birthDate, phone, registeredAt, status);
            
            case AppRole::Guest:
            default:
                return make_unique<Guest>(email, passwordHash, fullName, 
                                          birthDate, phone, registeredAt, status);
        }
    }
    
    /**
     * @brief Tạo Guest mặc định (chưa đăng nhập)
     * @return unique_ptr<IUser> pointing to Guest
     */
    static unique_ptr<IUser> createGuest() {
        return make_unique<Guest>();
    }
    
    /**
     * @brief Tạo Customer mới (đăng ký)
     * @param email Email
     * @param passwordHash Mật khẩu đã hash
     * @param fullName Họ tên
     * @param birthDate Ngày sinh
     * @param phone Số điện thoại
     * @return unique_ptr<IUser> pointing to Customer
     */
    static unique_ptr<IUser> createCustomer(
        const string& email,
        const string& passwordHash,
        const string& fullName,
        const string& birthDate,
        const string& phone
    ) {
        return make_unique<Customer>(email, passwordHash, fullName, 
                                     birthDate, phone, time(nullptr));
    }
    
    /**
     * @brief Tạo Admin mới
     * @param email Email
     * @param passwordHash Mật khẩu đã hash
     * @param fullName Họ tên
     * @param birthDate Ngày sinh
     * @param phone Số điện thoại
     * @param adminLevel Cấp độ admin (optional)
     * @return unique_ptr<IUser> pointing to Admin
     */
    static unique_ptr<IUser> createAdmin(
        const string& email,
        const string& passwordHash,
        const string& fullName,
        const string& birthDate,
        const string& phone,
        const string& adminLevel = "normal"
    ) {
        return make_unique<Admin>(email, passwordHash, fullName, 
                                  birthDate, phone, time(nullptr), 
                                  UserStatus::Active, adminLevel);
    }
    
    /**
     * @brief Clone một user (deep copy với đúng type)
     * @param user User cần clone
     * @return unique_ptr<IUser> pointing to cloned user
     */
    static unique_ptr<IUser> clone(const IUser& user) {
        return createUser(
            user.getRole(),
            user.getEmail(),
            user.getPasswordHash(),
            user.getFullName(),
            user.getBirthDate(),
            user.getPhone(),
            user.getRegisteredAt(),
            user.getStatus()
        );
    }
};
