#include "services/AuthService.h"
#include <iostream>

using namespace std;

AuthService::AuthService(const string& filePath) 
    : currentUserEmail(""), loggedIn(false) {
    repository = make_unique<UserRepository>(filePath);
    voucherService = make_shared<VoucherService>();
    ensureDefaultAdmin();
}

AuthService::~AuthService() {
}

bool AuthService::registerUser(const string& email, 
                               const string& password, 
                               const string& fullName, 
                               const string& birthDate, 
                               const string& phone,
                               AppRole role) {
    // Validate inputs
    if (!Validator::isValidEmail(email)) {
        return false;
    }
    
    if (!Validator::isStrongPassword(password)) {
        return false;
    }
    
    // Check if email already exists
    if (repository->exists(email)) {
        return false;
    }
    
    // Hash password
    string passwordHash = PasswordHasher::hashPassword(password);
    
    // Create new user
    User newUser(
        email,
        passwordHash,
        fullName.empty() ? email : fullName,
        birthDate,
        phone,
        time(nullptr),
        role,
        UserStatus::Active
    );
    
    bool success = repository->addUser(newUser);
    
    // Tự động tặng voucher WELCOME cho user mới (chỉ áp dụng cho Customer)
    if (success && role == AppRole::Customer && voucherService) {
        voucherService->giveVoucherToUser(email, "WELCOME", 365, 1); // Hạn 1 năm, 1 lượt dùng
        
        // Publish UserRegisteredEvent for auto-provisioning additional vouchers
        AppEventSystem::getInstance().publish(UserRegisteredEvent(email, newUser.getFullName()));
    }
    
    return success;
}

bool AuthService::verify(const string& email, const string& password) {
    User* user = repository->findByEmail(email);
    
    if (!user) {
        return false;
    }
    
    // Verify password
    return PasswordHasher::verifyPassword(password, user->getPasswordHash());
}

bool AuthService::login(const string& email, const string& password) {
    User* user = repository->findByEmail(email);
    
    if (!user) {
        return false; // User not found
    }
    
    // Check if account is locked
    if (user->isLocked()) {
        return false; // Account locked
    }
    
    // Verify password
    if (!PasswordHasher::verifyPassword(password, user->getPasswordHash())) {
        return false; // Wrong password
    }
    
    // Login successful
    currentUserEmail = email;
    loggedIn = true;
    
    return true;
}

void AuthService::logout() {
    currentUserEmail = "";
    loggedIn = false;
}

bool AuthService::isLoggedIn() const {
    return loggedIn && !currentUserEmail.empty();
}

string AuthService::getCurrentUserEmail() const {
    return currentUserEmail;
}

User* AuthService::getCurrentUser() {
    if (!isLoggedIn()) {
        return nullptr;
    }
    
    return repository->findByEmail(currentUserEmail);
}

bool AuthService::hasRole(AppRole role) const {
    if (!isLoggedIn()) {
        return false;
    }
    
    User* user = const_cast<AuthService*>(this)->getCurrentUser();
    if (!user) {
        return false;
    }
    
    return user->getRole() == role;
}

bool AuthService::isAdmin() const {
    return hasRole(AppRole::Admin);
}

User* AuthService::getUser(const string& email) {
    return repository->findByEmail(email);
}

bool AuthService::emailExists(const string& email) {
    return repository->exists(email);
}

bool AuthService::lockUserAccount(const string& email) {
    // Don't allow locking yourself
    if (isLoggedIn() && currentUserEmail == email) {
        return false;
    }
    
    return repository->lockUser(email);
}

bool AuthService::unlockUserAccount(const string& email) {
    return repository->unlockUser(email);
}

bool AuthService::changeUserRole(const string& email, AppRole newRole) {
    User* user = repository->findByEmail(email);
    if (!user) {
        return false;
    }
    
    user->setRole(newRole);
    return repository->updateUser(*user);
}

void AuthService::ensureSampleUser() {
    // Backward compatibility - create a sample customer
    if (!repository->exists("test@gmail.com")) {
        registerUser(
            "test@gmail.com",
            "12345678",
            "Test User",
            "01/01/2000",
            "0901234567",
            AppRole::Customer
        );
    }
}

void AuthService::ensureDefaultAdmin() {
    // Create default admin if not exists
    if (!repository->exists("admin@cinexine.vn")) {
        registerUser(
            "admin@cinexine.vn",
            "admin123",  // ⚠️ CHANGE THIS IN PRODUCTION!
            "Nguyễn Văn Quản Trị",
            "01/01/1990",
            "0900000000",
            AppRole::Admin
        );
        cout << "[AuthService] Created default admin: admin@cinexine.vn / admin123\n";
    }
    
    // Ensure sample user exists (backward compatibility)
    ensureSampleUser();
}
