#include "UI/screens/BookingScreen.h"
#include "repositories/MovieRepository.h"
#include "UI/components/TicketBooking/VoucherDisplayState.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>
using namespace std;
using namespace sf;

namespace {
    constexpr size_t kMaxVoucherLength = 16;
}

static string toUtf8String(const String& str) {
    auto buffer = str.toUtf8();
    return string(buffer.begin(), buffer.end());
}

BookingScreen::BookingScreen(Font& f, const String& movieId)
    : BaseScreen(f),
      font(f),
      header(f),
      summary(f),
      currentState(BookingState::suatchieu),
      showtimeSection(f, movieId, ShowtimeRepository().loadFromFile("../data/showtimes.txt")),
      seatRepo("../data/RoomStatusAtShowtime.txt"),
      ticketRepo("../data/tickets.txt"),
      bookingService(make_unique<BookingService>())
{
    // Khởi tạo seatSelection lần đầu
    seatSelection = make_unique<SeatSelection>(font);
    // loginPopup sẽ được tạo khi cần (nullptr ban đầu)
    
    // ✅ Initialize global search bar with movie data
    MovieRepository repo("../data/movies.txt");
    initializeGlobalSearch(repo.getAllMovies());
}

void BookingScreen::getUserInfo(const string& email, string& fullName, string& phone) {
    // ✅ SOLID: Delegate to BookingService instead of reading file directly
    if (bookingService) {
        bookingService->getUserInfo(email, fullName, phone);
    } else {
        fullName = "Khách hàng";
        phone = "";
    }
}

void BookingScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    BaseScreen::update(mousePos, mousePressed, state);
    
    // Don't process booking logic if search is active
    if (globalSearchBar && globalSearchBar->isInputActive()) return;

    BookingState prevState = currentState;
    header.update(mousePos, mousePressed, currentState);

    // ----- Bước 1: chọn suất chiếu -----
    if (currentState == BookingState::suatchieu) {
        showtimeSection.handleClick(mousePos, mousePressed);
        bool canProceed = showtimeSection.hasSelectedShowtime();
        header.handleNavigation(mousePos, mousePressed, currentState, canProceed);

        // Khi user vừa chọn xong suất chiếu -> tạo lại SeatSelection mới
        if (currentState == BookingState::ghengoi) {
            seatSelection = make_unique<SeatSelection>(font); // 💥 reset hoàn toàn
            seatSelection->setTicketPrice(showtimeSection.getSelectedPrice());

            string showtimeIdStr = showtimeSection.getSelectedShowtimeId().toAnsiString();
            string roomIdStr = showtimeSection.getSelectedRoomId().toAnsiString();

            DLL<string> occupied = seatRepo.getBookedSeats(showtimeIdStr, roomIdStr);
            seatSelection->setOccupiedSeats(occupied);
        }
    }

    // ----- Bước 2: chọn ghế -----
    else if (currentState == BookingState::ghengoi && seatSelection) {
        seatSelection->handleClick(mousePos, mousePressed);
        bool canProceed = seatSelection->hasSelectedSeats();
        header.handleNavigation(mousePos, mousePressed, currentState, canProceed);

        // ❌ KHÔNG LƯU GHẾ Ở ĐÂY NỮA - Chỉ lưu khi xác nhận thành công
        
        // Nếu quay lại suất chiếu => hủy seatSelection để đảm bảo reload khi vào lại
        if (currentState == BookingState::suatchieu) {
            seatSelection.reset();
        }
    }
    else if (currentState == BookingState::bapnuoc) {
        if (!comboSelection) {
            auto combos = comboRepo.loadFromFile("../data/combo.txt");
            comboSelection = make_unique<ComboSelection>(font, combos);
        }
        comboSelection->handleClick(mousePos, mousePressed);
        bool canProceed = comboSelection->hasSelectedCombos() || true; // cho phép skip
        header.handleNavigation(mousePos, mousePressed, currentState, canProceed);
    }
    else if (currentState == BookingState::thanhtoan) {
        // ✅ Xử lý popup nếu đang hiển thị
        if (loginPopup) {
            loginPopup->update(mousePos, mousePressed);
            int action = loginPopup->handleClick(mousePos, mousePressed);
            
            if (action == 1) {
                // Click "Quay lại" -> Hủy popup
                loginPopup.reset();
                return;
            }
            else if (action == 2) {
                // Click "Đăng nhập" -> Chuyển sang LOGIN state
                loginPopup.reset();
                state = AppState::LOGIN;
                return;
            }
            return;  // Không xử lý phần còn lại khi popup hiển thị
        }

        if (voucherLoginPopup) {
            voucherLoginPopup->update(mousePos, mousePressed);
            int voucherAction = voucherLoginPopup->handleClick(mousePos, mousePressed);
            if (voucherAction == 1) {
                voucherLoginPopup.reset();
            }
            else if (voucherAction == 2) {
                voucherLoginPopup.reset();
                state = AppState::LOGIN;
                return;
            }
            return;
        }
        
        // Khởi tạo OrderSummary 1 lần
        if (!orderSummary)
            orderSummary = make_unique<OrderSummary>(font);

        // Cập nhật nội dung từ các lựa chọn trước
        orderSummary->generateFromSelections(
            seatSelection ? seatSelection.get() : nullptr,
            comboSelection ? comboSelection.get() : nullptr
        );

        int currentSubtotal = orderSummary->getTotal();

        if (!BaseScreen::isUserLoggedIn()) {
            if (voucherApplied) {
                clearVoucherPreview();
            }
        }

        if (voucherApplied && currentSubtotal != voucherSubtotalSnapshot) {
            string email = BaseScreen::getLoggedInUserEmail();
            double refreshed = email.empty() ? 0.0 : bookingService->applyVoucher(email, appliedVoucherCode, currentSubtotal, false);
            if (refreshed <= 0.0) {
                voucherStatusMessage = "Voucher không còn hợp lệ cho đơn hàng hiện tại.";
                voucherStatusIsError = true;
                clearVoucherPreview();
            } else {
                voucherDiscountValue = static_cast<int>(round(refreshed));
                voucherSubtotalSnapshot = currentSubtotal;
            }
        }

        if (voucherCaretClock.getElapsedTime().asSeconds() >= 0.5f) {
            voucherCaretVisible = !voucherCaretVisible;
            voucherCaretClock.restart();
        }

        handleVoucherInteractions(mousePos, mousePressed, currentSubtotal);

        // ✅ KIỂM TRA: Nếu chưa đăng nhập và nhấn "Tiếp tục"
        bool canProceed = true;
        
        // Lưu state trước khi navigate
        BookingState beforeNav = currentState;
        
        // Xử lý click điều hướng
        header.handleNavigation(mousePos, mousePressed, currentState, canProceed);
        
        // ✅ Nếu user vừa nhấn "Tiếp tục" (state thay đổi) và chưa đăng nhập
        if (currentState != beforeNav && currentState == BookingState::xacnhan) {
            if (!BaseScreen::isUserLoggedIn()) {
                // Tạo popup mới
                loginPopup = make_unique<LoginRequiredPopup>(font);
                currentState = BookingState::thanhtoan;  // Giữ nguyên state
                return;
            }
            
            // ✅ ĐÃ ĐĂNG NHẬP - Thu thập và lưu dữ liệu vào bookingData
            
            // Kiểm tra dữ liệu cần thiết
            if (!seatSelection || !orderSummary) {
                currentState = BookingState::thanhtoan;
                return;
            }
            
            try {
                int subtotalBeforeDiscount = orderSummary->getTotal();

                // ====== THU THẬP DỮ LIỆU VÀO bookingData ======
                
                // (1) Thông tin user
                string userEmail = BaseScreen::getLoggedInUserEmail();
                string fullName, userPhone;
                getUserInfo(userEmail, fullName, userPhone);
                
                bookingData.customerEmail = userEmail;
                bookingData.customerName = fullName;
                bookingData.customerPhone = userPhone;
                
                // (2) Thông tin suất chiếu
                bookingData.showtimeId = toUtf8String(showtimeSection.getSelectedShowtimeId());
                bookingData.movieId = toUtf8String(showtimeSection.getSelectedMovieId());
                bookingData.movieName = toUtf8String(showtimeSection.getSelectedMovieName());
                bookingData.roomId = toUtf8String(showtimeSection.getSelectedRoomId());
                bookingData.roomName = toUtf8String(showtimeSection.getSelectedRoomName());
                bookingData.date = toUtf8String(showtimeSection.getSelectedDate());
                bookingData.time = toUtf8String(showtimeSection.getSelectedTime());
                bookingData.ticketPrice = showtimeSection.getSelectedPrice();
                
                // (3) Thông tin ghế
                auto selectedSeatsList = seatSelection->getSelectedSeats();
                bookingData.selectedSeats.clear();
                for (int i = 0; i < selectedSeatsList.getSize(); i++) {
                    bookingData.selectedSeats.push_back(selectedSeatsList[i]);
                }
                bookingData.seatsDisplay = seatSelection->getSelectedSeatsDisplay();
                bookingData.totalSeats = selectedSeatsList.getSize();
                
                // (4) Thông tin combo
                bookingData.selectedCombos.clear();
                bookingData.comboTotalPrice = 0;
                
                if (comboSelection && comboSelection->hasSelectedCombos()) {
                    auto selectedCombosDLL = comboSelection->getSelectedCombos();
                    for (int i = 0; i < selectedCombosDLL.getSize(); i++) {
                        auto combo = selectedCombosDLL[i];
                        BookingData::ComboItem item;
                        item.comboId = toUtf8String(combo.id);
                        item.comboName = toUtf8String(combo.name);
                        item.price = combo.price;
                        item.quantity = combo.quantity;
                        
                        bookingData.selectedCombos.push_back(item);
                        bookingData.comboTotalPrice += combo.price * combo.quantity;
                    }
                }
                
                // (5) Tổng tiền + voucher
                int appliedDiscount = voucherApplied ? min(voucherDiscountValue, subtotalBeforeDiscount) : 0;
                bookingData.voucherCode = voucherApplied ? appliedVoucherCode : "";
                bookingData.voucherDiscount = appliedDiscount;
                bookingData.totalPrice = max(0, subtotalBeforeDiscount - appliedDiscount);

                if (voucherApplied && !bookingData.voucherCode.empty()) {
                    bookingService->applyVoucher(bookingData.customerEmail, bookingData.voucherCode, subtotalBeforeDiscount, true);
                }
                
                // ====== TẠO VÉ VÀ LƯU VÀO DATABASE ======
                
                // Format combo string cho database (Direction B: combo_id:xN)
                string combosStr = "Không có";
                if (!bookingData.selectedCombos.empty()) {
                    combosStr = "";
                    for (size_t i = 0; i < bookingData.selectedCombos.size(); i++) {
                        if (i > 0) combosStr += ", ";
                        if (!bookingData.selectedCombos[i].comboId.empty()) {
                            combosStr += bookingData.selectedCombos[i].comboId
                                       + ":x" + to_string(bookingData.selectedCombos[i].quantity);
                        } else {
                            combosStr += bookingData.selectedCombos[i].comboName
                                       + " x" + to_string(bookingData.selectedCombos[i].quantity);
                        }
                    }
                }
                
                // Tạo ticket
                Ticket ticket = ticketRepo.createTicket(
                    bookingData.showtimeId,
                    bookingData.movieName,
                    bookingData.date,
                    bookingData.time,
                    bookingData.roomName,
                    bookingData.seatsDisplay,
                    combosStr,
                    bookingData.totalPrice,
                    bookingData.customerEmail,
                    bookingData.customerName
                );
                
                // Lưu ticket ID
                bookingData.ticketId = ticket.ticketId;
                
                // Lưu ghế vào RoomStatusAtShowtime.txt
                DLL<string> seatsDLL;
                for (const auto& seat : bookingData.selectedSeats) {
                    seatsDLL.push_back(seat);
                }
                seatRepo.addBookedSeats(bookingData.showtimeId, bookingData.roomId, seatsDLL);
                
                // ====== KHỞI TẠO CONFIRMATION VIEW ======
                confirmationView = make_unique<ConfirmationView>(font);
                confirmationView->setBookingData(bookingData);
            }
            catch (...) {
                // Nếu có lỗi, quay lại state thanhtoan
                currentState = BookingState::thanhtoan;
                return;
            }
        }
    }
    
    // ----- Bước 5: XÁC NHẬN -----
    else if (currentState == BookingState::xacnhan && confirmationView) {
        confirmationView->update(mousePos, mousePressed);
        
        // Xử lý nút "Quay lại trang chủ"
        if (confirmationView->handleHomeButtonClick(mousePos, mousePressed, state)) {
            // Reset toàn bộ booking state khi quay về home
            currentState = BookingState::suatchieu;
            bookingData.clear();  // ✅ Clear booking data
            seatSelection.reset();
            comboSelection.reset();
            orderSummary.reset();
            confirmationView.reset();
            resetVoucherState();
        }
    }
}

