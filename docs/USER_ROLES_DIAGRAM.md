# SƠ ĐỒ HỆ THỐNG PHÂN QUYỀN NGƯỜI DÙNG
## Cinema Booking System - PBL2

---

## 1. TỔNG QUAN

Hệ thống có **3 vai trò người dùng** chính:

| Vai trò | Mô tả | Quyền hạn |
|---------|-------|-----------|
| **Guest** | Người dùng chưa đăng nhập | Chỉ xem phim |
| **Customer** | Khách hàng đã đăng ký | Đặt vé, voucher, lịch sử |
| **Admin** | Quản trị viên | Toàn quyền quản lý |

---

## 2. SƠ ĐỒ KẾ THỪA CLASS

```
                    ┌─────────────────────────────────────┐
                    │           <<abstract>>              │
                    │              IUser                  │
                    │─────────────────────────────────────│
                    │ # email: string                     │
                    │ # passwordHash: string              │
                    │ # fullName: string                  │
                    │ # birthDate: string                 │
                    │ # phone: string                     │
                    │ # registeredAt: time_t              │
                    │ # role: AppRole                     │
                    │ # status: UserStatus                │
                    │─────────────────────────────────────│
                    │ + getUserType(): string {abstract}  │
                    │ + hasPermission(feature): bool      │
                    │ + getPermissions(): string          │
                    │ + getEmail(): string                │
                    │ + getFullName(): string             │
                    │ + getRole(): AppRole                │
                    │ + setFullName(name): void           │
                    │ + lock(): void                      │
                    │ + unlock(): void                    │
                    └──────────────┬──────────────────────┘
                                   │
           ┌───────────────────────┼───────────────────────┐
           │                       │                       │
           ▼                       ▼                       ▼
┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────────┐
│       Guest         │  │      Customer       │  │       Admin         │
│─────────────────────│  │─────────────────────│  │─────────────────────│
│                     │  │ - bookingHistory:   │  │ - adminLevel:string │
│  (Không có thuộc    │  │   vector<string>    │  │ - lastLogin: time_t │
│   tính riêng)       │  │ - voucherCodes:     │  │                     │
│                     │  │   vector<string>    │  │                     │
│─────────────────────│  │─────────────────────│  │─────────────────────│
│ + getUserType()     │  │ + getUserType()     │  │ + getUserType()     │
│ + hasPermission()   │  │ + hasPermission()   │  │ + hasPermission()   │
│ + getPermissions()  │  │ + getPermissions()  │  │ + getPermissions()  │
│                     │  │ + addBooking()      │  │ + getAdminLevel()   │
│                     │  │ + getBookingHistory │  │ + setAdminLevel()   │
│                     │  │ + addVoucher()      │  │ + isSuperAdmin()    │
│                     │  │ + removeVoucher()   │  │ + getLastLogin()    │
│                     │  │ + getVoucherCodes() │  │ + updateLastLogin() │
│                     │  │ + getTotalBookings()│  │ + canManageUser()   │
│                     │  │                     │  │ + canChangeRole()   │
└─────────────────────┘  └─────────────────────┘  └─────────────────────┘
```

---

## 3. CHI TIẾT TỪNG CLASS

### 3.1 CLASS: IUser (Abstract Base Class)

**File:** `include/models/IUser.h`

#### Thuộc tính (Protected - kế thừa cho derived classes)

| Tên | Kiểu | Mô tả |
|-----|------|-------|
| `email` | `string` | Email đăng nhập (Primary Key, IMMUTABLE) |
| `passwordHash` | `string` | Mật khẩu đã mã hóa SHA-256 |
| `fullName` | `string` | Họ tên đầy đủ |
| `birthDate` | `string` | Ngày sinh (dd/mm/yyyy) |
| `phone` | `string` | Số điện thoại |
| `registeredAt` | `time_t` | Thời điểm đăng ký (Unix timestamp) |
| `role` | `AppRole` | Vai trò: Guest/Customer/Admin |
| `status` | `UserStatus` | Trạng thái: Active/Locked |

