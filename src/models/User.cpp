#include "models/User.h"
#include <algorithm>
#include <cctype>

User::User() 
    : IUser()
{
    role = AppRole::Guest;
}

User::User(const string& email, 
           const string& passwordHash,
           const string& fullName,
           const string& birthDate,
           const string& phone,
           time_t registeredAt,
           AppRole role,
           UserStatus status)
    : IUser(email, passwordHash, fullName, birthDate, phone, 
            registeredAt, role, status)
{}

// Note: getRoleString(), getStatusString(), parseRole(), parseStatus()
// are now inherited from IUser base class
