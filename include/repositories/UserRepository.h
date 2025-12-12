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

    string normalizeEmail(const string& email) const;
    User parseLine(const string& line) const;
    string serializeLine(const User& user) const;

public:
    UserRepository(const string& filepath = "../data/users.txt");
    ~UserRepository();

    void loadFromFile();
    void saveToFile();

    bool exists(const string& email) const;
    User* findByEmail(const string& email);
    bool addUser(const User& user);
    bool updateUser(const User& user);
    bool lockUser(const string& email);
    bool unlockUser(const string& email);
    bool deleteUser(const string& email);

    vector<User> getAllUsers() const;
    vector<User> getUsersByRole(AppRole role) const;
    int countByRole(AppRole role) const;
    int getTotalUsers() const;
};
