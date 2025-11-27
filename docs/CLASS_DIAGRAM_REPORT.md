# 📋 BÁO CÁO CẤU TRÚC HƯỚNG ĐỐI TƯỢNG
## Dự án: Hệ thống Đặt vé xem phim và Quản lý Rạp chiếu phim

**Phiên bản:** 2.1 (SOLID Refactored)  
**Ngày cập nhật:** 27/11/2025

---

## 1. TỔNG QUAN KIẾN TRÚC

### 1.1 Nguyên lý SOLID đã áp dụng

| Nguyên lý | Áp dụng | Ví dụ |
|-----------|---------|-------|
| **S**ingle Responsibility | Mỗi class chỉ có 1 nhiệm vụ | `VoucherService` chỉ xử lý voucher, `AuthService` chỉ xử lý đăng nhập |
| **O**pen/Closed | Mở rộng mà không sửa code cũ | `IUser` abstract class, derived classes thêm chức năng riêng |
| **L**iskov Substitution | Class con thay thế class cha | `Customer`, `Admin` có thể dùng ở mọi nơi cần `IUser` |
| **I**nterface Segregation | Interface nhỏ gọn | `IAppEventObserver` chỉ có `onAppEvent()`, `getSubscribedEvents()` |
| **D**ependency Inversion | Phụ thuộc abstraction | Services dùng `StringUtils`, `DateTimeUtils` thay vì implement lại |

### 1.2 Sơ đồ kế thừa User (Class Diagram)

```
                          ┌─────────────────────┐
                          │    <<abstract>>     │
                          │       IUser         │
                          │─────────────────────│
                          │ # email: string     │
                          │ # passwordHash      │
                          │ # fullName          │
                          │ # birthDate         │
                          │ # phone             │
                          │ # registeredAt      │
                          │ # role: AppRole     │
                          │ # status: UserStatus│
                          │─────────────────────│
                          │ + getUserType()*    │
                          │ + hasPermission()*  │
                          │ + getPermissions()* │
                          │ + getEmail()        │
                          │ + getFullName()     │
                          │ + lock() / unlock() │
                          └──────────┬──────────┘
                                     │
            ┌────────────────────────┼────────────────────────┐
            │                        │                        │
            ▼                        ▼                        ▼
    ┌───────────────┐       ┌───────────────┐        ┌───────────────┐
    │    Guest      │       │   Customer    │        │    Staff      │
    │───────────────│       │───────────────│        │───────────────│
    │               │       │ - bookingHist │        │ - department  │
    │               │       │ - voucherCodes│        │ - employeeId  │
    │───────────────│       │───────────────│        │───────────────│
    │ + getUserType │       │ + addBooking  │        │ + getDepartment│
    │ + hasPerm...  │       │ + addVoucher  │        │ + getEmployeeId│
    └───────────────┘       │ + removeVouch │        └───────┬───────┘
                            └───────────────┘                │
                                                             │
                                                    ┌────────┴────────┐
                                                    │                 │
                                                    ▼                 ▼
                                            ┌───────────────┐  ┌───────────────┐
                                            │    Admin      │  │     User      │
                                            │───────────────│  │  (Legacy)     │
                                            │ - adminLevel  │  │───────────────│
                                            │ - lastLogin   │  │ Backwards     │
                                            │───────────────│  │ compatible    │
                                            │ + isSuperAdmin│  │ với code cũ   │
                                            │ + canManage..│   └───────────────┘
                                            └───────────────┘
```

### 1.3 Design Patterns đã áp dụng

| Pattern | Vị trí áp dụng | Mô tả |
|---------|----------------|-------|
| **Factory Method** | `UserFactory` | Tạo đúng loại user theo role |
| **Template Method** | `IUser` | Base class định nghĩa template, derived class implement |
| **Repository** | `UserRepository`, `TicketRepository`, `ShowtimeRepository` | Tách biệt logic truy xuất dữ liệu |
| **Service Layer** | `AuthService`, `BookingService`, `VoucherService` | Tách biệt business logic |
| **Observer** | `EventSystem`, `VoucherService` | Loose coupling giữa các components |
| **Singleton** | `App`, `AppEventSystem` | Chỉ có 1 instance |

### 1.4 Sơ đồ Service Layer

