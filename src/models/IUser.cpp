#include "models/IUser.h"
#include <algorithm>
#include <cctype>

AppRole IUser::parseRole(const string& roleStr) {
    string lower = roleStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "admin") return AppRole::Admin;
    if (lower == "customer") return AppRole::Customer;
    return AppRole::Guest;
}

UserStatus IUser::parseStatus(const string& statusStr) {
    string lower = statusStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "locked") return UserStatus::Locked;
    return UserStatus::Active;
}
