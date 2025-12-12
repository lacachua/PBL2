# User Class Hierarchy Documentation

## Tổng quan

Project sử dụng cấu trúc kế thừa với 3 class chính:

```
User (Base Class)
├── Customer (User đã đăng nhập với role Customer)
└── Admin (User đã đăng nhập với role Admin)
```

## 1. User (Base Class)

**File:** `include/models/User.h`, `src/models/User.cpp`

**Mô tả:** Class cơ sở đại diện cho người dùng trong hệ thống, bao gồm cả Guest (chưa đăng nhập) và các user đã đăng nhập.

**Thuộc tính:**
- `email` (string) - Primary key, IMMUTABLE
- `passwordHash` (string) - Mật khẩu đã hash
- `fullName` (string) - Họ tên đầy đủ
- `birthDate` (string) - Ngày sinh (dd/mm/yyyy)
- `phone` (string) - Số điện thoại
- `registeredAt` (time_t) - Thời gian đăng ký
- `role` (AppRole) - Vai trò: Guest, Customer, Staff, Admin
- `status` (UserStatus) - Trạng thái: Active, Locked

**Chức năng:**
- ✅ Xem danh sách phim
- ✅ Xem thông tin chi tiết phim
- ✅ Tìm kiếm phim
- ✅ Xem suất chiếu
- ✅ Truy cập giao diện chưa đăng nhập

**Methods chính:**
- `getEmail()`, `getFullName()`, `getRole()`, etc.
- `isActive()`, `isLocked()`
- `lock()`, `unlock()`
- `getRoleString()`, `getStatusString()`
- Static methods: `parseRole()`, `parseStatus()`

---

## 2. Customer (extends User)

**File:** `include/models/Customer.h`, `src/models/Customer.cpp`

**Mô tả:** Người dùng đã đăng nhập với quyền Customer. Thừa kế tất cả chức năng của User và có thêm các quyền riêng.

**Constructor:**
```cpp
Customer();                          // Mặc định
Customer(const User& user);          // Từ User object
Customer(email, passwordHash, ...);  // Đầy đủ parameters
```

**Chức năng bổ sung (so với User):**
- ✅ **Xem thông tin cá nhân** (`canViewPersonalInfo()`)
- ✅ **Xem lịch sử đặt vé** (`canViewBookingHistory()`)
- ✅ **Xem danh sách voucher của mình** 
- ✅ **Đặt vé xem phim** (`canBookTicket()`)
- ✅ **Sử dụng voucher khi đặt vé** (`canUseVoucher()`)
- ✅ Tất cả chức năng của User

**Methods chính:**
- `canBookTicket()` - Kiểm tra quyền đặt vé
- `canUseVoucher()` - Kiểm tra quyền sử dụng voucher
- `canViewPersonalInfo()` - Kiểm tra quyền xem thông tin cá nhân
- `canViewBookingHistory()` - Kiểm tra quyền xem lịch sử đặt vé
- `getDisplayName()` - Lấy tên hiển thị
- `isValidCustomer()` - Validate customer hợp lệ (role=Customer, active)

**UI/UX:**
- Truy cập qua `AccountScreen` sau khi login thành công
- Hiển thị thông tin cá nhân, lịch sử vé, voucher
- Cho phép đặt vé và áp dụng voucher tại `BookingScreen`

---

## 3. Admin (extends User)

**File:** `include/models/Admin.h`, `src/models/Admin.cpp`

**Mô tả:** Người dùng đã đăng nhập với quyền Admin. Thừa kế tất cả chức năng của User và có toàn quyền quản trị hệ thống.

**Constructor:**
```cpp
Admin();                          // Mặc định
Admin(const User& user);          // Từ User object
Admin(email, passwordHash, ...);  // Đầy đủ parameters
```

**Chức năng bổ sung (so với User):**
- ✅ **Quản lý phim** (`canManageMovies()`) - Thêm, sửa, xóa phim
- ✅ **Quản lý suất chiếu** (`canManageShowtimes()`) - Tạo, sửa, xóa suất chiếu
- ✅ **Quản lý phòng chiếu** (`canManageRooms()`) - Thêm, sửa phòng
- ✅ **Quản lý combo** (`canManageCombos()`) - Thêm, sửa, xóa combo
- ✅ **Quản lý voucher** (`canManageVouchers()`) - Tạo, phân phối voucher
- ✅ **Quản lý người dùng** (`canManageUsers()`) - Xem, khóa/mở khóa user
- ✅ **Xem thống kê doanh thu** (`canViewRevenue()`) - Dashboard thống kê
- ✅ **Xem overview** (`canViewOverview()`) - Tổng quan hệ thống
- ✅ Tất cả chức năng của User

**Methods chính:**
- `canManageMovies()`, `canManageShowtimes()`, `canManageRooms()`
- `canManageCombos()`, `canManageVouchers()`, `canManageUsers()`
- `canViewRevenue()`, `canViewOverview()`
- `getAdminDisplayName()` - Lấy tên hiển thị với prefix "Admin: "
- `isValidAdmin()` - Validate admin hợp lệ (role=Admin, active)
- `hasAdminAccess()` - Kiểm tra quyền truy cập admin panel

**UI/UX:**
- Truy cập qua `AdminScreen` sau khi login với tài khoản admin
- Giao diện admin panel với sidebar navigation
- Các panel: Overview, Movies, Showtimes, Rooms, Combos, Vouchers, Users, Revenue

---

## Cách sử dụng

