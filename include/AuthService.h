#pragma once
#include <string>
#include <ctime>
#include "HashTable.h"
#include "PasswordHasher.h"
#include "Validator.h"
using namespace std;

struct User {
    string username;      // Tên người dùng (từ phần đầu email)
    string email;
    string passwordHash;
    string fullName;
    string birthDate;
    string phone;
    time_t registeredAt;
    
    User() : registeredAt(0) {}
    User(const string& e, const string& h) 
        : email(e), passwordHash(h), registeredAt(time(nullptr)) {
        // Tự động tạo username từ email
        size_t atPos = e.find('@');
        username = (atPos != string::npos) ? e.substr(0, atPos) : e;
    }
};

class AuthService {
public:
    explicit AuthService(const string& filePath);
    ~AuthService();

    // Registration with validation
    bool registerUser(const string& email, const string& password, 
                     const string& fullName = "", const string& birthDate = "", 
                     const string& phone = "");

    // Login verification
    bool verify(const string& email, const string& password);

    // User management
    User* getUser(const string& email);
    bool emailExists(const string& email);
    
    // Load/Save
    void loadUsers();
    void saveUsers();
    
    // Create sample user for testing
    void ensureSampleUser();

private:
    string filePath;
    HashTable<string, User> userByEmail;  // email -> User (O(1) lookup)
};
