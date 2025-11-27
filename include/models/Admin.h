#pragma once
#include "models/IUser.h"

class Admin : public IUser {
private:
    string adminLevel;
    time_t lastLogin;

public:
    Admin() : IUser(), lastLogin(0) {
        role = AppRole::Admin;
        adminLevel = "normal";
    }
    
    Admin(const string& email, const string& passwordHash,
          const string& fullName, const string& birthDate,
          const string& phone, time_t registeredAt,
          UserStatus status = UserStatus::Active,
          const string& level = "normal")
        : IUser(email, passwordHash, fullName, birthDate, phone, 
                registeredAt, AppRole::Admin, status),
          adminLevel(level), lastLogin(0) {}
    
    string getUserType() const override {
        return "Admin";
    }
    
    bool hasPermission(const string&) const override {
        return true;
    }
    
    string getPermissions() const override {
        return "Toàn quyền quản trị: Quản lý người dùng, Quản lý phim, Quản lý suất chiếu, "
               "Quản lý voucher, Quản lý combo, Xem báo cáo, Phân quyền người dùng";
    }

    string getAdminLevel() const { return adminLevel; }
    void setAdminLevel(const string& level) { adminLevel = level; }
    bool isSuperAdmin() const { return adminLevel == "super"; }
    time_t getLastLogin() const { return lastLogin; }
    void updateLastLogin() { lastLogin = time(nullptr); }
    bool canManageUser(AppRole targetRole) const {
        if (isSuperAdmin()) return true;

        if (targetRole == AppRole::Admin) return false;
        
        return true;
    }
    bool canChangeRole(AppRole currentRole, AppRole newRole) const {
        if (isSuperAdmin()) return true;
        
        if (newRole == AppRole::Admin || currentRole == AppRole::Admin) {
            return false;
        }
        
        return true;
    }
};
