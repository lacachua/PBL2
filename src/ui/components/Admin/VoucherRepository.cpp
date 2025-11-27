#include "UI/components/Admin/VoucherRepository.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

AdminVoucherRepository::AdminVoucherRepository(
    const string& defPath, 
    const string& walletPathValue,
    const string& usersPathValue
) : definitionPath(defPath), walletPath(walletPathValue), usersPath(usersPathValue) {
    reload();
}

void AdminVoucherRepository::reload() {
    loadDefinitions();
    loadUserVouchers();
}

// ===== VOUCHER DEFINITION CRUD =====

void AdminVoucherRepository::loadDefinitions() {
    definitions.clear();
    definitionLookup.clear();
    
    ifstream file(definitionPath);
    if (!file.is_open()) return;
    
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        vector<string> cols = splitString(line, '|');
        if (cols.size() < 5) continue;
        if (isHeaderRow(cols, "code")) continue;
        
        VoucherDefinition def;
        def.code = toUpperCase(trim(cols[0]));
        def.type = stoi(cols[1]);
        def.value = stod(cols[2]);
        def.minBill = stod(cols[3]);
        def.description = trim(cols[4]);
        
        definitions.push_back(def);
        definitionLookup[def.code] = def;
    }
}

void AdminVoucherRepository::saveDefinitions() {
    ofstream file(definitionPath, ios::trunc);
    if (!file.is_open()) return;
    
    file << "code|type|value|min_bill|description\n";
    for (const auto& def : definitions) {
        file << def.code << "|" 
             << def.type << "|" 
             << def.value << "|" 
             << def.minBill << "|" 
             << def.description << "\n";
    }
}

vector<VoucherDefinition> AdminVoucherRepository::getAllDefinitions() const {
    return definitions;
}

const VoucherDefinition* AdminVoucherRepository::findDefinition(const string& code) const {
    string upperCode = toUpperCase(trim(code));
    auto it = definitionLookup.find(upperCode);
    if (it != definitionLookup.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool AdminVoucherRepository::addDefinition(const VoucherDefinition& def) {
    string upperCode = toUpperCase(trim(def.code));
    
    if (definitionLookup.find(upperCode) != definitionLookup.end()) {
        return false; // Already exists
    }
    
    VoucherDefinition newDef = def;
    newDef.code = upperCode;
    
    definitions.push_back(newDef);
    definitionLookup[upperCode] = newDef;
    saveDefinitions();
    
    return true;
}

bool AdminVoucherRepository::updateDefinition(const VoucherDefinition& def) {
    string upperCode = toUpperCase(trim(def.code));
    
    auto it = definitionLookup.find(upperCode);
    if (it == definitionLookup.end()) {
        return false; // Not found
    }
    
    // Update in lookup
    it->second = def;
    it->second.code = upperCode;
    
    // Update in vector
    for (auto& d : definitions) {
        if (d.code == upperCode) {
            d = def;
            d.code = upperCode;
            break;
        }
    }
    
    saveDefinitions();
    return true;
}

bool AdminVoucherRepository::deleteDefinition(const string& code) {
    string upperCode = toUpperCase(trim(code));
    
    auto it = definitionLookup.find(upperCode);
    if (it == definitionLookup.end()) {
        return false;
    }
    
    definitionLookup.erase(it);
    
    definitions.erase(
        remove_if(definitions.begin(), definitions.end(), 
            [&upperCode](const VoucherDefinition& d) { return d.code == upperCode; }),
        definitions.end()
    );
    
    saveDefinitions();
    return true;
}

// ===== USER VOUCHER MANAGEMENT =====

void AdminVoucherRepository::loadUserVouchers() {
    userVouchers.clear();
    
    ifstream file(walletPath);
    if (!file.is_open()) return;
    
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        vector<string> cols = splitString(line, '|');
        if (cols.size() < 4) continue;
        if (isHeaderRow(cols, "email")) continue;
        
        UserVoucherEntry entry;
        entry.email = trim(cols[0]);
        entry.code = toUpperCase(trim(cols[1]));
        entry.status = stoi(cols[2]);
        entry.expiryDate = trim(cols[3]);
        entry.quantity = (cols.size() >= 5) ? stoi(cols[4]) : 1;
        
        userVouchers.push_back(entry);
    }
}

void AdminVoucherRepository::saveUserVouchers() {
    ofstream file(walletPath, ios::trunc);
    if (!file.is_open()) return;
    
    file << "email|code|status|expiry_date|quantity\n";
    for (const auto& entry : userVouchers) {
        file << entry.email << "|" 
             << entry.code << "|" 
             << entry.status << "|" 
             << entry.expiryDate << "|" 
             << entry.quantity << "\n";
    }
}

bool AdminVoucherRepository::giveVoucherToUser(
    const string& email, 
    const string& code, 
    int daysToExpire, 
    int quantity
) {
    string upperCode = toUpperCase(trim(code));
    string lowerEmail = trim(email);
    
    // Check if voucher definition exists
    if (findDefinition(upperCode) == nullptr) {
        return false;
    }
    
    string expiry = buildExpiryDate(daysToExpire);
    
    // Check if user already has this voucher
    for (auto& entry : userVouchers) {
        if (entry.email == lowerEmail && entry.code == upperCode) {
            // Add more quantity to existing voucher
            entry.quantity += quantity;
            entry.status = 1;
            entry.expiryDate = expiry; // Reset expiry date
            saveUserVouchers();
            return true;
        }
    }
    
    // Add new entry
    UserVoucherEntry newEntry(lowerEmail, upperCode, 1, expiry, quantity);
    userVouchers.push_back(newEntry);
    saveUserVouchers();
    
    return true;
}

int AdminVoucherRepository::giveVoucherToAllActiveUsers(
    const string& code, 
    int daysToExpire, 
    int quantity
) {
    vector<pair<string, string>> activeUsers = getActiveUsers();
    int count = 0;
    
    for (const auto& user : activeUsers) {
        if (giveVoucherToUser(user.first, code, daysToExpire, quantity)) {
            count++;
        }
    }
    
    return count;
}

bool AdminVoucherRepository::removeVoucherFromUser(const string& email, const string& code) {
    string upperCode = toUpperCase(trim(code));
    string lowerEmail = trim(email);
    
    auto it = remove_if(userVouchers.begin(), userVouchers.end(),
        [&](const UserVoucherEntry& entry) {
            return entry.email == lowerEmail && entry.code == upperCode;
        });
    
    if (it == userVouchers.end()) {
        return false;
    }
    
    userVouchers.erase(it, userVouchers.end());
    saveUserVouchers();
    return true;
}

vector<VoucherUserInfo> AdminVoucherRepository::getUsersWithVoucher(const string& code) const {
    vector<VoucherUserInfo> result;
    string upperCode = toUpperCase(trim(code));
    
    // Load user names from users.txt
    unordered_map<string, string> userNames;
    ifstream usersFile(usersPath);
    if (usersFile.is_open()) {
        string line;
        while (getline(usersFile, line)) {
            if (line.empty()) continue;
            
            vector<string> cols = splitString(line, '|');
            if (cols.size() >= 3 && !isHeaderRow(cols, "email")) {
                string email = trim(cols[0]);
                string name = trim(cols[2]);
                userNames[email] = name;
            }
        }
    }
    
    int today = todayAsNumber();
    
    for (const auto& entry : userVouchers) {
        if (entry.code == upperCode && entry.quantity > 0 && 
            dateStringToNumber(entry.expiryDate) >= today) {
            
            VoucherUserInfo info;
            info.email = entry.email;
            info.fullName = userNames.count(entry.email) > 0 ? 
                            userNames[entry.email] : entry.email;
            info.quantity = entry.quantity;
            info.expiryDate = entry.expiryDate;
            info.status = entry.status;
            
            result.push_back(info);
        }
    }
    
    return result;
}

vector<pair<string, string>> AdminVoucherRepository::getActiveUsers() const {
    vector<pair<string, string>> result;
    
    ifstream file(usersPath);
    if (!file.is_open()) return result;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        vector<string> cols = splitString(line, '|');
        if (cols.size() < 8) continue;
        if (isHeaderRow(cols, "email")) continue;
        
        string email = trim(cols[0]);
        string fullName = trim(cols[2]);
        string status = trim(cols[7]);
        
        // Only include active users with customer role
        if (status == "active") {
            result.push_back({email, fullName});
        }
    }
    
    return result;
}