```
┌──────────────────────────────────────────────────────────────────────┐
│                         APPLICATION LAYER                            │
│                                                                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │
│  │AuthService  │  │BookingService│  │VoucherService│ │EventSystem  │ │
│  │             │  │             │  │             │  │             │ │
│  │ - login     │  │ - booking   │  │ - give      │  │ - publish   │ │
│  │ - register  │  │ - seats     │  │ - apply     │  │ - subscribe │ │
│  │ - logout    │  │ - voucher   │  │ - cleanup   │  │             │ │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘ │
│         │                │                │                │         │
└─────────┼────────────────┼────────────────┼────────────────┼─────────┘
          │                │                │                │
          ▼                ▼                ▼                ▼
┌──────────────────────────────────────────────────────────────────────┐
│                          UTILITY LAYER                               │
│                                                                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │
│  │StringUtils  │  │DateTimeUtils│  │FileUtils    │  │PasswordHash │  │
│  │             │  │             │  │             │  │             │  │
│  │ - trim      │  │ - now       │  │ - readAll   │  │ - hash      │  │
│  │ - split     │  │ - format    │  │ - writeAll  │  │ - verify    │  │
│  │ - normalize │  │ - parse     │  │ - exists    │  │             │  │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘  │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 2. BẢNG CHI TIẾT CÁC CLASS

### 2.1 ENUM CLASSES

#### AppRole
| Giá trị | Mô tả | Quyền hạn |
|---------|-------|-----------|
| `Guest` | Khách chưa đăng nhập | Chỉ xem phim |
| `Customer` | Khách hàng | Đặt vé, voucher |
| `Staff` | Nhân viên | Xem báo cáo, quản lý suất chiếu |
| `Admin` | Quản trị viên | Toàn quyền |

#### UserStatus
| Giá trị | Mô tả |
|---------|-------|
| `Active` | Tài khoản hoạt động |
| `Locked` | Tài khoản bị khóa |

---

### 2.2 CLASS: IUser (Abstract Base Class)

**File:** `include/models/IUser.h`

| Loại | Tên | Kiểu | Mô tả |
|------|-----|------|-------|
| **Thuộc tính** | `email` | `string` | Email (Primary Key - IMMUTABLE) |
| | `passwordHash` | `string` | Mật khẩu đã hash |
| | `fullName` | `string` | Họ tên đầy đủ |
| | `birthDate` | `string` | Ngày sinh (dd/mm/yyyy) |
| | `phone` | `string` | Số điện thoại |
| | `registeredAt` | `time_t` | Thời điểm đăng ký |
| | `role` | `AppRole` | Vai trò |
| | `status` | `UserStatus` | Trạng thái |
| **Pure Virtual** | `getUserType()` | `string` | Lấy loại người dùng |
| | `hasPermission(feature)` | `bool` | Kiểm tra quyền |
| | `getPermissions()` | `string` | Mô tả các quyền |
| **Virtual** | `~IUser()` | | Destructor |
| **Phương thức** | `getEmail()` | `string` | Getter email |
| | `getFullName()` | `string` | Getter họ tên |
| | `isActive()` | `bool` | Kiểm tra hoạt động |
| | `isLocked()` | `bool` | Kiểm tra bị khóa |
| | `lock()` | `void` | Khóa tài khoản |
| | `unlock()` | `void` | Mở khóa tài khoản |
| | `getRoleString()` | `string` | Role → chuỗi |
| | `getStatusString()` | `string` | Status → chuỗi |
| **Static** | `parseRole(str)` | `AppRole` | Chuỗi → Role |
| | `parseStatus(str)` | `UserStatus` | Chuỗi → Status |

---

### 2.3 CLASS: Guest (kế thừa IUser)

**File:** `include/models/Guest.h`

| Loại | Tên | Mô tả |
|------|-----|-------|
| **Quyền hạn** | `view_movies` | ✅ Xem danh sách phim |
| | `view_movie_detail` | ✅ Xem chi tiết phim |
| | `search_movies` | ✅ Tìm kiếm phim |
| | `book_ticket` | ❌ |
| | `use_voucher` | ❌ |
| **Phương thức** | `getUserType()` | Trả về "Guest" |
| | `hasPermission(feature)` | Chỉ cho phép xem phim |
| | `getPermissions()` | "Xem danh sách phim, Xem chi tiết phim, Tìm kiếm phim" |

---

### 2.4 CLASS: Customer (kế thừa IUser)

**File:** `include/models/Customer.h`

| Loại | Tên | Kiểu | Mô tả |
|------|-----|------|-------|
| **Thuộc tính riêng** | `bookingHistory` | `vector<string>` | Danh sách ticketId đã đặt |
| | `voucherCodes` | `vector<string>` | Danh sách voucher |
| **Quyền hạn** | Tất cả quyền Guest | | ✅ |
| | `book_ticket` | | ✅ Đặt vé |
| | `view_booking_history` | | ✅ Xem lịch sử |
| | `use_voucher` | | ✅ Dùng voucher |
| | `view_vouchers` | | ✅ Xem voucher |
| | `update_profile` | | ✅ Cập nhật profile |
| | `view_account` | | ✅ Xem tài khoản |
| **Phương thức** | `addBooking(ticketId)` | `void` | Thêm vào lịch sử |
| | `getBookingHistory()` | `vector<string>&` | Lấy lịch sử |
| | `addVoucher(code)` | `void` | Thêm voucher |
| | `removeVoucher(code)` | `bool` | Xóa voucher |
| | `getVoucherCodes()` | `vector<string>&` | Lấy vouchers |
| | `getTotalBookings()` | `int` | Đếm số vé |

---

### 2.5 CLASS: Staff (kế thừa IUser)

**File:** `include/models/Staff.h`

| Loại | Tên | Kiểu | Mô tả |
|------|-----|------|-------|
| **Thuộc tính riêng** | `department` | `string` | Bộ phận làm việc |
| | `employeeId` | `string` | Mã nhân viên |
| **Quyền hạn** | Tất cả quyền Customer | | ✅ |
| | `view_admin_panel` | | ✅ Xem panel admin |
| | `view_revenue_report` | | ✅ Xem báo cáo |
| | `manage_showtimes` | | ✅ Quản lý suất chiếu |
| | `view_rooms` | | ✅ Xem phòng chiếu |
| | `view_tickets` | | ✅ Xem danh sách vé |
| | `manage_users` | | ❌ |
| | `manage_vouchers` | | ❌ |
| **Phương thức** | `getDepartment()` | `string` | Lấy bộ phận |
| | `setDepartment(dept)` | `void` | Đặt bộ phận |
| | `getEmployeeId()` | `string` | Lấy mã NV |
| | `setEmployeeId(id)` | `void` | Đặt mã NV |

---

### 2.6 CLASS: Admin (kế thừa IUser)

**File:** `include/models/Admin.h`

| Loại | Tên | Kiểu | Mô tả |
|------|-----|------|-------|
| **Thuộc tính riêng** | `adminLevel` | `string` | "super" hoặc "normal" |
| | `lastLogin` | `time_t` | Đăng nhập cuối |
| **Quyền hạn** | TẤT CẢ | | ✅ `hasPermission()` luôn true |
| **Phương thức** | `getAdminLevel()` | `string` | Lấy cấp độ |
| | `setAdminLevel(level)` | `void` | Đặt cấp độ |
| | `isSuperAdmin()` | `bool` | Kiểm tra super admin |
| | `getLastLogin()` | `time_t` | Lấy thời gian login |
| | `updateLastLogin()` | `void` | Cập nhật login |
| | `canManageUser(role)` | `bool` | Có thể quản lý user? |
| | `canChangeRole(cur, new)` | `bool` | Có thể đổi role? |

---

### 2.7 CLASS: UserFactory (Factory Pattern)

**File:** `include/models/UserFactory.h`

| Phương thức | Tham số | Trả về | Mô tả |
|-------------|---------|--------|-------|
| `createUser(role, ...)` | role, email, pass, name, birth, phone, time, status | `unique_ptr<IUser>` | Tạo user theo role |
| `createGuest()` | - | `unique_ptr<IUser>` | Tạo Guest mặc định |
| `createCustomer(...)` | email, pass, name, birth, phone | `unique_ptr<IUser>` | Tạo Customer mới |
| `createStaff(...)` | email, pass, name, birth, phone, dept, empId | `unique_ptr<IUser>` | Tạo Staff mới |
| `createAdmin(...)` | email, pass, name, birth, phone, level | `unique_ptr<IUser>` | Tạo Admin mới |
| `clone(user)` | const IUser& | `unique_ptr<IUser>` | Clone user |

---

### 2.8 CLASS: User (Legacy - Backwards Compatible)

**File:** `include/models/User.h`

| Mô tả |
|-------|
| Class này kế thừa từ `IUser` để tương thích ngược với code cũ. |
| Cho code mới, nên sử dụng `Guest`, `Customer`, `Staff`, `Admin` và `UserFactory`. |

---

## 3. CÁC SERVICE CLASSES

### 3.1 CLASS: AuthService

**File:** `include/services/AuthService.h`

| Loại | Tên | Mô tả |
|------|-----|-------|
| **Thuộc tính** | `repository` | UserRepository |
| | `voucherService` | VoucherService |
| | `currentUserEmail` | Email đang login |
| | `loggedIn` | Trạng thái login |
| **Phương thức** | `registerUser(...)` | Đăng ký user mới |
| | `login(email, pass)` | Đăng nhập |
| | `logout()` | Đăng xuất |
| | `isLoggedIn()` | Kiểm tra login |
| | `getCurrentUser()` | Lấy user hiện tại |
| | `hasRole(role)` | Kiểm tra role |
| | `isAdmin()` | Kiểm tra admin |
| | `isStaffOrAbove()` | Staff hoặc Admin |
| | `lockUserAccount(email)` | Khóa tài khoản |
| | `unlockUserAccount(email)` | Mở khóa |
| | `changeUserRole(email, role)` | Đổi role |

---

### 3.2 CLASS: BookingService

**File:** `include/services/BookingService.h`

| Loại | Tên | Mô tả |
|------|-----|-------|
| **Thuộc tính** | `userRepository` | Quản lý users |
| | `voucherService` | Quản lý voucher |
| | `ticketRepository` | Quản lý vé |
| | `seatRepository` | Quản lý ghế |
| **Phương thức** | `getUserInfo(email)` | Lấy thông tin user |
| | `applyVoucher(...)` | Áp dụng voucher |
| | `createTicket(info)` | Tạo vé mới |
| | `saveBookedSeats(...)` | Lưu ghế đã đặt |
| | `getBookedSeats(...)` | Lấy ghế đã đặt |
| | `calculateTotal(...)` | Tính tổng tiền |

---

### 3.3 CLASS: VoucherService (Unified Voucher Service)

**File:** `include/services/VoucherService.h`

**Design Principles Applied:**
- Single Responsibility: Chỉ xử lý các thao tác liên quan đến voucher
- Open/Closed: Có thể mở rộng mà không cần sửa code hiện có
- Dependency Inversion: Sử dụng utility abstractions (StringUtils, DateTimeUtils)

| Loại | Tên | Mô tả |
|------|-----|-------|
| **Data Structures** | `VoucherDef` | Định nghĩa voucher (code, type, value, minBill, description) |
| | `UserVoucher` | Voucher của user (email, code, status, expiry, quantity) |
| | `VoucherDisplay` | DTO hiển thị voucher cho UI |
| | `AutoProvisionConfig` | Cấu hình auto-cấp voucher cho user mới |
| **Definition Ops** | `getAllDefinitions()` | Lấy tất cả voucher definitions |
| | `getDefinition(code)` | Lấy definition theo code |
| | `addVoucherDefinition(def)` | Thêm voucher mới |
| | `updateVoucherDefinition(def)` | Cập nhật voucher |
| | `deleteVoucherDefinition(code)` | Xóa voucher |
| **Wallet Ops** | `giveVoucherToUser(...)` | Cấp voucher cho user |
| | `getVouchersByUser(email)` | Lấy voucher của user |
| | `applyVoucher(...)` | Áp dụng voucher (trừ quantity) |
| **Distribution** | `giveVoucherToUsers(...)` | Cấp voucher cho nhiều user |
| | `distributeVoucherToAllActiveUsers(...)` | Cấp cho tất cả active users |
| | `removeVoucherFromUser(...)` | Thu hồi voucher |
| **Auto-Provision** | `setAutoProvision(config)` | Cài đặt auto-cấp |
| | `removeAutoProvision(code)` | Xóa auto-cấp |
| **Observer** | `onAppEvent(event)` | Xử lý sự kiện (UserRegistered) |
| **Maintenance** | `cleanupExpiredVouchers()` | Xóa voucher hết hạn |
| | `reload()` | Tải lại dữ liệu |

---

## 4. CÁC REPOSITORY CLASSES

### 4.1 CLASS: UserRepository

**File:** `include/services/UserRepository.h`

| Phương thức | Mô tả |
|-------------|-------|
| `loadFromFile()` | Load từ file |
| `saveToFile()` | Lưu ra file |
| `exists(email)` | Kiểm tra tồn tại |
| `findByEmail(email)` | Tìm theo email |
| `addUser(user)` | Thêm user |
| `updateUser(user)` | Cập nhật user |
| `lockUser(email)` | Khóa user |
| `unlockUser(email)` | Mở khóa |
| `deleteUser(email)` | Xóa user |
| `getAllUsers()` | Lấy tất cả |
| `getUsersByRole(role)` | Lọc theo role |
| `countByRole(role)` | Đếm theo role |

---

### 4.2 CLASS: TicketRepository

**File:** `include/UI/components/TicketBooking/TicketRepository.h`

| Phương thức | Mô tả |
|-------------|-------|
| `createTicket(...)` | Tạo vé mới |
| `saveTicket(ticket)` | Lưu vé |
| `generateTicketId()` | Tạo ID vé |
| `getTitleUtf8(ticket)` | Lấy tên phim (UTF-8) |
| `getRoomUtf8(ticket)` | Lấy phòng (UTF-8) |

---

### 4.3 CLASS: ShowtimeRepository

**File:** `include/UI/components/TicketBooking/ShowtimeRepository.h`

| Phương thức | Mô tả |
|-------------|-------|
| `loadFromFile(path)` | Load suất chiếu từ file |

---

## 5. CẤU TRÚC DỮ LIỆU TỰ CÀI ĐẶT

### 5.1 CLASS: DLL<T> (Doubly Linked List)

**File:** `include/data-structures/DLL.h`

| Phương thức | Độ phức tạp | Mô tả |
|-------------|-------------|-------|
| `push_back(value)` | O(1) | Thêm cuối |
| `push_front(value)` | O(1) | Thêm đầu |
| `pop_back()` | O(1) | Xóa cuối |
| `pop_front()` | O(1) | Xóa đầu |
| `removeAt(index)` | O(n) | Xóa vị trí |
| `operator[](index)` | O(n) | Truy cập |
| `getSize()` | O(1) | Lấy kích thước |
| `isEmpty()` | O(1) | Kiểm tra rỗng |
| `clear()` | O(n) | Xóa tất cả |

---

### 5.2 CLASS: HashTable<K, V>

**File:** `include/data-structures/HashTable.h`

| Phương thức | Độ phức tạp TB | Mô tả |
|-------------|----------------|-------|
| `insert(key, value)` | O(1) | Thêm/cập nhật |
| `find(key)` | O(1) | Tìm kiếm |
| `remove(key)` | O(1) | Xóa |
| `exists(key)` | O(1) | Kiểm tra tồn tại |
| `contains(key)` | O(1) | Alias của exists |
| `getSize()` | O(1) | Lấy kích thước |
| `forEach(callback)` | O(n) | Duyệt tất cả |

**Collision handling:** Separate Chaining (Linked List)
**Rehash:** Khi load factor > 0.75

---

### 5.3 CLASS: Trie

**File:** `include/data-structures/Trie.h`

| Phương thức | Mô tả |
|-------------|-------|
| `insert(word)` | Thêm từ |
| `search(word)` | Tìm từ chính xác |
| `startsWith(prefix)` | Kiểm tra prefix |
| `getSuggestions(prefix, limit)` | Gợi ý autocomplete |

**Đặc điểm:**
- Hỗ trợ UTF-8 (tiếng Việt)
- Sử dụng Linked List cho children (tiết kiệm bộ nhớ)

---

## 6. SƠ ĐỒ QUAN HỆ TỔNG THỂ

```
┌─────────────────────────────────────────────────────────────────────────┐
│                            PRESENTATION LAYER                           │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│  │ HomeScreen  │ │ LoginScreen │ │BookingScreen│ │ AdminScreen │       │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘       │
└─────────┼───────────────┼───────────────┼───────────────┼───────────────┘
          │               │               │               │
          ▼               ▼               ▼               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                            SERVICE LAYER                                │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│  │ AuthService │ │BookingServ. │ │VoucherManag.│ │ShowtimeClean│       │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘       │
