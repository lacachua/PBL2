#include "UI/screens/BookingScreen.h"
#include "models/MovieRepository.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace sf;

BookingScreen::BookingScreen(Font& f, const String& movieId)
    : BaseScreen(f),
      font(f),
      header(f),
      summary(f),
      currentState(BookingState::suatchieu),
      showtimeSection(f, movieId, ShowtimeRepository().loadFromFile("../data/showtimes.txt")),
      seatRepo("../data/RoomStatusAtShowtime.txt"),
      ticketRepo("../data/tickets.txt")
{
    // Khởi tạo seatSelection lần đầu
    seatSelection = make_unique<SeatSelection>(font);
    // loginPopup sẽ được tạo khi cần (nullptr ban đầu)
    
    // ✅ Initialize global search bar with movie data
    MovieRepository repo("../data/movies.txt");
    initializeGlobalSearch(repo.getAllMovies());
}

void BookingScreen::getUserInfo(const string& email, string& fullName, string& phone) {
    ifstream file("../data/users.txt");
    if (!file.is_open()) {
        fullName = "Khách hàng";
        phone = "";
        return;
    }
    
    string line;
    getline(file, line); // Bỏ header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string fileEmail, passwordHash, fileName, birthDate, filePhone, registeredAt;
        
        // Format: email|passwordHash|fullName|birthDate|phone|registeredAt
        getline(ss, fileEmail, '|');
        getline(ss, passwordHash, '|');
        getline(ss, fileName, '|');
        getline(ss, birthDate, '|');
        getline(ss, filePhone, '|');
        getline(ss, registeredAt, '|');
        
        if (fileEmail == email) {
            fullName = fileName;
            phone = filePhone;
            file.close();
            return;
        }
    }
    
    file.close();
    fullName = "Khách hàng";
    phone = "";
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
        
        // Khởi tạo OrderSummary 1 lần
        if (!orderSummary)
            orderSummary = make_unique<OrderSummary>(font);

        // Cập nhật nội dung từ các lựa chọn trước
        orderSummary->generateFromSelections(
            seatSelection ? seatSelection.get() : nullptr,
            comboSelection ? comboSelection.get() : nullptr
        );

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
            
            // ✅ ĐÃ ĐĂNG NHẬP - Thực hiện xác nhận đặt vé
            
            // Kiểm tra dữ liệu cần thiết
            if (!seatSelection || !orderSummary) {
                currentState = BookingState::thanhtoan;
                return;
            }
            
            try {
                // (1) Lấy thông tin user từ TXT
                string userEmail = BaseScreen::getLoggedInUserEmail();  // ✅ Lấy email đầy đủ
                string fullName, userPhone;
                getUserInfo(userEmail, fullName, userPhone);
                
                // (2) Lấy thông tin đặt vé
                string showtimeIdStr = showtimeSection.getSelectedShowtimeId().toAnsiString();
                string roomIdStr = showtimeSection.getSelectedRoomId().toAnsiString();
                string movieTitle = showtimeSection.getSelectedMovieName().toAnsiString();
                string roomName = showtimeSection.getSelectedRoomName().toAnsiString();
                string dateStr = showtimeSection.getSelectedDate().toAnsiString();
                string timeStr = showtimeSection.getSelectedTime().toAnsiString();
                string seatsStr = seatSelection->getSelectedSeatsDisplay();
                
                // Format combos
                string combosStr = "Không có";
                if (comboSelection && comboSelection->hasSelectedCombos()) {
                    auto selectedCombos = comboSelection->getSelectedCombos();
                    combosStr = "";
                    for (size_t i = 0; i < selectedCombos.getSize(); ++i) {
                        auto combo = selectedCombos[i];
                        if (i > 0) combosStr += ", ";
                        string comboName = combo.name.toAnsiString();
                        combosStr += comboName + " x" + to_string(combo.quantity);
                    }
                }
                
                int totalPrice = orderSummary->getTotal();
                
                // (3) Tạo ticket và lưu vào tickets.txt
                Ticket ticket = ticketRepo.createTicket(
                    showtimeIdStr,    // showtime_id
                    movieTitle,       // title
                    dateStr,          // date
                    timeStr,          // time
                    roomName,         // room_name
                    seatsStr,         // booked (ghế)
                    combosStr,        // combo_name
                    totalPrice,       // price
                    userEmail,        // email
                    fullName          // fullName
                );
                
                // (4) Lưu ghế vào RoomStatusAtShowtime.txt
                seatRepo.addBookedSeats(showtimeIdStr, roomIdStr, seatSelection->getSelectedSeats());
                
                // (5) Khởi tạo ConfirmationView với thông tin ticket
                confirmationView = make_unique<ConfirmationView>(font);
                
                confirmationView->setTicketData(
                    ticket,
                    fullName,
                    userPhone,
                    movieTitle,
                    roomName,
                    dateStr,
                    timeStr
                );
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
            seatSelection.reset();
            comboSelection.reset();
            orderSummary.reset();
            confirmationView.reset();
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

        orderSummary->draw(window);

        summary.drawPayment(window,
            showtimeSection.getSelectedMovieName(),
            showtimeSection.getSelectedRoomName(),
            showtimeSection.getSelectedDate(),
            showtimeSection.getSelectedTime(),
            orderSummary->getTotal()
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
}