#### Phương thức Virtual (Pure - phải implement)

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getUserType()` | `string` | Trả về "Guest", "Customer", hoặc "Admin" |
| `hasPermission(feature)` | `bool` | Kiểm tra quyền truy cập feature |
| `getPermissions()` | `string` | Mô tả các quyền của user |

#### Phương thức Common (Không virtual - giống nhau cho tất cả)

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getEmail()` | `string` | Lấy email |
| `getPasswordHash()` | `string` | Lấy password hash |
| `getFullName()` | `string` | Lấy họ tên |
| `getBirthDate()` | `string` | Lấy ngày sinh |
| `getPhone()` | `string` | Lấy số điện thoại |
| `getRegisteredAt()` | `time_t` | Lấy thời điểm đăng ký |
| `getRole()` | `AppRole` | Lấy vai trò |
| `getStatus()` | `UserStatus` | Lấy trạng thái |
| `getUsername()` | `string` | Alias cho getFullName() |
| `isActive()` | `bool` | Kiểm tra tài khoản đang active |
| `isLocked()` | `bool` | Kiểm tra tài khoản bị khóa |
| `setPasswordHash(hash)` | `void` | Đặt password hash mới |
| `setFullName(name)` | `void` | Cập nhật họ tên |
| `setBirthDate(date)` | `void` | Cập nhật ngày sinh |
| `setPhone(phone)` | `void` | Cập nhật số điện thoại |
| `setStatus(status)` | `void` | Đặt trạng thái |
| `lock()` | `void` | Khóa tài khoản |
| `unlock()` | `void` | Mở khóa tài khoản |
| `getRoleString()` | `string` | Chuyển role thành chuỗi |
| `getStatusString()` | `string` | Chuyển status thành chuỗi |

#### Phương thức Static

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `parseRole(roleStr)` | `AppRole` | Chuyển chuỗi thành enum AppRole |
| `parseStatus(statusStr)` | `UserStatus` | Chuyển chuỗi thành enum UserStatus |

---

### 3.2 CLASS: Guest (Kế thừa IUser)

**File:** `include/models/Guest.h`

**Mô tả:** Người dùng chưa đăng nhập - quyền hạn thấp nhất trong hệ thống

#### Thuộc tính riêng

> *Không có thuộc tính riêng - chỉ sử dụng thuộc tính từ IUser*

#### Constructors

| Constructor | Mô tả |
|-------------|-------|
| `Guest()` | Tạo Guest rỗng với role = AppRole::Guest |
| `Guest(email, passwordHash, fullName, birthDate, phone, registeredAt, status)` | Tạo Guest với thông tin đầy đủ |

#### Phương thức Override

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getUserType()` | `"Guest"` | Trả về loại người dùng |
| `hasPermission(feature)` | `bool` | Chỉ cho phép: view_movies, view_movie_detail, search_movies |
| `getPermissions()` | `string` | "Xem danh sách phim, Xem chi tiết phim, Tìm kiếm phim" |

#### Quyền hạn (Permissions)

| Feature | Được phép |
|---------|:---------:|
| `view_movies` | ✅ |
| `view_movie_detail` | ✅ |
| `search_movies` | ✅ |
| `book_ticket` | ❌ |
| `view_booking_history` | ❌ |
| `use_voucher` | ❌ |
| `view_account` | ❌ |
| `view_admin_panel` | ❌ |

---

### 3.3 CLASS: Customer (Kế thừa IUser)

**File:** `include/models/Customer.h`

**Mô tả:** Khách hàng đã đăng ký - có thể đặt vé và sử dụng voucher

#### Thuộc tính riêng

| Tên | Kiểu | Mô tả |
|-----|------|-------|
| `bookingHistory` | `vector<string>` | Danh sách ticketId đã đặt |
| `voucherCodes` | `vector<string>` | Danh sách mã voucher đang có |

#### Constructors

| Constructor | Mô tả |
|-------------|-------|
| `Customer()` | Tạo Customer rỗng với role = AppRole::Customer |
| `Customer(email, passwordHash, fullName, birthDate, phone, registeredAt, status)` | Tạo Customer với thông tin đầy đủ |

#### Phương thức Override

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getUserType()` | `"Customer"` | Trả về loại người dùng |
| `hasPermission(feature)` | `bool` | Bao gồm quyền Guest + quyền riêng Customer |
| `getPermissions()` | `string` | Mô tả đầy đủ quyền Customer |