└─────────┼───────────────┼───────────────┼───────────────┼───────────────┘
          │               │               │               │
          ▼               ▼               ▼               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                           REPOSITORY LAYER                              │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│  │UserReposit. │ │TicketRepos.│ │ShowtimeRepo.│ │VoucherRepos.│       │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘       │
└─────────┼───────────────┼───────────────┼───────────────┼───────────────┘
          │               │               │               │
          ▼               ▼               ▼               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                             MODEL LAYER                                 │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│  │   IUser     │ │   Ticket    │ │  Showtime   │ │  VoucherDef │       │
│  │Guest/Cust/..│ │             │ │             │ │             │       │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘       │
└─────────┼───────────────┼───────────────┼───────────────┼───────────────┘
          │               │               │               │
          ▼               ▼               ▼               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                             DATA LAYER                                  │
│     users.txt       tickets.txt     showtimes.txt    voucher_defs.txt  │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 7. MA TRẬN QUYỀN HẠN

| Feature | Guest | Customer | Staff | Admin |
|---------|:-----:|:--------:|:-----:|:-----:|
| Xem danh sách phim | ✅ | ✅ | ✅ | ✅ |
| Xem chi tiết phim | ✅ | ✅ | ✅ | ✅ |
| Tìm kiếm phim | ✅ | ✅ | ✅ | ✅ |
| Đặt vé | ❌ | ✅ | ✅ | ✅ |
| Xem lịch sử đặt vé | ❌ | ✅ | ✅ | ✅ |
| Sử dụng voucher | ❌ | ✅ | ✅ | ✅ |
| Cập nhật profile | ❌ | ✅ | ✅ | ✅ |
| Xem admin panel | ❌ | ❌ | ✅ | ✅ |
| Xem báo cáo doanh thu | ❌ | ❌ | ✅ | ✅ |
| Quản lý suất chiếu | ❌ | ❌ | ✅ | ✅ |
| Quản lý phòng | ❌ | ❌ | ✅ | ✅ |
| Quản lý phim | ❌ | ❌ | ❌ | ✅ |
| Quản lý người dùng | ❌ | ❌ | ❌ | ✅ |
| Quản lý voucher | ❌ | ❌ | ❌ | ✅ |
| Quản lý combo | ❌ | ❌ | ❌ | ✅ |
| Phân quyền | ❌ | ❌ | ❌ | ✅ |

