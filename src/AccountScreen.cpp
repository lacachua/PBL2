#include "AccountScreen.h"
#include "PasswordHasher.h"

AccountScreen::AccountScreen(const Font& f, AuthService& auth) 
    : font(f),
      authService(&auth),
      titleText(f, L"THÔNG TIN KHÁCH HÀNG", 28),
      menuItem1(f, L"Thông tin khách\nhàng", 220.f, 52.f, 14),
      menuItem2(f, L"Lịch sử đặt vé", 220.f, 52.f, 14),
      menuItem3(f, L"Thành viên\nCinestar", 220.f, 52.f, 14),
      personalInfoTitle(f, L"Thông tin cá nhân", 18),
      fullNameLabel(f, L"Họ và tên", 14),
      birthDateLabel(f, L"Ngày sinh", 14),
      phoneLabel(f, L"Số điện thoại", 14),
      emailLabel(f, L"Email", 14),
      fullNameText(font, "", 14),
      birthDateText(font, "", 14),
      phoneText(font, "", 14),
      emailText(font, "", 14),
      changePasswordTitle(font, L"Đổi mật khẩu", 18),
      oldPasswordLabel(font, L"Mật khẩu cũ *", 14),
      newPasswordLabel(font, L"Mật khẩu mới *", 14),
      confirmPasswordLabel(font, L"Xác thực mật khẩu *", 14),
      oldPasswordText(font, "", 14),
      newPasswordText(font, "", 14),
      confirmPasswordText(font, "", 14),
      saveInfoBtn(f, L"LƯU THÔNG TIN", 220.f, 45.f, 14),
      changePasswordBtn(f, L"ĐỔI MẬT KHẨU", 220.f, 45.f, 14),
      infoMessage(font, "", 14),
      passwordMessage(font, "", 14)
{
    // Setup title
    titleText.setFillColor(Color::White);
    titleText.setStyle(Text::Bold);
    
    // Setup section titles - như cũ
    personalInfoTitle.setFillColor(Color(238, 238, 238));
    personalInfoTitle.setStyle(Text::Bold);
    changePasswordTitle.setFillColor(Color(238, 238, 238));
    changePasswordTitle.setStyle(Text::Bold);
    
    // Setup labels - như cũ
    fullNameLabel.setFillColor(Color(191, 215, 234));
    birthDateLabel.setFillColor(Color(191, 215, 234));
    phoneLabel.setFillColor(Color(191, 215, 234));
    emailLabel.setFillColor(Color(191, 215, 234));
    oldPasswordLabel.setFillColor(Color(191, 215, 234));
    newPasswordLabel.setFillColor(Color(191, 215, 234));
    confirmPasswordLabel.setFillColor(Color(191, 215, 234));
    
    // Setup input boxes - giữ kích thước như ảnh
    fullNameBox.setSize({220.f, 35.f});
    birthDateBox.setSize({220.f, 35.f});
    phoneBox.setSize({220.f, 35.f});
    emailBox.setSize({220.f, 35.f});
    
    // Input background: White (như ảnh)
    fullNameBox.setFillColor(Color::White);
    birthDateBox.setFillColor(Color::White);
    phoneBox.setFillColor(Color::White);
    emailBox.setFillColor(Color::White);
    
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineThickness(1.f);
    emailBox.setOutlineThickness(1.f);
    
    // Border color - xám nhạt
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineColor(Color(201, 214, 226));
    emailBox.setOutlineColor(Color(201, 214, 226));
    
    // Setup input text - đen
    fullNameText.setFillColor(Color(13, 27, 42));
    birthDateText.setFillColor(Color(13, 27, 42));
    phoneText.setFillColor(Color(13, 27, 42));
    emailText.setFillColor(Color(13, 27, 42));
    
    // Setup input boxes - Password Change
    oldPasswordBox.setSize({420.f, 35.f}); // Full width trong card
    newPasswordBox.setSize({420.f, 35.f});
    confirmPasswordBox.setSize({420.f, 35.f});
    
    oldPasswordBox.setFillColor(Color::White);
    newPasswordBox.setFillColor(Color::White);
    confirmPasswordBox.setFillColor(Color::White);
    
    oldPasswordBox.setOutlineThickness(1.f);
    newPasswordBox.setOutlineThickness(1.f);
    confirmPasswordBox.setOutlineThickness(1.f);
    
    oldPasswordBox.setOutlineColor(Color(201, 214, 226));
    newPasswordBox.setOutlineColor(Color(201, 214, 226));
    confirmPasswordBox.setOutlineColor(Color(201, 214, 226));
    
    // Setup password text
    oldPasswordText.setFillColor(Color(13, 27, 42));
    newPasswordText.setFillColor(Color(13, 27, 42));
    confirmPasswordText.setFillColor(Color(13, 27, 42));
    
    // Setup buttons - màu xanh như cũ
    saveInfoBtn.setNormalColor(Color(0, 153, 255));
    saveInfoBtn.setHoverColor(Color(0, 191, 255));
    saveInfoBtn.setTextFillColor(Color::White);
    
    changePasswordBtn.setNormalColor(Color(0, 153, 255));
    changePasswordBtn.setHoverColor(Color(0, 191, 255));
    changePasswordBtn.setTextFillColor(Color::White);
    
    // Setup cursor
    cursor.setSize({2.f, 20.f});
    cursor.setFillColor(Color(0, 153, 255));
    
    // Setup message texts
    infoMessage.setFillColor(Color(60, 160, 90)); // Green for success
    passwordMessage.setFillColor(Color(60, 160, 90));
    
    // Setup main card background - xanh navy đậm như cũ
    mainCardBg.setFillColor(Color(0, 24, 48, 235)); // rgba(0,24,48,0.92)
    mainCardBg.setOutlineThickness(1.f);
    mainCardBg.setOutlineColor(Color(10, 51, 92));
    
    // Setup sidebar - giữ nguyên như cũ
    sidebarBg.setSize({340.f, 550.f});
    sidebarBg.setFillColor(Color(15, 35, 65, 200));
    
    // Setup menu items - màu xanh navy như cũ
    menuItem1.setNormalColor(Color(60, 130, 210, 200)); // Active - xanh sáng
    menuItem1.setHoverColor(Color(80, 150, 230, 220));
    menuItem1.setTextFillColor(Color(220, 240, 255));
    
    menuItem2.setNormalColor(Color(25, 45, 80, 150)); // Inactive - xanh đậm
    menuItem2.setHoverColor(Color(45, 65, 100, 170));
    menuItem2.setTextFillColor(Color(160, 180, 210));
    
    menuItem3.setNormalColor(Color(25, 45, 80, 150));
    menuItem3.setHoverColor(Color(45, 65, 100, 170));
    menuItem3.setTextFillColor(Color(160, 180, 210));
}

