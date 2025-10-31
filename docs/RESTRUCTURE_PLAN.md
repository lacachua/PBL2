# 🏗️ PROJECT RESTRUCTURE PLAN

## 📊 Current State Analysis

### Existing Structure (Mixed)
```
include/
├── AccountScreen.h          ❌ UI + Logic mixed
├── BookingScreen.h          ❌ UI + Logic mixed
├── HomeScreen.h             ❌ UI + Logic mixed
├── Movie.h                  ✅ Model (OK)
├── AuthService.h            ⚠️ Service (needs split)
└── ...

src/
├── AccountScreen.cpp        ❌ UI + Logic mixed
├── BookingScreen.cpp        ❌ 1739 lines monolith
├── HomeScreen.cpp           ❌ UI + Logic mixed
└── ...
```

## 🎯 Target Structure (Clean Architecture)

```
PBL2/
├── include/
│   ├── models/              ✅ Plain entities (DONE partially)
│   │   ├── User.h
│   │   ├── Movie.h
│   │   ├── Showtime.h
│   │   ├── Seat.h
│   │   ├── Ticket.h
│   │   └── FoodItem.h
│   │
│   ├── interfaces/          ✅ Abstract interfaces (DONE partially)
│   │   ├── IRepository.h
│   │   ├── ISeatRepository.h
│   │   ├── ITicketRepository.h
│   │   ├── IMovieRepository.h
│   │   ├── IAuthService.h
│   │   └── IPricingStrategy.h
│   │
│   ├── repositories/        🔄 Data access layer
│   │   ├── ISeatRepository.h
│   │   ├── ITicketRepository.h
│   │   └── IMovieRepository.h
│   │
│   ├── services/            🔄 Business logic
│   │   ├── BookingService.h
│   │   ├── PaymentService.h
│   │   ├── AuthService.h
│   │   └── MovieService.h
│   │
│   ├── strategies/          📝 Strategy pattern
│   │   └── IPricingStrategy.h
│   │
│   ├── controllers/         📝 Orchestrators
│   │   ├── HomeController.h
│   │   ├── BookingController.h
│   │   └── AccountController.h
│   │
│   ├── views/               📝 Pure UI rendering
│   │   ├── HomeView.h
│   │   ├── BookingView.h
│   │   └── AccountView.h
│   │
│   ├── components/          ✅ Reusable UI (DONE)
│   │   ├── Button.h
│   │   ├── Text.h
│   │   └── MovieCard.h
│   │
│   └── utils/               ✅ Helpers (DONE)
│       ├── DateTimeHelper.h
│       └── PasswordHasher.h
│
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   │   ├── App.cpp
│   │   └── DIContainer.cpp
│   │
│   ├── models/              ✅ Entity implementations
│   │
│   ├── repositories/        🔄 CSV/DB implementations
│   │   └── csv/
│   │       ├── CSVSeatRepository.cpp
│   │       ├── CSVTicketRepository.cpp
│   │       └── CSVMovieRepository.cpp
│   │
│   ├── services/            🔄 Business logic
│   │   ├── BookingService.cpp
│   │   ├── PaymentService.cpp
│   │   └── AuthService.cpp
│   │
│   ├── strategies/          📝 Pricing strategies
│   │   ├── StandardPricing.cpp
│   │   └── WeekendPricing.cpp
│   │
│   ├── controllers/         📝 Event handlers
│   │   ├── HomeController.cpp
│   │   ├── BookingController.cpp
│   │   └── AccountController.cpp
│   │
│   ├── views/               📝 UI rendering
│   │   ├── HomeView.cpp
│   │   ├── BookingView.cpp
│   │   └── AccountView.cpp
│   │
│   ├── components/          ✅ UI components
│   │
│   └── utils/               ✅ Helpers
│
└── tests/                   📝 Unit tests
    ├── mocks/
    └── services/
```

## 📋 Migration Tasks

### Phase 1: Setup Structure ✅ (CURRENT)
- [x] Create folder structure
- [x] Move existing models to `models/`
- [x] Move existing interfaces to `interfaces/`
- [x] Move existing utils to `utils/`

### Phase 2: Extract Models (1-2 hours)
- [ ] Create `include/models/User.h` from AuthService
- [ ] Create `include/models/Seat.h` from SeatStatusRepository
- [ ] Create `include/models/Ticket.h` from TicketRepository
- [ ] Move `Movie.h` to `models/`
- [ ] Move `Showtime.h` to `models/`
- [ ] Create `include/models/FoodItem.h` from FoodsRepository

### Phase 3: Define Interfaces (2-3 hours)
- [ ] Create `IAuthService.h`
- [ ] Create `IBookingService.h`
- [ ] Create `IPaymentService.h`
- [ ] Create `IPricingStrategy.h`
- [ ] Refine repository interfaces

### Phase 4: Implement Services (4-6 hours)
- [ ] Extract `BookingService` from `BookingScreen`
  - Seat selection logic
  - Food selection logic
  - Price calculation
  - Validation rules
  