---

## 8. TÓM TẮT REFACTORING

### 8.1 Những thay đổi chính

1. **Tạo Abstract Base Class `IUser`**: Định nghĩa interface chung cho tất cả loại người dùng

2. **Tạo các Concrete Classes**: 
   - `Guest`: Người dùng chưa đăng nhập
   - `Customer`: Khách hàng đã đăng ký
   - `Staff`: Nhân viên
   - `Admin`: Quản trị viên

3. **Áp dụng Factory Pattern**: `UserFactory` để tạo đúng loại user

4. **Giữ Backwards Compatibility**: Class `User` cũ vẫn hoạt động

### 8.2 Lợi ích

| Lợi ích | Mô tả |
|---------|-------|
| **Polymorphism** | Xử lý user qua base class pointer |
| **Open/Closed** | Dễ thêm loại user mới mà không sửa code cũ |
| **Single Responsibility** | Mỗi class chỉ có 1 nhiệm vụ |
| **Type Safety** | Compile-time check cho từng loại user |
| **Maintainability** | Code dễ đọc, dễ bảo trì |

### 8.3 Các file đã tạo mới

```
include/models/
├── IUser.h           # Abstract base class cho User
├── Guest.h           # Concrete class cho Guest
├── Customer.h        # Concrete class cho Customer  
├── Staff.h           # Concrete class cho Staff
├── Admin.h           # Concrete class cho Admin
├── UserFactory.h     # Factory để tạo user
├── Movie.h           # Model cho phim
├── Showtime.h        # Model cho suất chiếu
├── CinemaModels.h    # Room, Ticket, Combo, Voucher models
└── User.h            # (Updated) Backwards compatible

include/utils/
├── StringUtils.h     # Utility cho xử lý chuỗi
├── DateTimeUtils.h   # Utility cho xử lý ngày giờ
├── FileUtils.h       # Utility cho đọc/ghi file
├── PasswordHasher.h  # Hash và verify mật khẩu
└── Validator.h       # Validate input (email, phone, date...)

src/models/
├── IUser.cpp         # Implementation của IUser
└── User.cpp          # (Updated)
```