void AccountScreen::setCurrentUser(const string& email) {
    currentUserEmail = email;
    currentUser = authService->getUser(email);
    
    if (currentUser) {
        // Set text displays
        fullNameText.setString(currentUser->fullName);
        birthDateText.setString(currentUser->birthDate);
        phoneText.setString(currentUser->phone);
        emailText.setString(currentUser->email);
        
        // Initialize editable inputs
        fullNameInput = wstring(currentUser->fullName.begin(), currentUser->fullName.end());
        birthDateInput = wstring(currentUser->birthDate.begin(), currentUser->birthDate.end());
        phoneInput = wstring(currentUser->phone.begin(), currentUser->phone.end());
    }
}

void AccountScreen::updatePositions(Vector2u windowSize) {
    // Sidebar - giữ nguyên vị trí cũ (60, 200)
    float sidebarX = 60.f;
    float sidebarY = 200.f;
    sidebarBg.setPosition({sidebarX, sidebarY});
    
    // Menu items inside sidebar (10px padding)
    float menuStartY = sidebarY + 40.f;
    menuItem1.setPosition(sidebarX + 10.f, menuStartY);
    menuItem2.setPosition(sidebarX + 10.f, menuStartY + 70.f);
    menuItem3.setPosition(sidebarX + 10.f, menuStartY + 140.f);
    
    // Main card - white background bên phải sidebar
    float cardX = sidebarX + 340.f + 20.f; // sidebar width + gap
    float cardY = 200.f;
    float cardWidth = 780.f;
    float cardHeight = 600.f;
    float cardPadding = 30.f;
    
    mainCardBg.setSize({cardWidth, cardHeight});
    mainCardBg.setPosition({cardX, cardY});
    
    // Section 1: Thông tin cá nhân (top section)
    float section1Y = cardY + cardPadding;
    personalInfoTitle.setPosition({cardX + cardPadding, section1Y});
    
    // Two columns layout for personal info
    float col1X = cardX + cardPadding;
    float col2X = cardX + cardPadding + 230.f; // 220px box + 10px gap
    float inputStartY = section1Y + 35.f; // Below title
    float rowHeight = 65.f;
    
    // Column 1: Họ và tên, Số điện thoại
    fullNameLabel.setPosition({col1X, inputStartY});
    fullNameBox.setPosition({col1X, inputStartY + 20.f});
    fullNameText.setPosition({col1X + 8.f, inputStartY + 28.f});
    
    phoneLabel.setPosition({col1X, inputStartY + rowHeight});
    phoneBox.setPosition({col1X, inputStartY + rowHeight + 20.f});
    phoneText.setPosition({col1X + 8.f, inputStartY + rowHeight + 28.f});
    
    // Column 2: Ngày sinh, Email
    birthDateLabel.setPosition({col2X, inputStartY});
    birthDateBox.setPosition({col2X, inputStartY + 20.f});
    birthDateText.setPosition({col2X + 8.f, inputStartY + 28.f});
    
    emailLabel.setPosition({col2X, inputStartY + rowHeight});
    emailBox.setPosition({col2X, inputStartY + rowHeight + 20.f});
    emailText.setPosition({col2X + 8.f, inputStartY + rowHeight + 28.f});
    
    // Save button for personal info
    float saveInfoBtnY = inputStartY + rowHeight * 2 + 10.f;
    saveInfoBtn.setPosition(col1X, saveInfoBtnY);
    infoMessage.setPosition({col1X, saveInfoBtnY + 50.f});
    
    // Section 2: Đổi mật khẩu (bottom section)
    float section2Y = saveInfoBtnY + 70.f;
    changePasswordTitle.setPosition({cardX + cardPadding, section2Y});
    
    float pwdInputStartY = section2Y + 35.f;
    float pwdRowHeight = 60.f;
    
    // Password fields - full width
    oldPasswordLabel.setPosition({col1X, pwdInputStartY});
    oldPasswordBox.setPosition({col1X, pwdInputStartY + 20.f});
    oldPasswordText.setPosition({col1X + 8.f, pwdInputStartY + 28.f});
    
    newPasswordLabel.setPosition({col1X, pwdInputStartY + pwdRowHeight});
    newPasswordBox.setPosition({col1X, pwdInputStartY + pwdRowHeight + 20.f});
    newPasswordText.setPosition({col1X + 8.f, pwdInputStartY + pwdRowHeight + 28.f});
    
    confirmPasswordLabel.setPosition({col1X, pwdInputStartY + pwdRowHeight * 2});
    confirmPasswordBox.setPosition({col1X, pwdInputStartY + pwdRowHeight * 2 + 20.f});
    confirmPasswordText.setPosition({col1X + 8.f, pwdInputStartY + pwdRowHeight * 2 + 28.f});
    
    // Change password button
    float changePwdBtnY = pwdInputStartY + pwdRowHeight * 3 + 10.f;
    changePasswordBtn.setPosition(col1X, changePwdBtnY);
    passwordMessage.setPosition({col1X, changePwdBtnY + 50.f});
}

