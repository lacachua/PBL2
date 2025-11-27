#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <unordered_map>
#include "services/EventSystem.h"
#include "utils/StringUtils.h"
#include "utils/DateTimeUtils.h"

/**
 * @brief Data structure for voucher definition
 * Follows Single Responsibility: Only holds voucher definition data
 */
struct VoucherDef {
    std::string code;
    int type = 0;          // 1: fixed amount, 2: percentage
    double value = 0.0;
    double minBill = 0.0;
    std::string description;
};

/**
 * @brief Data structure for user's voucher in wallet
 */
struct UserVoucher {
    std::string email;
    std::string code;
    int status = 0;        // 1: usable, 0: used/expired
    std::string expiry;    // YYYYMMDD
    int quantity = 1;
};

/**
 * @brief Data Transfer Object for displaying voucher info to user
 */
struct VoucherDisplay {
    std::string code;
    std::string description;
    double value = 0.0;
    int type = 0;
    int status = 0;
    std::string expiry;
    int quantity = 1;
};

/**
 * @brief Configuration for auto-provisioning vouchers to new users
 */
struct AutoProvisionConfig {
    std::string voucherCode;
    int daysToExpire = 30;
    int quantity = 1;
    bool enabled = true;
};

/**
 * @brief Unified service class for ALL voucher operations
 * 
 * This service consolidates VoucherManager functionality and provides:
 * - Voucher definition management (CRUD)
 * - User wallet operations (give, apply, get)
 * - Mass voucher distribution
 * - Auto-provisioning vouchers to new users (Observer pattern)
 * 
 * Design Principles Applied:
 * - Single Responsibility: Only handles voucher-related operations
 * - Open/Closed: Can extend functionality without modifying existing code
 * - Dependency Inversion: Uses utility abstractions (StringUtils, DateTimeUtils)
 * - Interface Segregation: Implements only required IAppEventObserver methods
 */
class VoucherService : public IAppEventObserver, public std::enable_shared_from_this<VoucherService> {
public:
    VoucherService(const std::string& defPath = "../data/voucher_defs.txt",
                   const std::string& walletPath = "../data/user_wallets.txt",
                   const std::string& usersPath = "../data/users.txt");
    
    ~VoucherService() override;
    
    /**
     * @brief Initialize the service and register with EventSystem
     * Call this after creating the service with std::make_shared
     */
    void initialize();
    
    // ===== Voucher Definition Operations (Admin) =====
    
    /**
     * @brief Get all voucher definitions
     * @return Vector of VoucherDef
     */
    std::vector<VoucherDef> getAllDefinitions() const;
    
    /**
     * @brief Get voucher definition by code
     * @param code Voucher code
     * @return Pointer to VoucherDef or nullptr if not found
     */
    const VoucherDef* getDefinition(const std::string& code) const;
    
    /**
     * @brief Add a new voucher definition
     * @return true if successful
     */
    bool addVoucherDefinition(const VoucherDef& def);
    
    /**
     * @brief Update an existing voucher definition
     * @return true if successful
     */
    bool updateVoucherDefinition(const VoucherDef& def);
    
    /**
     * @brief Delete a voucher definition and all user vouchers with this code
     * @return true if successful
     */
    bool deleteVoucherDefinition(const std::string& code);
    
    // ===== User Wallet Operations (Customer-facing) =====
    
    /**
     * @brief Give voucher to a single user
     * @param email User's email
     * @param voucherCode The voucher code
     * @param daysToExpire Number of days until expiration
     * @param quantity Number of vouchers
     * @return true if successful
     */
    bool giveVoucherToUser(const std::string& email, const std::string& voucherCode,
                           int daysToExpire, int quantity = 1);
    
    /**
     * @brief Get all vouchers owned by a user (for display)
     * @param email User's email
     * @return Vector of VoucherDisplay
     */
    std::vector<VoucherDisplay> getVouchersByUser(const std::string& email);
    
