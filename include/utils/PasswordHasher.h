#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H

#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

class PasswordHasher {
private:
    // Simple hash function (for educational purposes)
    // In production, use bcrypt or Argon2
    static unsigned long djb2Hash(const string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
    
public:
    // Hash password with salt
    static string hashPassword(const string& password) {
        const string SALT = "PBL2_CINEMA_2025";  // Local constant
        string combined = password + SALT;
        unsigned long hash1 = djb2Hash(combined);
        unsigned long hash2 = djb2Hash(to_string(hash1) + SALT);
        
        stringstream ss;
        ss << hex << hash1 << hash2;
        return ss.str();
    }
    
    // Verify password against hash
    static bool verifyPassword(const string& password, const string& hash) {
        return hashPassword(password) == hash;
    }
    
    // Check password strength
    static int getPasswordStrength(const string& password) {
        int score = 0;
        
        // Length
        if (password.length() >= 8) score += 20;
        if (password.length() >= 12) score += 20;
        
        // Character types
        bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
        for (char c : password) {
            if (islower(c)) hasLower = true;
            if (isupper(c)) hasUpper = true;
            if (isdigit(c)) hasDigit = true;
            if (ispunct(c)) hasSpecial = true;
        }
        
        if (hasLower) score += 15;
        if (hasUpper) score += 15;
        if (hasDigit) score += 15;
        if (hasSpecial) score += 15;
        
        return score;  // 0-100
    }
    
    static string getStrengthLabel(int score) {
        if (score < 40) return "Yeu";
        if (score < 60) return "Trung binh";
        if (score < 80) return "Manh";
        return "Rat manh";
    }
};

#endif