---

## 9. UTILITY CLASSES

### 9.1 CLASS: StringUtils

**File:** `include/utils/StringUtils.h`

Utility class cho các thao tác xử lý chuỗi, loại bỏ code trùng lặp.

| Phương thức | Tham số | Mô tả |
|-------------|---------|-------|
| `trim(str)` | string | Xóa khoảng trắng đầu/cuối |
| `split(str, delimiter)` | string, char | Tách chuỗi theo delimiter |
| `toLower(str)` | string | Chuyển thành chữ thường |
| `toUpper(str)` | string | Chuyển thành chữ hoa |
| `normalize(str)` | string | trim + toLower |
| `isBlank(str)` | string | Kiểm tra rỗng |
| `join(parts, delimiter)` | vector, string | Nối các phần lại |
| `startsWith(str, prefix)` | string, string | Kiểm tra prefix |
| `endsWith(str, suffix)` | string, string | Kiểm tra suffix |
| `replaceAll(str, from, to)` | string x3 | Thay thế tất cả |
| `isHeaderRow(cols, expected)` | vector, string | Kiểm tra header CSV |

---

### 9.2 CLASS: DateTimeUtils

**File:** `include/utils/DateTimeUtils.h`

Utility class cho các thao tác xử lý ngày giờ.

| Phương thức | Mô tả |
|-------------|-------|
| `now()` | Lấy time_t hiện tại |
| `getTodayISO()` | Lấy ngày hôm nay (YYYY-MM-DD) |
| `getTodayAsNumber()` | Lấy ngày dạng số (YYYYMMDD) |
| `getCurrentTimeInMinutes()` | Giờ hiện tại tính bằng phút |
| `formatDate(timestamp, format)` | Format time_t thành chuỗi |
| `minutesToTimeString(minutes)` | Phút → HH:MM |
| `parseISODate(str)` | Parse YYYY-MM-DD → tm |
| `parseDisplayDate(str)` | Parse DD/MM/YYYY → tm |
| `parseTimeToMinutes(str)` | HH:MM → phút |
| `buildExpiryDate(days)` | Tính ngày hết hạn |
| `roundUpToNext10Minutes(min)` | Làm tròn phút lên bội 10 |
| `isToday(dateStr)` | Kiểm tra có phải hôm nay |
| `isPastDate(dateStr)` | Kiểm tra quá khứ |
| `isFutureDate(dateStr)` | Kiểm tra tương lai |
| `daysBetween(date1, date2)` | Số ngày giữa 2 ngày |
| `displayToISO(date)` | DD/MM/YYYY → YYYY-MM-DD |
| `isoToDisplay(date)` | YYYY-MM-DD → DD/MM/YYYY |

