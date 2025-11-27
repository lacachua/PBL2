#include "services/VoucherService.h"
#include "utils/StringUtils.h"
#include "utils/DateTimeUtils.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <map>

using namespace std;

VoucherService::VoucherService(const string& defPath, const string& walletPath, const string& usersPath)
    : definitionPath(defPath), walletPath(walletPath), usersPath(usersPath) {
    loadDefinitions();
    loadAutoProvisionConfigs();
}

VoucherService::~VoucherService() {
    AppEventSystem::getInstance().unsubscribe(shared_from_this());
}

void VoucherService::initialize() {
    AppEventSystem::getInstance().subscribe(shared_from_this());
}

// ===== Data Loading/Saving =====

void VoucherService::loadDefinitions() {
    definitions.clear();
    voucherLookup.clear();
    
    ifstream file(definitionPath);
    if (!file.is_open()) return;
    
    string line;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        if (cols.size() < 5) continue;
        
        // Skip header
        if (isFirstLine && StringUtils::isHeaderRow(cols, "code")) {
            isFirstLine = false;
            continue;
        }
        isFirstLine = false;
        
        VoucherDef def;
        def.code = cols[0];
        def.type = stoi(cols[1]);
        def.value = stod(cols[2]);
        def.minBill = stod(cols[3]);
        def.description = cols.size() > 4 ? cols[4] : "";
        
        definitions.push_back(def);
        voucherLookup[def.code] = def;
    }
}

void VoucherService::saveDefinitions() {
    ofstream file(definitionPath);
    if (!file.is_open()) return;
    
    // Header
    file << "code|type|value|minBill|description\n";
    
    for (const auto& def : definitions) {
        file << def.code << "|" << def.type << "|" << def.value << "|" 
             << def.minBill << "|" << def.description << "\n";
    }
}

void VoucherService::loadAutoProvisionConfigs() {
    autoProvisionConfigs.clear();
    
    string configPath = "../data/voucher_auto_provision.txt";
    ifstream file(configPath);
    if (!file.is_open()) return;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        if (cols.size() < 4) continue;
        
        // Skip header
        if (StringUtils::isHeaderRow(cols, "code") || StringUtils::isHeaderRow(cols, "voucherCode")) continue;
        
        AutoProvisionConfig config;
        config.voucherCode = cols[0];
        config.daysToExpire = stoi(cols[1]);
        config.quantity = stoi(cols[2]);
        config.enabled = (cols[3] == "1" || cols[3] == "true");
        
        autoProvisionConfigs.push_back(config);
    }
}

void VoucherService::saveAutoProvisionConfigs() {
    string configPath = "../data/voucher_auto_provision.txt";
    ofstream file(configPath);
    if (!file.is_open()) return;
    
    file << "code|daysToExpire|quantity|enabled\n";
    
    for (const auto& config : autoProvisionConfigs) {
        file << config.voucherCode << "|" << config.daysToExpire << "|" 
             << config.quantity << "|" << (config.enabled ? "1" : "0") << "\n";
    }
}

void VoucherService::reload() {
    loadDefinitions();
    loadAutoProvisionConfigs();
}

// ===== Voucher Definition Operations =====

vector<VoucherDef> VoucherService::getAllDefinitions() const {
    return definitions;
}