int AdminVoucherRepository::countUsersWithVoucher(const string& code) const {
    return getUsersWithVoucher(code).size();
}

void AdminVoucherRepository::cleanupExpiredVouchers() {
    int today = todayAsNumber();
    bool changed = false;
    
    auto it = remove_if(userVouchers.begin(), userVouchers.end(),
        [today, &changed, this](const UserVoucherEntry& entry) {
            if (dateStringToNumber(entry.expiryDate) < today || entry.quantity <= 0) {
                changed = true;
                return true;
            }
            return false;
        });
    
    if (it != userVouchers.end()) {
        userVouchers.erase(it, userVouchers.end());
    }
    
    if (changed) {
        saveUserVouchers();
    }
}

// ===== HELPERS =====

vector<string> AdminVoucherRepository::splitString(const string& input, char delimiter) const {
    vector<string> tokens;
    string token;
    stringstream ss(input);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string AdminVoucherRepository::trim(const string& input) const {
    const char* whitespace = " \t\r\n";
    size_t start = input.find_first_not_of(whitespace);
    if (start == string::npos) return "";
    size_t end = input.find_last_not_of(whitespace);
    return input.substr(start, end - start + 1);
}

string AdminVoucherRepository::buildExpiryDate(int daysToExpire) const {
    using namespace chrono;
    auto now = system_clock::now();
    auto future = now + chrono::hours(24 * daysToExpire);
    time_t futureTime = system_clock::to_time_t(future);
    tm tmValue;
#if defined(_WIN32)
    localtime_s(&tmValue, &futureTime);
#else
    localtime_r(&futureTime, &tmValue);
#endif
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%Y%m%d", &tmValue);
    return string(buffer);
}

int AdminVoucherRepository::todayAsNumber() const {
    using namespace chrono;
    auto now = system_clock::now();
    time_t nowTime = system_clock::to_time_t(now);
    tm tmValue;
#if defined(_WIN32)
    localtime_s(&tmValue, &nowTime);
#else
    localtime_r(&nowTime, &tmValue);
#endif
    return (tmValue.tm_year + 1900) * 10000 + (tmValue.tm_mon + 1) * 100 + tmValue.tm_mday;
}

int AdminVoucherRepository::dateStringToNumber(const string& date) const {
    if (date.length() != 8) return 0;
    try {
        return stoi(date);
    } catch (...) {
        return 0;
    }
}

bool AdminVoucherRepository::isHeaderRow(const vector<string>& cols, const string& expectedFirstCell) const {
    if (cols.empty()) return false;
    string cell = trim(cols[0]);
    transform(cell.begin(), cell.end(), cell.begin(), [](unsigned char ch) {
        return static_cast<char>(tolower(ch));
    });
    string expected = expectedFirstCell;
    transform(expected.begin(), expected.end(), expected.begin(), [](unsigned char ch) {
        return static_cast<char>(tolower(ch));
    });
    return cell == expected;
}

string AdminVoucherRepository::toUpperCase(const string& input) const {
    string result = input;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return result;
}