#### Phương thức riêng Customer

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `addBooking(ticketId)` | `void` | Thêm vé vào lịch sử đặt |
| `getBookingHistory()` | `const vector<string>&` | Lấy lịch sử đặt vé |
| `addVoucher(code)` | `void` | Thêm voucher vào danh sách |
| `removeVoucher(code)` | `bool` | Xóa voucher sau khi sử dụng |
| `getVoucherCodes()` | `const vector<string>&` | Lấy danh sách voucher |
| `getTotalBookings()` | `int` | Đếm tổng số vé đã đặt |

#### Quyền hạn (Permissions)

| Feature | Được phép |
|---------|:---------:|
| `view_movies` | ✅ |
| `view_movie_detail` | ✅ |
| `search_movies` | ✅ |
| `book_ticket` | ✅ |
| `view_booking_history` | ✅ |
| `use_voucher` | ✅ |
| `view_vouchers` | ✅ |
| `update_profile` | ✅ |
| `view_account` | ✅ |
| `view_admin_panel` | ❌ |
| `manage_users` | ❌ |

---

### 3.4 CLASS: Admin (Kế thừa IUser)

**File:** `include/models/Admin.h`

**Mô tả:** Quản trị viên - toàn quyền quản lý hệ thống

#### Thuộc tính riêng

| Tên | Kiểu | Mô tả |
|-----|------|-------|
| `adminLevel` | `string` | Cấp độ admin: "normal" hoặc "super" |
| `lastLogin` | `time_t` | Thời điểm đăng nhập gần nhất |

#### Constructors

| Constructor | Mô tả |
|-------------|-------|
| `Admin()` | Tạo Admin rỗng với level = "normal" |
| `Admin(email, passwordHash, fullName, birthDate, phone, registeredAt, status, level)` | Tạo Admin với thông tin đầy đủ |