---

### 9.3 CLASS: FileUtils

**File:** `include/utils/FileUtils.h`

Utility class cho các thao tác đọc/ghi file.

| Phương thức | Mô tả |
|-------------|-------|
| `exists(path)` | Kiểm tra file tồn tại |
| `ensureDirectoryExists(path)` | Tạo thư mục nếu chưa có |
| `readAll(path)` | Đọc toàn bộ file |
| `readLines(path, skipHeader)` | Đọc từng dòng |
| `readCSV(path, delimiter)` | Đọc file CSV |
| `writeAll(path, content)` | Ghi file (overwrite) |
| `writeLines(path, lines)` | Ghi từng dòng |
| `writeCSV(path, data)` | Ghi file CSV |
| `append(path, content)` | Thêm vào cuối file |
| `appendCSVRow(path, row)` | Thêm dòng CSV |
| `ensureCSVExists(path, header)` | Đảm bảo CSV có header |
| `remove(path)` | Xóa file |
| `getFileName(path)` | Lấy tên file |
| `getExtension(path)` | Lấy extension |

---

## 10. CINEMA MODELS

### 10.1 STRUCT: Movie

**File:** `include/models/Movie.h`

| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `id` | string | ID phim (VD: "M001") |
| `title` | string | Tên phim |
| `ageRating` | string | Giới hạn độ tuổi |
| `country` | string | Quốc gia |
| `language` | string | Ngôn ngữ |
| `genres` | string | Thể loại |
| `durationMin` | int | Thời lượng (phút) |
| `releaseDate` | string | Ngày khởi chiếu |
| `director` | string | Đạo diễn |
| `cast` | string | Diễn viên |
| `synopsis` | string | Mô tả nội dung |
| `posterPath` | string | Đường dẫn poster |
| `status` | string | Trạng thái |