const VoucherDef* VoucherService::getDefinition(const string& code) const {
    auto it = voucherLookup.find(code);
    if (it != voucherLookup.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool VoucherService::addVoucherDefinition(const VoucherDef& def) {
    // Check for duplicate code
    for (const auto& existing : definitions) {
        if (existing.code == def.code) {
            return false;
        }
    }
    
    definitions.push_back(def);
    voucherLookup[def.code] = def;
    saveDefinitions();
    
    // Publish event
    AppEventSystem::getInstance().publish(VoucherCreatedEvent(def.code));
    
    return true;
}

bool VoucherService::updateVoucherDefinition(const VoucherDef& def) {
    for (auto& existing : definitions) {
        if (existing.code == def.code) {
            existing.type = def.type;
            existing.value = def.value;
            existing.minBill = def.minBill;
            existing.description = def.description;
            voucherLookup[def.code] = existing;
            saveDefinitions();
            return true;
        }
    }
    return false;
}

bool VoucherService::deleteVoucherDefinition(const string& code) {
    auto it = find_if(definitions.begin(), definitions.end(),
        [&code](const VoucherDef& def) { return def.code == code; });
    
    if (it == definitions.end()) return false;
    
    definitions.erase(it);
    voucherLookup.erase(code);
    saveDefinitions();
    
    // Also remove from user wallets
    vector<string> lines;
    ifstream file(walletPath);
    string line;
    while (getline(file, line)) {
        auto cols = StringUtils::split(line, '|');
        if (cols.size() >= 2 && cols[1] != code) {
            lines.push_back(line);
        }
    }
    file.close();
    
    ofstream out(walletPath);
    for (const auto& l : lines) {
        out << l << "\n";
    }
    
    // Remove from auto-provision
    removeAutoProvision(code);
    
    return true;
}

// ===== User Wallet Operations (from VoucherManager) =====

vector<VoucherDisplay> VoucherService::getVouchersByUser(const string& email) {
    vector<VoucherDisplay> result;
    vector<UserVoucher> wallet;
    loadWallet(wallet);
    
    int today = DateTimeUtils::getTodayAsNumber();

    for (const auto& voucher : wallet) {
        if (voucher.email != email) continue;
        
        // Skip expired or depleted vouchers
        if (DateTimeUtils::dateStringToNumber(voucher.expiry) < today || voucher.quantity <= 0) continue;
        
        auto it = voucherLookup.find(voucher.code);
        if (it == voucherLookup.end()) continue;

        VoucherDisplay display;
        display.code = voucher.code;
        display.description = it->second.description;
        display.value = it->second.value;
        display.type = it->second.type;
        display.status = voucher.status;
        display.expiry = voucher.expiry;
        display.quantity = voucher.quantity;
        result.push_back(display);
    }

    return result;
}

double VoucherService::applyVoucher(const string& email, const string& code, 
                                     double totalBill, bool consume) {
    vector<UserVoucher> wallet;
    loadWallet(wallet);
    
    bool updated = false;
    double discount = 0.0;
    int today = DateTimeUtils::getTodayAsNumber();

    for (auto& voucher : wallet) {
        if (voucher.email != email || voucher.code != code) continue;
        if (voucher.status != 1) break;
        if (DateTimeUtils::dateStringToNumber(voucher.expiry) < today) break;
        if (voucher.quantity <= 0) break;

        auto defIt = voucherLookup.find(code);
        if (defIt == voucherLookup.end()) break;
        
        const auto& def = defIt->second;
        if (totalBill < def.minBill) break;

        if (def.type == 1) {
            // Fixed amount discount
            discount = min(def.value, totalBill);
        } else {
            // Percentage discount
            discount = min(totalBill, totalBill * def.value / 100.0);
        }

        if (discount > 0.0 && consume) {
            voucher.quantity--;
            updated = true;
        }
        break;
    }

    if (updated) {
        saveWallet(wallet);
    }

    return discount;
}

void VoucherService::loadWallet(vector<UserVoucher>& wallet) const {
    wallet.clear();
    ifstream file(walletPath);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto cols = StringUtils::split(line, '|');
        if (cols.size() < 4) continue;

        if (StringUtils::isHeaderRow(cols, "email")) continue;

        UserVoucher voucher;
        voucher.email = cols[0];
        voucher.code = cols[1];
        voucher.status = stoi(cols[2]);
        voucher.expiry = cols[3];
        voucher.quantity = (cols.size() >= 5) ? stoi(cols[4]) : 1;
        wallet.push_back(voucher);
    }
}

void VoucherService::saveWallet(const vector<UserVoucher>& wallet) const {
    ofstream file(walletPath, ios::trunc);
    if (!file.is_open()) return;
    
    file << "email|code|status|expiry_date|quantity\n";
    for (const auto& voucher : wallet) {
        file << voucher.email << '|' << voucher.code << '|' << voucher.status 
             << '|' << voucher.expiry << '|' << voucher.quantity << '\n';
    }
}

// ===== Distribution Operations =====

bool VoucherService::giveVoucherToUser(const string& email, const string& voucherCode,
                                        int daysToExpire, int quantity) {
    // Check if voucher exists
    bool found = false;
    for (const auto& def : definitions) {
        if (def.code == voucherCode) {
            found = true;
            break;
        }
    }
    if (!found) return false;
    
    string expiry = DateTimeUtils::buildExpiryDate(daysToExpire);
    
    // Check if user already has this voucher
    vector<string> lines;
    ifstream file(walletPath);
    string line;
    bool updated = false;
    bool isFirst = true;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        if (isFirst && cols.size() > 0 && StringUtils::isHeaderRow(cols, "email")) {
            lines.push_back(line);
            isFirst = false;
            continue;
        }
        isFirst = false;
        
        if (cols.size() >= 5 && cols[0] == email && cols[1] == voucherCode && cols[2] == "1") {
            // Update existing entry
            int existingQty = stoi(cols[4]);
            cols[4] = to_string(existingQty + quantity);
            cols[3] = expiry; // Update expiry
            
            string newLine = cols[0];
            for (size_t i = 1; i < cols.size(); i++) {
                newLine += "|" + cols[i];
            }
            lines.push_back(newLine);
            updated = true;
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    // Add new entry if not updated
    if (!updated) {
        string newLine = email + "|" + voucherCode + "|1|" + expiry + "|" + to_string(quantity);
        lines.push_back(newLine);
    }
    
    ofstream out(walletPath);
    for (const auto& l : lines) {
        out << l << "\n";
    }
    
    return true;
}

int VoucherService::giveVoucherToUsers(const set<string>& emails, const string& voucherCode,
                                       int daysToExpire, int quantity) {
    int count = 0;
    for (const auto& email : emails) {
        if (giveVoucherToUser(email, voucherCode, daysToExpire, quantity)) {
            count++;
        }
    }
    return count;
}

int VoucherService::distributeVoucherToAllActiveUsers(const string& voucherCode,
                                                       int daysToExpire, int quantity) {
    auto users = getActiveUsers();
    int count = 0;
    for (const auto& user : users) {
        if (giveVoucherToUser(user.first, voucherCode, daysToExpire, quantity)) {
            count++;
        }
    }
    return count;
}

bool VoucherService::removeVoucherFromUser(const string& email, const string& voucherCode) {
    vector<string> lines;
    ifstream file(walletPath);
    string line;
    bool removed = false;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        if (cols.size() >= 2 && cols[0] == email && cols[1] == voucherCode) {
            removed = true;
            continue; // Skip this line
        }
        lines.push_back(line);
    }
    file.close();
    
    ofstream out(walletPath);
    for (const auto& l : lines) {
        out << l << "\n";
    }
    
    return removed;
}

// ===== Auto-Provision Configuration =====

void VoucherService::setAutoProvision(const AutoProvisionConfig& config) {
    // Remove existing config for this voucher
    removeAutoProvision(config.voucherCode);
    
    autoProvisionConfigs.push_back(config);
    saveAutoProvisionConfigs();
}

void VoucherService::removeAutoProvision(const string& voucherCode) {
    auto it = find_if(autoProvisionConfigs.begin(), autoProvisionConfigs.end(),
        [&voucherCode](const AutoProvisionConfig& c) { return c.voucherCode == voucherCode; });
    
    if (it != autoProvisionConfigs.end()) {
        autoProvisionConfigs.erase(it);
        saveAutoProvisionConfigs();
    }
}

vector<AutoProvisionConfig> VoucherService::getAutoProvisionConfigs() const {
    return autoProvisionConfigs;
}

bool VoucherService::isAutoProvisionEnabled(const string& voucherCode) const {
    for (const auto& config : autoProvisionConfigs) {
        if (config.voucherCode == voucherCode && config.enabled) {
            return true;
        }
    }
    return false;
}

// ===== Query Operations =====

vector<pair<string, string>> VoucherService::getActiveUsers() const {
    vector<pair<string, string>> users;
    
    ifstream file(usersPath);
    if (!file.is_open()) return users;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        // Skip header
        if (cols.size() >= 1 && StringUtils::isHeaderRow(cols, "email")) continue;
        
        // Format: email|password|fullName|phone|role|status
        if (cols.size() >= 6) {
            string status = cols[5];
            // Only include active users (status == "1" or "active")
            if (status == "1" || status == "active") {
                string email = cols[0];
                string fullName = cols.size() > 2 ? cols[2] : "";
                users.push_back({email, fullName});
            }
        }
    }
    
    return users;
}