void AccountScreen::handlePasswordInput(const Event* event) {
    if (!event) return;
    
    if (auto* textEvent = event->getIf<Event::TextEntered>()) {
        char c = static_cast<char>(textEvent->unicode);
        
        if (c == '\b') { // Backspace
            if (activeField == 0 && !oldPasswordInput.empty())
                oldPasswordInput.pop_back();
            else if (activeField == 1 && !newPasswordInput.empty())
                newPasswordInput.pop_back();
            else if (activeField == 2 && !confirmPasswordInput.empty())
                confirmPasswordInput.pop_back();
        }
        else if (c >= 32 && c < 127) { // Printable characters
            if (activeField == 0)
                oldPasswordInput += c;
            else if (activeField == 1)
                newPasswordInput += c;
            else if (activeField == 2)
                confirmPasswordInput += c;
        }
        
        // Update display with asterisks
        if (activeField == 0)
            oldPasswordText.setString(string(oldPasswordInput.length(), '*'));
        else if (activeField == 1)
            newPasswordText.setString(string(newPasswordInput.length(), '*'));
        else if (activeField == 2)
            confirmPasswordText.setString(string(confirmPasswordInput.length(), '*'));
    }
}

void AccountScreen::handleInfoInput(const Event* event) {
    if (!event) return;
    
    if (auto* textEvent = event->getIf<Event::TextEntered>()) {
        wchar_t c = static_cast<wchar_t>(textEvent->unicode);
        
        if (c == '\b') { // Backspace
            if (activeInfoField == 0 && !fullNameInput.empty())
                fullNameInput.pop_back();
            else if (activeInfoField == 1 && !birthDateInput.empty())
                birthDateInput.pop_back();
            else if (activeInfoField == 2 && !phoneInput.empty())
                phoneInput.pop_back();
        }
        else if (c >= 32 && c != 127) { // Printable characters (including Unicode)
            if (activeInfoField == 0)
                fullNameInput += c;
            else if (activeInfoField == 1 && birthDateInput.length() < 10) // DD/MM/YYYY
                birthDateInput += c;
            else if (activeInfoField == 2 && phoneInput.length() < 15)
                phoneInput += c;
        }
        
        // Update display text - convert wstring to String properly
        if (activeInfoField == 0) {
            fullNameText.setString(fullNameInput);
        }
        else if (activeInfoField == 1) {
            birthDateText.setString(birthDateInput);
        }
        else if (activeInfoField == 2) {
            phoneText.setString(phoneInput);
        }
    }
}