| Phương thức | Mô tả |
|-------------|-------|
| `isNowShowing()` | Kiểm tra đang chiếu |
| `isComingSoon()` | Kiểm tra sắp chiếu |
| `getDurationString()` | Lấy "XXX phút" |
| `getTitleSf()` | Lấy sf::String (UTF-8) |

---

### 10.2 STRUCT: Showtime

**File:** `include/models/Showtime.h`

| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `id` | string | ID suất chiếu |
| `movieId` | string | ID phim |
| `roomId` | string | ID phòng |
| `date` | string | Ngày (YYYY-MM-DD) |
| `time` | string | Giờ (HH:MM) |
| `price` | int | Giá vé (VND) |

| Phương thức | Mô tả |
|-------------|-------|
| `getTimeInMinutes()` | Giờ → phút |
| `getPriceFormatted()` | Giá có format |
| `operator<` | So sánh (sort) |

---

### 10.3 STRUCT: Room, Ticket, Combo, Voucher

**File:** `include/models/CinemaModels.h`

#### Room
| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `id` | string | ID phòng |
| `name` | string | Tên phòng |
| `capacity` | int | Sức chứa |
| `type` | string | Loại (2D, 3D, IMAX) |
| `status` | string | Trạng thái |

#### Ticket
| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `ticketId` | string | ID vé |
| `showtimeId` | string | ID suất chiếu |
| `movieTitle` | string | Tên phim |
| `seats` | string | Danh sách ghế |
| `combos` | string | Combo đã chọn |
| `totalPrice` | int | Tổng giá |
| `customerEmail` | string | Email KH |
| `customerName` | string | Tên KH |