vector<VoucherService::UserVoucherInfo> VoucherService::getUsersWithVoucher(const string& voucherCode) const {
    vector<UserVoucherInfo> result;
    
    // Load user names
    map<string, string> userNames;
    ifstream usersFile(usersPath);
    string line;
    while (getline(usersFile, line)) {
        auto cols = StringUtils::split(line, '|');
        if (cols.size() >= 3 && cols[0] != "email") {
            userNames[cols[0]] = cols[2];
        }
    }
    usersFile.close();
    
    // Load wallet entries
    ifstream walletFile(walletPath);
    while (getline(walletFile, line)) {
        auto cols = StringUtils::split(line, '|');
        // email|code|status|expiry|quantity
        if (cols.size() >= 5 && cols[1] == voucherCode && cols[2] == "1") {
            UserVoucherInfo info;
            info.email = cols[0];
            info.fullName = userNames.count(cols[0]) ? userNames[cols[0]] : "";
            info.quantity = stoi(cols[4]);
            info.expiryDate = cols[3];
            result.push_back(info);
        }
    }
    
    return result;
}

int VoucherService::countUsersWithVoucher(const string& voucherCode) const {
    return static_cast<int>(getUsersWithVoucher(voucherCode).size());
}

// ===== Maintenance =====

void VoucherService::cleanupExpiredVouchers() {
    int today = DateTimeUtils::getTodayAsNumber();
    
    vector<string> lines;
    ifstream file(walletPath);
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        auto cols = StringUtils::split(line, '|');
        // Keep header
        if (cols.size() > 0 && StringUtils::isHeaderRow(cols, "email")) {
            lines.push_back(line);
            continue;
        }
        
        // email|code|status|expiry|quantity
        if (cols.size() >= 4) {
            int expiry = DateTimeUtils::dateStringToNumber(cols[3]);
            if (expiry >= today || cols[2] == "0") {
                lines.push_back(line);
            }
            // Expired and status=1 entries are removed
        }
    }
    file.close();
    
    ofstream out(walletPath);
    for (const auto& l : lines) {
        out << l << "\n";
    }
}

// ===== Observer Interface =====

void VoucherService::onAppEvent(const AppEvent& event) {
    if (event.getType() == "UserRegistered") {
        handleUserRegistered(static_cast<const UserRegisteredEvent&>(event));
    }
}

vector<string> VoucherService::getSubscribedEvents() const {
    return {"UserRegistered"};
}

void VoucherService::handleUserRegistered(const UserRegisteredEvent& event) {
    // Auto-provision vouchers to new user
    for (const auto& config : autoProvisionConfigs) {
        if (config.enabled) {
            giveVoucherToUser(event.email, config.voucherCode, config.daysToExpire, config.quantity);
            cout << "[VoucherService] Auto-provisioned voucher " << config.voucherCode 
                 << " to new user: " << event.email << endl;
        }
    }
}