void AccountScreen::savePasswordChange() {
    if (!currentUser) return;
    
    // Validate inputs
    if (oldPasswordInput.empty() || newPasswordInput.empty() || confirmPasswordInput.empty()) {
        passwordMessage.setFillColor(Color(200, 60, 60)); // Red for error
        passwordMessage.setString(L"Vui lòng điền đầy đủ thông tin!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    // Verify old password
    if (!PasswordHasher::verifyPassword(oldPasswordInput, currentUser->passwordHash)) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Mật khẩu cũ không đúng!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    // Check if new passwords match
    if (newPasswordInput != confirmPasswordInput) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Mật khẩu mới không khớp!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    // Update password
    currentUser->passwordHash = PasswordHasher::hashPassword(newPasswordInput);
    authService->saveUsers();
    
    // Clear inputs
    oldPasswordInput.clear();
    newPasswordInput.clear();
    confirmPasswordInput.clear();
    oldPasswordText.setString("");
    newPasswordText.setString("");
    confirmPasswordText.setString("");
    activeField = -1;
    
    // Show success message
    passwordMessage.setFillColor(Color(60, 160, 90)); // Green
    passwordMessage.setString(L"Đổi mật khẩu thành công!");
    showPasswordMessage = true;
    messageTimerPassword.restart();
}

void AccountScreen::saveInfoChange() {
    if (!currentUser) return;
    
    // Convert wstring to string
    string newFullName(fullNameInput.begin(), fullNameInput.end());
    string newBirthDate(birthDateInput.begin(), birthDateInput.end());
    string newPhone(phoneInput.begin(), phoneInput.end());
    
    // Validate full name - không rỗng, không chứa số
    if (newFullName.empty()) {
        infoMessage.setFillColor(Color(255, 68, 68)); // #FF4444 - Red for error
        infoMessage.setString(L"Họ tên không được để trống!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        fullNameBox.setOutlineColor(Color(255, 68, 68));
        fullNameBox.setOutlineThickness(2.f);
        return;
    }
    
    if (!Validator::isValidFullName(newFullName)) {
        infoMessage.setFillColor(Color(255, 68, 68));
        infoMessage.setString(L"Họ tên không được chứa số!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        fullNameBox.setOutlineColor(Color(255, 68, 68));
        fullNameBox.setOutlineThickness(2.f);
        return;
    }
    
    // Validate birth date - đúng định dạng, không lớn hơn ngày hiện tại
    if (!newBirthDate.empty() && !Validator::isValidDate(newBirthDate)) {
        infoMessage.setFillColor(Color(255, 68, 68));
        infoMessage.setString(L"Ngày sinh không hợp lệ (dd/mm/yyyy)!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        birthDateBox.setOutlineColor(Color(255, 68, 68));
        birthDateBox.setOutlineThickness(2.f);
        return;
    }
    
    // Validate phone - 10 chữ số
    if (!newPhone.empty() && !Validator::isValidPhone(newPhone)) {
        infoMessage.setFillColor(Color(255, 68, 68));
        infoMessage.setString(L"Số điện thoại phải có 10 chữ số!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        phoneBox.setOutlineColor(Color(255, 68, 68));
        phoneBox.setOutlineThickness(2.f);
        return;
    }
    
    // Reset outline colors về bình thường - #C9D6E2
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineThickness(1.f);
    
    // Update user data
    currentUser->fullName = newFullName;
    currentUser->birthDate = newBirthDate;
    currentUser->phone = newPhone;
    
    // Save to CSV
    authService->saveUsers();
    
    // Show success message
    infoMessage.setFillColor(Color(60, 160, 90)); // Green
    infoMessage.setString(L"Cập nhật thông tin thành công!");
    showInfoMessage = true;
    messageTimerInfo.restart();
}

void AccountScreen::update(Vector2f mousePos, bool mousePressed, const Event* event, AppState& state) {
    // Update menu buttons hover state
    menuItem1.update(mousePos);
    menuItem2.update(mousePos);
    menuItem3.update(mousePos);
    
    // Handle menu clicks
    if (mousePressed) {
        if (menuItem1.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::CUSTOMER_INFO;
            // Update menu colors - blue theme
            menuItem1.setNormalColor(Color(60, 130, 210, 200)); // Active
            menuItem1.setTextFillColor(Color(220, 240, 255));
            
            menuItem2.setNormalColor(Color(25, 45, 80, 150)); // Inactive
            menuItem2.setTextFillColor(Color(160, 180, 210));
            
            menuItem3.setNormalColor(Color(25, 45, 80, 150));
            menuItem3.setTextFillColor(Color(160, 180, 210));
        }
        else if (menuItem2.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::PURCHASE_HISTORY;
            menuItem1.setNormalColor(Color(25, 45, 80, 150));
            menuItem1.setTextFillColor(Color(160, 180, 210));
            
            menuItem2.setNormalColor(Color(60, 130, 210, 200)); // Active
            menuItem2.setTextFillColor(Color(220, 240, 255));
            
            menuItem3.setNormalColor(Color(25, 45, 80, 150));
            menuItem3.setTextFillColor(Color(160, 180, 210));
        }
        else if (menuItem3.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::MY_GIFTS;
            menuItem1.setNormalColor(Color(25, 45, 80, 150));
            menuItem1.setTextFillColor(Color(160, 180, 210));
            
            menuItem2.setNormalColor(Color(25, 45, 80, 150));
            menuItem2.setTextFillColor(Color(160, 180, 210));
            
            menuItem3.setNormalColor(Color(60, 130, 210, 200)); // Active
            menuItem3.setTextFillColor(Color(220, 240, 255));
        }
    }
    
    // Only handle input for CUSTOMER_INFO tab
    if (currentTab != AccountTab::CUSTOMER_INFO) {
        return;
    }
    
    // Handle cursor blinking
    if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
    
    // Hide messages after 2 seconds
    if (showInfoMessage && messageTimerInfo.getElapsedTime().asSeconds() >= 2.0f) {
        showInfoMessage = false;
    }
    if (showPasswordMessage && messageTimerPassword.getElapsedTime().asSeconds() >= 2.0f) {
        showPasswordMessage = false;
    }
    
    // Handle keyboard input
    if (event) {
        if (auto* keyEvent = event->getIf<Event::KeyPressed>()) {
            auto code = keyEvent->code;
            
            // Tab to switch between fields
            if (code == Keyboard::Key::Tab) {
                if (activeInfoField >= 0) {
                    activeInfoField = (activeInfoField + 1) % 3; // Cycle through 0, 1, 2
                    activeField = -1;
                } else if (activeField >= 0) {
                    activeField = (activeField + 1) % 3; // Cycle through password fields
                    activeInfoField = -1;
                }
                showCursor = true;
                cursorClock.restart();
            }
            // Escape to deactivate
            else if (code == Keyboard::Key::Escape) {
                activeInfoField = -1;
                activeField = -1;
            }
        }
    }
    
    // Handle input
    handlePasswordInput(event);
    handleInfoInput(event);
    
    // Check info field clicks
    if (mousePressed) {
        if (fullNameBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 0;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
        }
        else if (birthDateBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 1;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
        }
        else if (phoneBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 2;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
        }
        else if (oldPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 0;
            showCursor = true;
            cursorClock.restart();
        }
        else if (newPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 1;
            showCursor = true;
            cursorClock.restart();
        }
        else if (confirmPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 2;
            showCursor = true;
            cursorClock.restart();
        }
        else if (!saveInfoBtn.getGlobalBounds().contains(mousePos) && 
                 !changePasswordBtn.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = -1;
        }
    }
    
    // Update info box outlines - Focus: viền #09F (0099FF) 2px
    fullNameBox.setOutlineColor(activeInfoField == 0 ? Color(0, 153, 255) : Color(201, 214, 226));
    birthDateBox.setOutlineColor(activeInfoField == 1 ? Color(0, 153, 255) : Color(201, 214, 226));
    phoneBox.setOutlineColor(activeInfoField == 2 ? Color(0, 153, 255) : Color(201, 214, 226));
    
    fullNameBox.setOutlineThickness(activeInfoField == 0 ? 2.f : 1.f);
    birthDateBox.setOutlineThickness(activeInfoField == 1 ? 2.f : 1.f);
    phoneBox.setOutlineThickness(activeInfoField == 2 ? 2.f : 1.f);
    
    // Update password box outlines - Focus: viền #09F 2px
    oldPasswordBox.setOutlineColor(activeField == 0 ? Color(0, 153, 255) : Color(201, 214, 226));
    newPasswordBox.setOutlineColor(activeField == 1 ? Color(0, 153, 255) : Color(201, 214, 226));
    confirmPasswordBox.setOutlineColor(activeField == 2 ? Color(0, 153, 255) : Color(201, 214, 226));
    
    oldPasswordBox.setOutlineThickness(activeField == 0 ? 2.f : 1.f);
    newPasswordBox.setOutlineThickness(activeField == 1 ? 2.f : 1.f);
    confirmPasswordBox.setOutlineThickness(activeField == 2 ? 2.f : 1.f);
    
    // Update buttons using Button class
    saveInfoBtn.update(mousePos);
    changePasswordBtn.update(mousePos);
    
    // Handle button clicks
    if (mousePressed) {
        if (changePasswordBtn.isClicked(mousePos, mousePressed)) {
            savePasswordChange();
        }
        else if (saveInfoBtn.isClicked(mousePos, mousePressed)) {
            saveInfoChange();
        }
    }
}

void AccountScreen::draw(RenderWindow& window) {
    // Update positions
    updatePositions(window.getSize());
    
    // Draw Sidebar
    window.draw(sidebarBg);
    
    // Draw menu items using Button class
    menuItem1.draw(window);
    menuItem2.draw(window);
    menuItem3.draw(window);
    
    // Draw content based on active tab
    if (currentTab == AccountTab::CUSTOMER_INFO) {
        // Draw main white card
        window.draw(mainCardBg);
        
        // Draw Section 1: Thông tin cá nhân
        window.draw(personalInfoTitle);
        
        // Column 1: Họ và tên, Số điện thoại
        window.draw(fullNameLabel);
        window.draw(fullNameBox);
        window.draw(fullNameText);
        
        window.draw(phoneLabel);
        window.draw(phoneBox);
        window.draw(phoneText);
        
        // Column 2: Ngày sinh, Email
        window.draw(birthDateLabel);
        window.draw(birthDateBox);
        window.draw(birthDateText);
        
        window.draw(emailLabel);
        window.draw(emailBox);
        window.draw(emailText);
        
        // Save info button
        saveInfoBtn.draw(window);
        
        // Draw Section 2: Đổi mật khẩu
        window.draw(changePasswordTitle);
        
        window.draw(oldPasswordLabel);
        window.draw(oldPasswordBox);
        window.draw(oldPasswordText);
        
        window.draw(newPasswordLabel);
        window.draw(newPasswordBox);
        window.draw(newPasswordText);
        
        window.draw(confirmPasswordLabel);
        window.draw(confirmPasswordBox);
        window.draw(confirmPasswordText);
        
        // Change password button
        changePasswordBtn.draw(window);
        
        // Draw cursor for active fields
        if (showCursor) {
            if (activeInfoField == 0) { // Full name
                FloatRect bounds = fullNameText.getLocalBounds();
                cursor.setPosition({fullNameText.getPosition().x + bounds.size.x + 2.f, fullNameText.getPosition().y - 2.f});
                window.draw(cursor);
            }
            else if (activeInfoField == 1) { // Birth date
                FloatRect bounds = birthDateText.getLocalBounds();
                cursor.setPosition({birthDateText.getPosition().x + bounds.size.x + 2.f, birthDateText.getPosition().y - 2.f});
                window.draw(cursor);
            }
            else if (activeInfoField == 2) { // Phone
                FloatRect bounds = phoneText.getLocalBounds();
                cursor.setPosition({phoneText.getPosition().x + bounds.size.x + 2.f, phoneText.getPosition().y - 2.f});
                window.draw(cursor);
            }
            else if (activeField == 0) { // Old password
                FloatRect bounds = oldPasswordText.getLocalBounds();
                cursor.setPosition({oldPasswordText.getPosition().x + bounds.size.x + 2.f, oldPasswordText.getPosition().y - 2.f});
                window.draw(cursor);
            }
            else if (activeField == 1) { // New password
                FloatRect bounds = newPasswordText.getLocalBounds();
                cursor.setPosition({newPasswordText.getPosition().x + bounds.size.x + 2.f, newPasswordText.getPosition().y - 2.f});
                window.draw(cursor);
            }
            else if (activeField == 2) { // Confirm password
                FloatRect bounds = confirmPasswordText.getLocalBounds();
                cursor.setPosition({confirmPasswordText.getPosition().x + bounds.size.x + 2.f, confirmPasswordText.getPosition().y - 2.f});
                window.draw(cursor);
            }
        }
        
        // Draw messages
        if (showInfoMessage) {
            window.draw(infoMessage);
        }
        if (showPasswordMessage) {
            window.draw(passwordMessage);
        }
    }
    else if (currentTab == AccountTab::PURCHASE_HISTORY) {
        // TODO: Draw purchase history
        Text placeholder(font, L"Lịch sử mua hàng - Chưa triển khai", 24);
        placeholder.setPosition({400.f, 300.f});
        placeholder.setFillColor(Color::White);
        window.draw(placeholder);
    }
    else if (currentTab == AccountTab::MY_GIFTS) {
        // TODO: Draw gifts
        Text placeholder(font, L"Quà tặng của tôi - Chưa triển khai", 24);
        placeholder.setPosition({400.f, 300.f});
        placeholder.setFillColor(Color::White);
        window.draw(placeholder);
    }
}