void BookingScreen::draw(RenderWindow& window) {
    BaseScreen::draw(window);
    header.draw(window, currentState);

    if (currentState == BookingState::suatchieu) {
        showtimeSection.draw(window);
        summary.draw(window,
            showtimeSection.getSelectedMovieName(),
            showtimeSection.getSelectedRoomName(),
            showtimeSection.getSelectedDate(),
            showtimeSection.getSelectedTime(),
            showtimeSection.getSelectedPrice(),
            showtimeSection.hasSelectedShowtime()
        );
    }
    else if (currentState == BookingState::ghengoi && seatSelection) {
        seatSelection->draw(window);
        summary.drawWithSeats(window,
            showtimeSection.getSelectedMovieName(),
            showtimeSection.getSelectedRoomName(),
            showtimeSection.getSelectedDate(),
            showtimeSection.getSelectedTime(),
            seatSelection->getSelectedSeatsDisplay(),
            seatSelection->getTotalPrice()
        );
    }
    else if (currentState == BookingState::bapnuoc && comboSelection) {
        comboSelection->draw(window);
        int total = seatSelection->getTotalPrice() + comboSelection->getTotalPrice();
        summary.drawWithSeats(window,
            showtimeSection.getSelectedMovieName(),
            showtimeSection.getSelectedRoomName(),
            showtimeSection.getSelectedDate(),
            showtimeSection.getSelectedTime(),
            seatSelection->getSelectedSeatsDisplay(),
            total
        );
    }
    else if (currentState == BookingState::thanhtoan) {
        if (!orderSummary)
            orderSummary = make_unique<OrderSummary>(font);

        orderSummary->generateFromSelections(
            seatSelection ? seatSelection.get() : nullptr,
            comboSelection ? comboSelection.get() : nullptr
        );

        int subtotal = orderSummary->getTotal();
        int discountValue = voucherApplied ? min(voucherDiscountValue, subtotal) : 0;
        VoucherDisplayState voucherState;
        voucherState.inputText = voucherInput;
        voucherState.appliedCode = appliedVoucherCode;
        voucherState.statusMessage = voucherStatusMessage;
        voucherState.inputActive = voucherInputActive;
        voucherState.caretVisible = voucherCaretVisible;
        voucherState.userLoggedIn = BaseScreen::isUserLoggedIn();
        voucherState.statusIsError = voucherStatusIsError;

        orderSummary->draw(window, voucherState, discountValue);

        int finalTotal = max(0, subtotal - discountValue);

        summary.drawPayment(window,
            showtimeSection.getSelectedMovieName(),
            showtimeSection.getSelectedRoomName(),
            showtimeSection.getSelectedDate(),
            showtimeSection.getSelectedTime(),
            finalTotal
        );
    }
    else if (currentState == BookingState::xacnhan && confirmationView) {
        // ✅ Vẽ ConfirmationView - chiếm toàn bộ không gian (không vẽ header/summary)
        confirmationView->draw(window);
    }
    
    // ✅ Vẽ popup sau cùng (trên tất cả) - chỉ khi tồn tại
    if (loginPopup) {
        loginPopup->draw(window);
    }
    if (voucherLoginPopup) {
        voucherLoginPopup->draw(window);
    }
}