### 1. Tạo User mới (Guest)

```cpp
User guestUser;  // Role mặc định là Guest
```

### 2. Đăng ký Customer

```cpp
// Trong AuthService::registerUser()
User newUser(
    email, 
    passwordHash, 
    fullName, 
    birthDate, 
    phone, 
    time(nullptr),
    AppRole::Customer,  // Set role là Customer
    UserStatus::Active
);
```

### 3. Chuyển User thành Customer sau khi login

```cpp
User* user = repository->findByEmail(email);
if (user && user->getRole() == AppRole::Customer) {
    Customer customer(*user);
    
    // Sử dụng customer methods
    if (customer.canBookTicket()) {
        // Cho phép đặt vé
    }
    
    if (customer.canUseVoucher()) {
        // Cho phép sử dụng voucher
    }
}
```

### 4. Chuyển User thành Admin sau khi login

```cpp
User* user = repository->findByEmail(email);
if (user && user->getRole() == AppRole::Admin) {
    Admin admin(*user);
    
    // Sử dụng admin methods
    if (admin.canManageMovies()) {
        // Cho phép quản lý phim
    }
    
    if (admin.hasAdminAccess()) {
        // Chuyển đến AdminScreen
    }
}
```

---

## Luồng hoạt động

### Khi chưa đăng nhập (Guest/User)
```
[User] → HomeScreen
  ├── Xem danh sách phim
  ├── Xem chi tiết phim (DetailScreen)
  ├── Tìm kiếm phim (GlobalSearchBar)
  ├── Chọn suất chiếu → BookingScreen
  └── Nhập voucher → Thông báo "Cần đăng nhập"
```

### Khi đăng nhập Customer
```
[Customer extends User] → Sau login thành công
  ├── Tất cả chức năng của User
  ├── AccountScreen
  │   ├── Thông tin cá nhân (PersonalInfoView)
  │   ├── Lịch sử đặt vé (PurchaseHistoryView)
  │   └── Danh sách voucher (VoucherListView)
  └── BookingScreen
      ├── Đặt vé → Tạo ticket
      └── Sử dụng voucher → Apply discount
```

### Khi đăng nhập Admin
```
[Admin extends User] → Sau login thành công
  ├── Tất cả chức năng của User
  └── AdminScreen (AdminPanel)
      ├── Overview (Tổng quan)
      ├── Movies (Quản lý phim)
      ├── Showtimes (Quản lý suất chiếu)
      ├── Rooms (Quản lý phòng)
      ├── Combos (Quản lý combo)
      ├── Vouchers (Quản lý voucher)
      ├── Users (Quản lý người dùng)
      └── Revenue (Thống kê doanh thu)
```

---

## Lợi ích của cấu trúc này

### 1. **Single Responsibility Principle (SRP)**
- Mỗi class chỉ chịu trách nhiệm cho một vai trò cụ thể
- User: Dữ liệu cơ bản
- Customer: Nghiệp vụ của khách hàng
- Admin: Nghiệp vụ quản trị

### 2. **Open/Closed Principle (OCP)**
- Dễ dàng mở rộng thêm roles mới (ví dụ: Staff, Manager)
- Không cần sửa code hiện tại

### 3. **Liskov Substitution Principle (LSP)**
- Customer và Admin có thể thay thế User mà không ảnh hưởng logic
- Polymorphism hoạt động chính xác

### 4. **Code Reusability**
- Tránh duplicate code giữa Customer và Admin
- Các chức năng chung được kế thừa từ User

### 5. **Maintainability**
- Dễ debug và test
- Clear separation of concerns
- Dễ đọc và hiểu code

---

## Testing Guidelines

### Test User
```cpp
User user("test@example.com", "hash", "Test User", "01/01/2000", "0123456789", time(nullptr));
assert(user.getEmail() == "test@example.com");
assert(user.getRole() == AppRole::Guest);
assert(user.isActive());
```

### Test Customer
```cpp
Customer customer("customer@example.com", "hash", "Customer Name", "01/01/1990", "0123456789", time(nullptr));
assert(customer.getRole() == AppRole::Customer);
assert(customer.canBookTicket());
assert(customer.canUseVoucher());
assert(customer.isValidCustomer());
```

### Test Admin
```cpp
Admin admin("admin@example.com", "hash", "Admin Name", "01/01/1980", "0123456789", time(nullptr));
assert(admin.getRole() == AppRole::Admin);
assert(admin.canManageMovies());
assert(admin.hasAdminAccess());
assert(admin.isValidAdmin());
```

---

## Migration Notes

### Backward Compatibility
✅ **Hoàn toàn tương thích ngược** với code hiện tại vì:
- Class `User` giữ nguyên 100%
- Customer và Admin là extension, không thay đổi logic cũ
- AppRole enum giữ nguyên
- Tất cả code hiện tại vẫn hoạt động bình thường

### Future Enhancements
Có thể mở rộng thêm:
- `Staff` class cho nhân viên bán vé
- `Manager` class cho quản lý rạp
- Thêm methods cho loyalty program, membership tiers
- Thêm permissions system chi tiết hơn

---

## File Structure

```
include/models/
├── User.h          # Base class
├── Customer.h      # Customer class
└── Admin.h         # Admin class

src/models/
├── User.cpp        # User implementation
├── Customer.cpp    # Customer implementation
└── Admin.cpp       # Admin implementation
```

---

**Version:** 1.0.0  
**Last Updated:** December 12, 2025  
**Status:** ✅ Production Ready
