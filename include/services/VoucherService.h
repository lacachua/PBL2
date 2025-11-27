#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include "services/EventSystem.h"
#include "services/VoucherManager.h"

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
 * @brief Service class for admin voucher management operations
 * 
 * This service provides business logic for:
 * - Mass voucher distribution
 * - Individual user voucher assignment
 * - Auto-provisioning vouchers to new users (Observer pattern)
 * 
 * Follows Single Responsibility Principle by separating business logic from UI.
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
    
    // ===== Voucher Definition Operations =====
    
    /**
     * @brief Get all voucher definitions
     * @return Vector of VoucherDef
     */
    std::vector<VoucherDef> getAllDefinitions() const;
    
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
    
    // ===== Distribution Operations =====
    
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
    
private:
    std::string definitionPath;
    std::string walletPath;
    std::string usersPath;
    
    std::vector<VoucherDef> definitions;
    std::vector<AutoProvisionConfig> autoProvisionConfigs;
    
    // Helper methods
    void loadDefinitions();
    void saveDefinitions();
    void loadAutoProvisionConfigs();
    void saveAutoProvisionConfigs();
    
    std::string buildExpiryDate(int daysToExpire) const;
    int todayAsNumber() const;
    int dateStringToNumber(const std::string& date) const;
    std::vector<std::string> splitString(const std::string& input, char delimiter) const;
    std::string trim(const std::string& input) const;
    
    void handleUserRegistered(const UserRegisteredEvent& event);
};