void BookingScreen::handleEvent(const Event& event) {
    BaseScreen::handleEvent(event);

    if (currentState != BookingState::thanhtoan)
        return;

    if (voucherLoginPopup)
        return;

    if (voucherInputActive && event.is<Event::TextEntered>()) {
        auto unicode = event.getIf<Event::TextEntered>()->unicode;
        if (unicode >= 32 && unicode < 128 && voucherInput.size() < kMaxVoucherLength) {
            char ch = static_cast<char>(unicode);
            if (isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_') {
                voucherInput.push_back(static_cast<char>(toupper(static_cast<unsigned char>(ch))));
                clearVoucherPreview();
            }
        }
    }

    if (voucherInputActive && event.is<Event::KeyPressed>()) {
        auto key = event.getIf<Event::KeyPressed>()->code;
        if (key == Keyboard::Key::Backspace) {
            if (!voucherInput.empty()) {
                voucherInput.pop_back();
                clearVoucherPreview();
            }
        }
        else if (key == Keyboard::Key::Enter) {
            int subtotal = orderSummary ? orderSummary->getTotal() : 0;
            applyVoucherCode(subtotal);
        }
    }
}

FloatRect BookingScreen::getVoucherInputBounds() const {
    return FloatRect(
        Vector2f(OrderSummaryLayout::VoucherLabelX, OrderSummaryLayout::VoucherSectionY + 25.f),
        Vector2f(OrderSummaryLayout::VoucherInputWidth, OrderSummaryLayout::VoucherInputHeight)
    );
}

