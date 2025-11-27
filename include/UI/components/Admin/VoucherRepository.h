#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief Định nghĩa voucher (loại voucher có thể tạo)
 */
struct VoucherDefinition {
    std::string code;           // Mã voucher (unique key)
    int type = 1;               // 1: fixed amount, 2: percentage
    double value = 0.0;         // Giá trị giảm (VND hoặc %)
    double minBill = 0.0;       // Hóa đơn tối thiểu để áp dụng
    std::string description;    // Mô tả voucher
    
    VoucherDefinition() = default;
    VoucherDefinition(const std::string& c, int t, double v, double m, const std::string& d)
        : code(c), type(t), value(v), minBill(m), description(d) {}
};

/**
 * @brief Voucher đã được cấp cho user
 */
struct UserVoucherEntry {
    std::string email;          // Email người nhận
    std::string code;           // Mã voucher
    int status = 1;             // 1: usable, 0: used/expired
    std::string expiryDate;     // Ngày hết hạn (YYYYMMDD)
    int quantity = 1;           // Số lượng còn lại
    
    UserVoucherEntry() = default;
    UserVoucherEntry(const std::string& e, const std::string& c, int s, const std::string& exp, int qty)
        : email(e), code(c), status(s), expiryDate(exp), quantity(qty) {}
};

/**
 * @brief Thông tin user đang sở hữu voucher (để hiển thị trong admin)
 */
struct VoucherUserInfo {
    std::string email;
    std::string fullName;
    int quantity;
    std::string expiryDate;
    int status;
};

/**
 * @brief Repository quản lý voucher cho Admin Panel
 * 
 * Responsibilities:
 * - CRUD cho voucher definitions
 * - Quản lý việc cấp/thu hồi voucher cho users
 * - Cấp voucher đồng loạt cho tất cả active users
 * - Lấy danh sách users đang có voucher
 */
class AdminVoucherRepository {
public:
    AdminVoucherRepository(
        const std::string& defPath = "../data/voucher_defs.txt",
        const std::string& walletPath = "../data/user_wallets.txt",
        const std::string& usersPath = "../data/users.txt"
    );
    
    // ===== VOUCHER DEFINITION CRUD =====
    
    /**
     * @brief Load tất cả voucher definitions từ file
     */
    void loadDefinitions();
    
    /**
     * @brief Save tất cả voucher definitions ra file
     */
    void saveDefinitions();
    
    /**
     * @brief Lấy tất cả voucher definitions
     */
    std::vector<VoucherDefinition> getAllDefinitions() const;
    
    /**
     * @brief Tìm voucher definition theo code
     * @return nullptr nếu không tìm thấy
     */
    const VoucherDefinition* findDefinition(const std::string& code) const;
    
    /**
     * @brief Thêm voucher definition mới
     * @return true nếu thành công, false nếu code đã tồn tại
     */
    bool addDefinition(const VoucherDefinition& def);
    
    /**
     * @brief Cập nhật voucher definition
     * @return true nếu thành công
     */
    bool updateDefinition(const VoucherDefinition& def);
    
    /**
     * @brief Xóa voucher definition
     * @return true nếu thành công
     */
    bool deleteDefinition(const std::string& code);
    
    // ===== USER VOUCHER MANAGEMENT =====
    
    /**
     * @brief Load tất cả user vouchers từ file
     */
    void loadUserVouchers();
    
    /**
     * @brief Save tất cả user vouchers ra file
     */
    void saveUserVouchers();
    
    /**
     * @brief Cấp voucher cho một user cụ thể
     * @param email Email của user
     * @param code Mã voucher
     * @param daysToExpire Số ngày hết hạn
     * @param quantity Số lượng voucher
     * @return true nếu thành công
     */
    bool giveVoucherToUser(const std::string& email, const std::string& code, 
                           int daysToExpire, int quantity = 1);
    
    /**
     * @brief Cấp voucher cho tất cả active users
     * @param code Mã voucher
     * @param daysToExpire Số ngày hết hạn
     * @param quantity Số lượng voucher
     * @return Số users đã được cấp voucher
     */
    int giveVoucherToAllActiveUsers(const std::string& code, int daysToExpire, int quantity = 1);
    
    /**
     * @brief Thu hồi voucher từ user
     * @return true nếu thành công
     */
    bool removeVoucherFromUser(const std::string& email, const std::string& code);
    
    /**
     * @brief Lấy danh sách users đang có voucher theo mã
     * @param code Mã voucher
     * @return Vector chứa thông tin users
     */
    std::vector<VoucherUserInfo> getUsersWithVoucher(const std::string& code) const;
    
    /**
     * @brief Lấy danh sách tất cả active users từ file users.txt
     */
    std::vector<std::pair<std::string, std::string>> getActiveUsers() const;
    
    /**
     * @brief Đếm số users đang có voucher
     */
    int countUsersWithVoucher(const std::string& code) const;
    
    /**
     * @brief Cleanup voucher hết hạn
     */
    void cleanupExpiredVouchers();
    
    /**
     * @brief Reload tất cả data
     */
    void reload();
    
private:
    std::string definitionPath;
    std::string walletPath;
    std::string usersPath;
    
    std::vector<VoucherDefinition> definitions;
    std::unordered_map<std::string, VoucherDefinition> definitionLookup;
    std::vector<UserVoucherEntry> userVouchers;
    
    // Helpers
    std::vector<std::string> splitString(const std::string& input, char delimiter) const;
    std::string trim(const std::string& input) const;
    std::string buildExpiryDate(int daysToExpire) const;
    int todayAsNumber() const;
    int dateStringToNumber(const std::string& date) const;
    bool isHeaderRow(const std::vector<std::string>& cols, const std::string& expectedFirstCell) const;
    std::string toUpperCase(const std::string& input) const;
};