    /**
     * @brief Apply a voucher to a purchase
     * @param email User's email
     * @param code Voucher code
     * @param totalBill Current bill amount
     * @param consume If true, deduct voucher quantity
     * @return Discount amount (0 if invalid/not applicable)
     */
    double applyVoucher(const std::string& email, const std::string& code, 
                        double totalBill, bool consume = true);
    
    // ===== Distribution Operations (Admin) =====
    
    /**
     * @brief Give voucher to multiple users at once
     * @param emails Set of user emails
     * @param voucherCode The voucher code
     * @param daysToExpire Number of days until expiration
     * @param quantity Number of vouchers
     * @return Number of users that received the voucher
     */
    int giveVoucherToUsers(const std::set<std::string>& emails, const std::string& voucherCode,
                          int daysToExpire, int quantity = 1);
    
    /**
     * @brief Give voucher to all active users
     * @param voucherCode The voucher code
     * @param daysToExpire Number of days until expiration
     * @param quantity Number of vouchers per user
     * @return Number of users that received the voucher
     */
    int distributeVoucherToAllActiveUsers(const std::string& voucherCode,
                                          int daysToExpire, int quantity = 1);
    
    /**
     * @brief Remove voucher from a user
     * @return true if successful
     */
    bool removeVoucherFromUser(const std::string& email, const std::string& voucherCode);
    
    // ===== Auto-Provision Configuration =====
    
    /**
     * @brief Set a voucher for auto-provisioning to new users
     * @param config The auto-provision configuration
     */
    void setAutoProvision(const AutoProvisionConfig& config);
    
    /**
     * @brief Remove auto-provision for a voucher
     * @param voucherCode The voucher code
     */
    void removeAutoProvision(const std::string& voucherCode);
    
    /**
     * @brief Get all auto-provision configurations
     * @return Vector of AutoProvisionConfig
     */
    std::vector<AutoProvisionConfig> getAutoProvisionConfigs() const;
    
    /**
     * @brief Check if a voucher is set for auto-provision
     * @param voucherCode The voucher code
     * @return true if auto-provision is enabled
     */
    bool isAutoProvisionEnabled(const std::string& voucherCode) const;
    
    // ===== Query Operations =====
    
    /**
     * @brief Get list of active users (email, fullName)
     * @return Vector of pairs (email, fullName)
     */
    std::vector<std::pair<std::string, std::string>> getActiveUsers() const;
    
    /**
     * @brief Get users who have a specific voucher
     * @param voucherCode The voucher code
     * @return Vector of user info structs
     */
    struct UserVoucherInfo {
        std::string email;
        std::string fullName;
        int quantity;
        std::string expiryDate;
    };
    std::vector<UserVoucherInfo> getUsersWithVoucher(const std::string& voucherCode) const;
    
    /**
     * @brief Count users who have a specific voucher
     * @param voucherCode The voucher code
     * @return Number of users
     */
    int countUsersWithVoucher(const std::string& voucherCode) const;
    
    // ===== Maintenance =====
    
    /**
     * @brief Clean up expired vouchers
     */
    void cleanupExpiredVouchers();
    
    /**
     * @brief Reload data from files
     */
    void reload();
    
    // ===== Observer Interface =====
    
    void onAppEvent(const AppEvent& event) override;
    std::vector<std::string> getSubscribedEvents() const override;
    
    // Public lookup map for external access (e.g., VoucherListView)
    std::unordered_map<std::string, VoucherDef> voucherLookup;
    
private:
    std::string definitionPath;
    std::string walletPath;
    std::string usersPath;
    
    std::vector<VoucherDef> definitions;
    std::vector<AutoProvisionConfig> autoProvisionConfigs;
    
    // Helper methods - delegate to utility classes
    void loadDefinitions();
    void saveDefinitions();
    void loadWallet(std::vector<UserVoucher>& wallet) const;
    void saveWallet(const std::vector<UserVoucher>& wallet) const;
    void loadAutoProvisionConfigs();
    void saveAutoProvisionConfigs();
    
    void handleUserRegistered(const UserRegisteredEvent& event);
};