#### Phương thức Override

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getUserType()` | `"Admin"` | Trả về loại người dùng |
| `hasPermission(feature)` | `true` | **Luôn trả về true** - toàn quyền |
| `getPermissions()` | `string` | Mô tả toàn bộ quyền quản trị |

#### Phương thức riêng Admin

| Phương thức | Return | Mô tả |
|-------------|--------|-------|
| `getAdminLevel()` | `string` | Lấy cấp độ admin |
| `setAdminLevel(level)` | `void` | Đặt cấp độ admin |
| `isSuperAdmin()` | `bool` | Kiểm tra có phải super admin |
| `getLastLogin()` | `time_t` | Lấy thời điểm đăng nhập gần nhất |
| `updateLastLogin()` | `void` | Cập nhật thời điểm đăng nhập |
| `canManageUser(targetRole)` | `bool` | Kiểm tra có thể quản lý user với role cụ thể |
| `canChangeRole(currentRole, newRole)` | `bool` | Kiểm tra có thể thay đổi role của user |

#### Quyền hạn (Permissions)

| Feature | Được phép |
|---------|:---------:|
| **Tất cả features** | ✅ |
| `view_movies` | ✅ |
| `view_movie_detail` | ✅ |
| `search_movies` | ✅ |
| `book_ticket` | ✅ |
| `view_booking_history` | ✅ |
| `use_voucher` | ✅ |
| `view_vouchers` | ✅ |
| `update_profile` | ✅ |
| `view_account` | ✅ |
| `view_admin_panel` | ✅ |
| `manage_movies` | ✅ |
| `manage_showtimes` | ✅ |
| `manage_rooms` | ✅ |
| `manage_tickets` | ✅ |
| `manage_combos` | ✅ |
| `manage_vouchers` | ✅ |
| `manage_users` | ✅ |
| `view_revenue_report` | ✅ |
| `change_user_role` | ✅ |
| `lock_user` | ✅ |

---

## 4. MA TRẬN QUYỀN HẠN

| Chức năng | Guest | Customer | Admin |
|-----------|:-----:|:--------:|:-----:|
| Xem danh sách phim | ✅ | ✅ | ✅ |
| Xem chi tiết phim | ✅ | ✅ | ✅ |
| Tìm kiếm phim | ✅ | ✅ | ✅ |
| Đặt vé | ❌ | ✅ | ✅ |
| Xem lịch sử đặt vé | ❌ | ✅ | ✅ |
| Sử dụng voucher | ❌ | ✅ | ✅ |
| Xem voucher cá nhân | ❌ | ✅ | ✅ |
| Cập nhật thông tin | ❌ | ✅ | ✅ |
| Xem thông tin tài khoản | ❌ | ✅ | ✅ |
| **Panel Admin** | ❌ | ❌ | ✅ |
| Quản lý phim | ❌ | ❌ | ✅ |
| Quản lý phòng chiếu | ❌ | ❌ | ✅ |
| Quản lý suất chiếu | ❌ | ❌ | ✅ |
| Quản lý vé | ❌ | ❌ | ✅ |
| Quản lý combo | ❌ | ❌ | ✅ |
| Quản lý voucher | ❌ | ❌ | ✅ |
| Quản lý khách hàng | ❌ | ❌ | ✅ |
| Xem báo cáo doanh thu | ❌ | ❌ | ✅ |
| Khóa/Mở khóa tài khoản | ❌ | ❌ | ✅ |
| Thay đổi role người dùng | ❌ | ❌ | ✅ |

---

## 5. ENUM DEFINITIONS

### 5.1 AppRole

```cpp
// File: include/core/AppRole.h
enum class AppRole {
    Guest,      // Chưa đăng nhập
    Customer,   // Khách hàng
    Admin       // Quản trị viên
};
```

### 5.2 UserStatus

```cpp
// File: include/core/AppRole.h
enum class UserStatus {
    Active,     // Tài khoản đang hoạt động
    Locked      // Tài khoản bị khóa
};
```

---

## 6. FACTORY PATTERN

### UserFactory Class

**File:** `include/models/UserFactory.h`

| Phương thức | Mô tả |
|-------------|-------|
| `createUser(role, email, ...)` | Tạo user theo role |
| `createGuest()` | Tạo Guest mặc định |
| `createCustomer(email, ...)` | Tạo Customer mới |
| `createAdmin(email, ...)` | Tạo Admin mới |
| `clone(user)` | Sao chép user |

```cpp
// Ví dụ sử dụng
auto guest = UserFactory::createGuest();
auto customer = UserFactory::createCustomer("user@email.com", "hash", "Tên", "01/01/2000", "0901234567");
auto admin = UserFactory::createAdmin("admin@email.com", "hash", "Admin", "01/01/1990", "0900000000");
```

---

## 7. LUỒNG XỬ LÝ ĐĂNG NHẬP

```
┌─────────────────┐     ┌──────────────────┐     ┌────────────────────┐
│  App khởi động  │────▶│  Guest tự động   │────▶│ Xem phim, tìm kiếm │
│                 │     │  (chưa login)    │     │                    │
└─────────────────┘     └──────────────────┘     └────────────────────┘
                                │
                                ▼
                        ┌──────────────┐
                        │   Đăng nhập  │
                        └──────────────┘
                                │
            ┌───────────────────┼───────────────────┐
            │                   │                   │
            ▼                   ▼                   ▼
    ┌───────────────┐   ┌───────────────┐   ┌───────────────┐
    │   Customer    │   │    Admin      │   │ Sai mật khẩu  │
    │  HomeScreen   │   │ AdminDashboard│   │  Thông báo    │
    │ (Đặt vé, ...)│   │ (Quản lý, ...)│   │    lỗi        │
    └───────────────┘   └───────────────┘   └───────────────┘
```

---

## 8. CẤU TRÚC THƯ MỤC

```
include/
├── core/
│   └── AppRole.h           # Enum AppRole, UserStatus
├── models/
│   ├── IUser.h             # Abstract base class
│   ├── Guest.h             # Guest implementation
│   ├── Customer.h          # Customer implementation
│   ├── Admin.h             # Admin implementation
│   └── UserFactory.h       # Factory để tạo users
└── services/
    └── AuthService.h       # Xử lý đăng nhập/đăng ký

src/
├── models/
│   └── IUser.cpp           # Implementation parseRole, parseStatus
└── services/
    └── AuthService.cpp     # Implementation authentication
```

---

*Tài liệu cập nhật: 27/11/2025*
*Version: 2.0 - Đã loại bỏ Staff role*
