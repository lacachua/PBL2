#include "services/UserRepository.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

UserRepository::UserRepository(const string& filepath) 
    : filePath(filepath) {
    // Ensure file exists
    namespace fs = filesystem;
    if (!fs::exists(filepath)) {
        ofstream file(filepath);
        file << "email|passwordHash|fullName|birthDate|phone|registeredAt|role|status\n";
        file.close();
    }
    loadFromFile();
}

UserRepository::~UserRepository() {
    saveToFile();
}

string UserRepository::normalizeEmail(const string& email) const {
    string normalized = email;
    transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    return normalized;
}

User UserRepository::parseLine(const string& line) const {
    stringstream ss(line);
    string email, passwordHash, fullName, birthDate, phone, registeredAtStr, roleStr, statusStr;
    
    getline(ss, email, '|');
    getline(ss, passwordHash, '|');
    getline(ss, fullName, '|');
    getline(ss, birthDate, '|');
    getline(ss, phone, '|');
    getline(ss, registeredAtStr, '|');
    getline(ss, roleStr, '|');
    getline(ss, statusStr, '|');
    
    time_t registeredAt = 0;
    try {
        registeredAt = stoll(registeredAtStr);
    } catch (...) {
        registeredAt = time(nullptr);
    }
    
    AppRole role = User::parseRole(roleStr);
    UserStatus status = User::parseStatus(statusStr);
    
    return User(email, passwordHash, fullName, birthDate, phone, registeredAt, role, status);
}

string UserRepository::serializeLine(const User& user) const {
    stringstream ss;
    ss << user.getEmail() << "|"
       << user.getPasswordHash() << "|"
       << user.getFullName() << "|"
       << user.getBirthDate() << "|"
       << user.getPhone() << "|"
       << user.getRegisteredAt() << "|"
       << user.getRoleString() << "|"
       << user.getStatusString();
    return ss.str();
}

void UserRepository::loadFromFile() {
    ifstream file(filePath);
    if (!file.is_open()) return;
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        try {
            User user = parseLine(line);
            string key = normalizeEmail(user.getEmail());
            users.insert(key, user);
        } catch (...) {
            // Skip invalid lines
            continue;
        }
    }
    
    file.close();
}

void UserRepository::saveToFile() {
    ofstream file(filePath);
    if (!file.is_open()) return;
    
    // Write header
    file << "email|passwordHash|fullName|birthDate|phone|registeredAt|role|status\n";
    
    // Write all users
    vector<User> allUsers = getAllUsers();
    for (const auto& user : allUsers) {
        file << serializeLine(user) << "\n";
    }
    
    file.close();
}

bool UserRepository::exists(const string& email) const {
    string key = normalizeEmail(email);
    return users.contains(key);
}

User* UserRepository::findByEmail(const string& email) {
    string key = normalizeEmail(email);
    return users.get(key);
}

bool UserRepository::addUser(const User& user) {
    string key = normalizeEmail(user.getEmail());
    
    if (exists(key)) {
        return false; // Email already exists
    }
    
    users.insert(key, user);
    return true;
}

bool UserRepository::updateUser(const User& user) {
    string key = normalizeEmail(user.getEmail());
    
    if (!exists(key)) {
        return false; // User not found
    }
    
    // Update existing user (email remains the same - PRIMARY KEY)
    users.insert(key, user);
    return true;
}

bool UserRepository::lockUser(const string& email) {
    User* user = findByEmail(email);
    if (!user) return false;
    
    user->lock();
    return true;
}

bool UserRepository::unlockUser(const string& email) {
    User* user = findByEmail(email);
    if (!user) return false;
    
    user->unlock();
    return true;
}

bool UserRepository::deleteUser(const string& email) {
    string key = normalizeEmail(email);
    
    if (!exists(key)) {
        return false;
    }
    
    users.remove(key);
    return true;
}

vector<User> UserRepository::getAllUsers() const {
    vector<User> result;
    
    // Use forEach to iterate through all users
    // Need to cast away const to use forEach (HashTable issue)
    auto& mutableUsers = const_cast<HashTable<string, User>&>(users);
    
    mutableUsers.forEach([&result](const string& key, User& user) {
        result.push_back(user);
    });
    
    return result;
}

vector<User> UserRepository::getUsersByRole(AppRole role) const {
    vector<User> allUsers = getAllUsers();
    vector<User> filtered;
    
    for (const auto& user : allUsers) {
        if (user.getRole() == role) {
            filtered.push_back(user);
        }
    }
    
    return filtered;
}

int UserRepository::countByRole(AppRole role) const {
    return getUsersByRole(role).size();
}

int UserRepository::getTotalUsers() const {
    return getAllUsers().size();
}
