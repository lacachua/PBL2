#pragma once
#include <string>
#include <ctime>
#include "core/AppRole.h"

using namespace std;

class User {
private:
    string email;
    string passwordHash;
    string fullName;
    string birthDate;
    string phone;
    time_t registeredAt;
    AppRole role;
    UserStatus status;
public:
    User();
    User(const string& email, 
         const string& passwordHash,
         const string& fullName,
         const string& birthDate,
         const string& phone,
         time_t registeredAt,
         AppRole role = AppRole::Customer,
         UserStatus status = UserStatus::Active);

    string getEmail() const;
    string getPasswordHash() const;
    string getFullName() const;
    string getBirthDate() const;
    string getPhone() const;
    time_t getRegisteredAt() const;
    AppRole getRole() const;
    UserStatus getStatus() const;
    string getUsername() const;

    bool isActive() const;
    bool isLocked() const;

    void setPasswordHash(const string&);
    void setFullName(const string&);
    void setBirthDate(const string&);
    void setPhone(const string&);
    void setRole(AppRole);
    void setStatus(UserStatus);

    void lock();
    void unlock();

    string getRoleString() const;
    string getStatusString() const;
    static AppRole parseRole(const string& roleStr);
    static UserStatus parseStatus(const string& statusStr);
};