#### Combo
| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `id` | string | ID combo |
| `name` | string | Tên combo |
| `description` | string | Mô tả |
| `price` | int | Giá |

#### Voucher
| Thuộc tính | Kiểu | Mô tả |
|------------|------|-------|
| `code` | string | Mã voucher |
| `type` | int | 1=fixed, 2=% |
| `value` | double | Giá trị |
| `minBill` | double | Hóa đơn tối thiểu |
| `description` | string | Mô tả |

| Phương thức | Mô tả |
|-------------|-------|
| `calculateDiscount(bill)` | Tính discount |
| `isApplicable(bill)` | Kiểm tra áp dụng được |

---

## 11. SƠ ĐỒ CẤU TRÚC THƯ MỤC

```
PBL2-main/
├── include/
│   ├── core/
│   │   ├── App.h
│   │   ├── AppRole.h          # Enum AppRole, UserStatus
│   │   └── AppState.h         # Enum các state của app
│   │
│   ├── models/
│   │   ├── IUser.h            # Abstract base class
│   │   ├── Guest.h            # Guest user
│   │   ├── Customer.h         # Customer user
│   │   ├── Staff.h            # Staff user
│   │   ├── Admin.h            # Admin user
│   │   ├── User.h             # Legacy (backwards compatible)
│   │   ├── UserFactory.h      # Factory pattern
│   │   ├── Movie.h            # Movie model
│   │   ├── Showtime.h         # Showtime model
│   │   └── CinemaModels.h     # Room, Ticket, Combo, Voucher
│   │
│   ├── services/
│   │   ├── AuthService.h      # Authentication
│   │   ├── BookingService.h   # Booking logic
│   │   ├── UserRepository.h   # User CRUD
│   │   ├── VoucherService.h   # Unified voucher service
│   │   ├── EventSystem.h      # Observer pattern
│   │   ├── MovieSearchManager.h
│   │   └── ShowtimeCleanupService.h
│   │
│   ├── utils/
│   │   ├── StringUtils.h      # String helpers
│   │   ├── DateTimeUtils.h    # DateTime helpers
│   │   ├── FileUtils.h        # File I/O helpers
│   │   ├── PasswordHasher.h   # Password hashing
│   │   └── Validator.h        # Input validation
│   │
│   ├── data-structures/
│   │   ├── DLL.h              # Doubly Linked List
│   │   ├── HashTable.h        # Hash Table
│   │   └── Trie.h             # Trie for search
│   │
│   └── UI/
│       ├── screens/           # Các màn hình
│       └── components/        # Các component UI
│
├── src/                       # Implementation files
├── data/                      # Data files (txt)
├── assets/                    # Fonts, images
├── docs/                      # Documentation
└── build/                     # Build output
```

---

*Tài liệu được tạo bởi GitHub Copilot - PBL2 Cinema Booking System v2.0*