FloatRect BookingScreen::getVoucherButtonBounds() const {
    float x = OrderSummaryLayout::VoucherLabelX
            + OrderSummaryLayout::VoucherInputWidth + OrderSummaryLayout::VoucherButtonSpacing;
    float y = OrderSummaryLayout::VoucherSectionY + 25.f;
    return FloatRect(Vector2f(x, y),
                     Vector2f(OrderSummaryLayout::VoucherButtonWidth, OrderSummaryLayout::VoucherButtonHeight));
}

void BookingScreen::handleVoucherInteractions(Vector2f mousePos, bool mousePressed, int currentSubtotal) {
    if (!mousePressed || voucherLoginPopup) return;

    FloatRect inputBounds = getVoucherInputBounds();
    FloatRect buttonBounds = getVoucherButtonBounds();

    if (inputBounds.contains(mousePos)) {
        voucherInputActive = true;
        return;
    }

    if (buttonBounds.contains(mousePos)) {
        applyVoucherCode(currentSubtotal);
        return;
    }

    if (!inputBounds.contains(mousePos)) {
        voucherInputActive = false;
    }
}

void BookingScreen::applyVoucherCode(int currentSubtotal) {
    if (currentSubtotal <= 0) {
        voucherStatusMessage = "Bạn cần chọn ghế hoặc combo trước khi áp dụng.";
        voucherStatusIsError = true;
        return;
    }

    string code = voucherInput;
    code.erase(remove_if(code.begin(), code.end(), [](unsigned char ch) { return isspace(ch); }), code.end());
    for (auto& ch : code) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    voucherInput = code;

    if (code.empty()) {
        voucherStatusMessage = "Vui lòng nhập mã voucher.";
        voucherStatusIsError = true;
        return;
    }

    if (!BaseScreen::isUserLoggedIn()) {
        voucherLoginPopup = make_unique<LoginRequiredPopup>(font, String(L"Bạn cần đăng nhập để sử dụng voucher."));
        voucherInputActive = false;
        return;
    }

    string email = BaseScreen::getLoggedInUserEmail();
    clearVoucherPreview();

    double discount = bookingService->applyVoucher(email, code, currentSubtotal, false);
    if (discount <= 0.0) {
        voucherStatusMessage = "Voucher không hợp lệ hoặc chưa đáp ứng điều kiện.";
        voucherStatusIsError = true;
        return;
    }

    voucherApplied = true;
    appliedVoucherCode = code;
    voucherDiscountValue = static_cast<int>(round(discount));
    voucherSubtotalSnapshot = currentSubtotal;
    voucherStatusMessage = "Đã áp dụng mã " + code + ".";
    voucherStatusIsError = false;
}

void BookingScreen::clearVoucherPreview() {
    appliedVoucherCode.clear();
    voucherApplied = false;
    voucherDiscountValue = 0;
    voucherSubtotalSnapshot = 0;
    voucherStatusMessage.clear();
    voucherStatusIsError = false;
}

void BookingScreen::resetVoucherState() {
    voucherInput.clear();
    voucherInputActive = false;
    voucherCaretVisible = true;
    voucherCaretClock.restart();
    clearVoucherPreview();
    voucherLoginPopup.reset();
}