- [ ] Refactor `AuthService`
  - Registration logic
  - Login logic
  - Session management
  
- [ ] Create `PaymentService`
  - Payment processing
  - Ticket generation
  - Booking confirmation

### Phase 5: Implement Controllers (3-4 hours)
- [ ] Create `HomeController`
  - Handle movie search
  - Handle navigation
  - Coordinate HomeView
  
- [ ] Create `BookingController`
  - Handle date selection
  - Handle seat clicks
  - Handle food quantity
  - Coordinate BookingView
  
- [ ] Create `AccountController`
  - Handle login/logout
  - Handle registration
  - Coordinate AccountView

### Phase 6: Extract Views (5-7 hours)
- [ ] Create `HomeView` (pure rendering)
  - Draw movie posters
  - Draw search box
  - Draw navigation
  
- [ ] Create `BookingView` (pure rendering)
  - Draw date selection
  - Draw seat map
  - Draw food menu
  - Draw payment summary
  
- [ ] Create `AccountView` (pure rendering)
  - Draw login form
  - Draw registration form
  - Draw user info

### Phase 7: Update Main App (2-3 hours)
- [ ] Create `DIContainer` for dependency injection
- [ ] Refactor `App.cpp` to use controllers
- [ ] Wire all dependencies
- [ ] Remove old Screen classes

### Phase 8: Testing (3-4 hours)
- [ ] Create mock repositories
- [ ] Write service unit tests
- [ ] Write controller tests
- [ ] Integration testing

### Phase 9: Cleanup (1-2 hours)
- [ ] Delete old Screen files
- [ ] Update CMakeLists.txt
- [ ] Update documentation
- [ ] Code review

## 📝 Example: BookingScreen Migration

### Before (Monolith - 1739 lines)
```cpp
// BookingScreen.cpp - Everything mixed
class BookingScreen : public HomeScreen {
    // Data
    vector<Seat> seats;
    vector<Food> foods;
    
    // Business Logic
    void selectSeat() { ... }
    void calculatePrice() { ... }
    void validateBooking() { ... }
    
    // UI Rendering
    void drawSeatMap() { ... }
    void drawFoodMenu() { ... }
    
    // Data Access
    void loadSeats() { CSV::read(...) }
    void saveBooking() { CSV::write(...) }
    
    // Event Handling
    void handleClick() { ... }
};
```

### After (Clean Architecture)
```cpp
// 1. Model (include/models/Seat.h)
struct Seat {
    string id;
    bool occupied;
    bool selected;
};

// 2. Service (include/services/BookingService.h)
class BookingService {
    ISeatRepository& seatRepo;
    ITicketRepository& ticketRepo;
public:
    bool selectSeat(string seatId);
    Money calculateTotal();
    bool validateBooking();
};

// 3. Controller (include/controllers/BookingController.h)
class BookingController {
    BookingService& bookingService;
    BookingView& view;
public:
    void handleSeatClick(Vector2f pos);
    void handleConfirm();
};

// 4. View (include/views/BookingView.h)
class BookingView {
public:
    void drawSeatMap(const vector<Seat>& seats);
    void drawFoodMenu(const vector<Food>& foods);
    void drawPaymentSummary(Money total);
};

// 5. Repository (src/repositories/csv/CSVSeatRepository.cpp)
class CSVSeatRepository : public ISeatRepository {
    vector<Seat> loadSeats(string showtimeKey);
    void updateSeats(string key, vector<Seat> seats);
};
```

## 🎯 Benefits

### Before
❌ BookingScreen.cpp: 1739 lines  
❌ Hard to test (need SFML, CSV files)  
❌ Hard to change (everything coupled)  
❌ Hard to reuse (logic tied to UI)  

### After
✅ BookingService.cpp: ~200 lines  
✅ BookingView.cpp: ~300 lines  
✅ BookingController.cpp: ~150 lines  
✅ Easy to test (mock repositories)  
✅ Easy to change (single responsibility)  
✅ Easy to reuse (service for mobile/web)  

## 🚀 Execution Strategy

### Week 1: Foundation
- Day 1-2: Extract models
- Day 3-4: Define interfaces
- Day 5: Create folder structure

### Week 2: Core Logic
- Day 1-3: Implement services
- Day 4-5: Implement controllers

### Week 3: UI Layer
- Day 1-3: Extract views
- Day 4-5: Wire dependencies

### Week 4: Testing & Polish
- Day 1-2: Unit tests
- Day 3-4: Integration tests
- Day 5: Cleanup & documentation

## ✅ Success Criteria

- [ ] Zero cyclic dependencies
- [ ] All services have >80% test coverage
- [ ] UI code separated from business logic
- [ ] Can swap CSV → Database without changing services
- [ ] Can run services without SFML
- [ ] Build time < 30 seconds
- [ ] All tests pass

---

**Status: Phase 1 Complete - Ready for Phase 2**
