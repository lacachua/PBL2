#include "AuthService.h"
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;

AuthService::AuthService(const string& filePath) : filePath(filePath) {
    namespace fs = filesystem;
    fs::path p(filePath);
    fs::create_directories(p.parent_path());
    
    loadUsers();
}

AuthService::~AuthService() {
    saveUsers();
}

void AuthService::loadUsers() {
    ifstream file(filePath);
    if (!file.is_open()) return;
    
    string line;
    // Skip header if exists
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        User user;
        
        string regTimeStr;
        getline(ss, user.email, ',');
        getline(ss, user.passwordHash, ',');
        getline(ss, user.fullName, ',');
        getline(ss, user.birthDate, ',');
        getline(ss, user.phone, ',');
        getline(ss, regTimeStr, ',');
        
        if (!user.email.empty() && !user.passwordHash.empty()) {
            try {
                user.registeredAt = stoll(regTimeStr);
            } catch (...) {
                user.registeredAt = time(nullptr);
            }
            
            userByEmail.insert(user.email, user);
        }
    }
    
    file.close();
}

void AuthService::saveUsers() {
    ofstream file(filePath, ios::out | ios::trunc);
    if (!file.is_open()) return;
    
    // Write header
    file << "email,passwordHash,fullName,birthDate,phone,registeredAt\n";
    
    // Write all users
    userByEmail.forEach([&](const string& email, User& user) {
        file << user.email << ","
             << user.passwordHash << ","
             << user.fullName << ","
             << user.birthDate << ","
             << user.phone << ","
             << user.registeredAt << "\n";
    });
    
    file.close();
}

bool AuthService::registerUser(const string& email, const string& password,
                               const string& fullName, const string& birthDate,
                               const string& phone) {
    // Validate inputs
    if (email.empty() || password.empty()) return false;
    if (!Validator::isValidEmail(email)) return false;
    if (!Validator::isStrongPassword(password)) return false;
    
    if (!phone.empty() && !Validator::isValidPhone(phone)) return false;
    if (!birthDate.empty() && !Validator::isValidDate(birthDate)) return false;
    
    // Check if email already exists
    if (emailExists(email)) return false;
    
    // Create new user
    User newUser;
    newUser.email = email;
    newUser.passwordHash = PasswordHasher::hashPassword(password);
    newUser.fullName = fullName;
    newUser.birthDate = birthDate;
    newUser.phone = phone;
    newUser.registeredAt = time(nullptr);
    
    // Insert into hash table
    userByEmail.insert(email, newUser);
    
    // Save to file
    saveUsers();
    
    return true;
}

bool AuthService::verify(const string& email, const string& password) {
    // Find user
    User* user = userByEmail.find(email);
    if (!user) {
        return false;
    }
    
    // Verify password
    return PasswordHasher::verifyPassword(password, user->passwordHash);
}

User* AuthService::getUser(const string& email) {
    return userByEmail.find(email);
}

bool AuthService::emailExists(const string& email) {
    return userByEmail.exists(email);
}

void AuthService::ensureSampleUser() {
    if (!emailExists("test@gmail.com")) {
        registerUser("test@gmail.com", "Test1234", "Test User", "01/01/2000", "0901234567");
    }
}