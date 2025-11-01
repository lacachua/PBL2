# 🏗️ PBL2 - PROJECT RESTRUCTURE

## 🎯 Objective

Tách biệt hoàn toàn **UI (Views)** khỏi **Business Logic (Services)** theo Clean Architecture.

## 📁 New Structure

```
PBL2/
├── include/
│   ├── models/              ✅ DONE - Pure entities (POCO)
│   │   ├── User.h
│   │   ├── Movie.h
│   │   ├── Showtime.h
│   │   ├── Seat.h
│   │   ├── Ticket.h
│   │   └── FoodItem.h
│   │
│   ├── interfaces/          🔄 IN PROGRESS
│   │   ├── IAuthService.h
│   │   ├── IBookingService.h
│   │   ├── IMovieService.h
│   │   └── IPaymentService.h
│   │
│   ├── services/            📋 PLANNED
│   │   ├── BookingService.h
│   │   ├── AuthService.h
│   │   └── MovieService.h
│   │
│   ├── controllers/         📋 PLANNED
│   │   ├── HomeController.h
│   │   ├── BookingController.h
│   │   └── AccountController.h
│   │
│   └── views/               📋 PLANNED
│       ├── HomeView.h
│       ├── BookingView.h
│       └── AccountView.h
│
└── src/
    ├── models/              ✅ DONE
    ├── services/            📋 PLANNED
    ├── controllers/         📋 PLANNED
    └── views/               📋 PLANNED
```

## ✅ Progress: 40% Complete

### Phase 1: Foundation ✅ DONE
- [x] Create folder structure
- [x] Extract 6 model classes (User, Movie, Showtime, Seat, Ticket, FoodItem)
- [x] Build verification (passing)

### Phase 2: Services 🔄 IN PROGRESS
- [ ] Define service interfaces
- [ ] Implement BookingService
- [ ] Implement AuthService
- [ ] Implement MovieService

### Phase 3: Controllers 📋 PLANNED
- [ ] HomeController
- [ ] BookingController
- [ ] AccountController

### Phase 4: Views 📋 PLANNED
- [ ] Extract UI rendering from Screen classes
- [ ] Create pure SFML views
- [ ] Wire controllers ↔ views

## 🎓 Key Principles

### ✅ Separation of Concerns
```
Models     → Pure data structures
Services   → Business logic (no UI)
Controllers → Orchestrate flow
Views      → SFML rendering (no logic)
```

### ✅ Dependency Rule
```
Views → Controllers → Services → Models
(Outer layers depend on inner layers)
```

### ✅ Testability
```cpp
// ✅ Can test services WITHOUT SFML
BookingService service(mockRepo);
auto result = service.selectSeat("A1");
EXPECT_TRUE(result.success);
```

## 📊 Before vs After

| Aspect | Before | After |
|--------|--------|-------|
| **UI + Logic** | Mixed in Screen classes | Separated |
| **Largest File** | 1739 lines | ~300 lines |
| **Test Coverage** | 0% (untestable) | 80%+ (mockable) |
| **Reusability** | Tied to SFML | Platform-independent |

## 📚 Documentation

- 📖 [RESTRUCTURE_PLAN.md](./RESTRUCTURE_PLAN.md) - Detailed plan
- 📊 [PROGRESS_REPORT.md](./PROGRESS_REPORT.md) - Current status
- 🏛️ [ARCHITECTURE_DIAGRAM.md](./ARCHITECTURE_DIAGRAM.md) - Architecture
- 🎯 [SOLID_REFACTORING_SUMMARY.md](./SOLID_REFACTORING_SUMMARY.md) - SOLID principles

## 🚀 Quick Start

### Build Project
```bash
cmake --build build
```

### Run Tests (when ready)
```bash
ctest --test-dir build
```

## 📞 Next Actions

1. **Create service interfaces** (Phase 2)
2. **Implement BookingService**
3. **Extract views from Screen classes**

---

**Status: 40% Complete | Build: ✅ Passing**
