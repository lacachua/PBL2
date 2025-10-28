#include "AccountScreen.h"
#include "PasswordHasher.h"

AccountScreen::AccountScreen(const Font& f, AuthService& auth) 
    : font(f),
      authService(&auth),
      titleText(f, L"THÔNG TIN KHÁCH HÀNG", 28),
      menuText1(f, L"THÔNG TIN KHÁCH HÀNG", 16),
      menuText2(f, L"LỊCH SỬ ĐẶT VÉ", 16),
      menuText3(f, L"QUÀ TẶNG CỦA TÔI", 16),
      personalInfoTitle(f, L"THÔNG TIN CÁ NHÂN", 20),
      usernameLabel(f, L"Username", 16),
      fullNameLabel(f, L"Họ và tên", 16),
      birthDateLabel(f, L"Ngày sinh", 16),
      phoneLabel(f, L"Số điện thoại", 16),
      emailLabel(f, L"Email", 16),
      usernameText(f, "", 16),
      fullNameText(font, "", 16),
      birthDateText(font, "", 16),
      phoneText(font, "", 16),
      emailText(font, "", 16),
      changePasswordTitle(font, L"ĐỔI MẬT KHẨU", 20),
      oldPasswordLabel(font, L"Mật khẩu cũ *", 16),
      newPasswordLabel(font, L"Mật khẩu mới *", 16),
      confirmPasswordLabel(font, L"Xác thực mật khẩu *", 16),
      oldPasswordText(font, "", 16),
      newPasswordText(font, "", 16),
      confirmPasswordText(font, "", 16),
      saveInfoBtnText(font, L"LƯU THÔNG TIN", 16),
      changePasswordBtnText(font, L"ĐỔI MẬT KHẨU", 16),
      infoMessage(font, "", 14),
      passwordMessage(font, "", 14)
{
    // Setup title
    titleText.setFillColor(Color::White);
    titleText.setStyle(Text::Bold);
    
    // Setup section titles - Montserrat SemiBold, #EEEEEE
    personalInfoTitle.setFillColor(Color(238, 238, 238));
    personalInfoTitle.setStyle(Text::Bold);
    changePasswordTitle.setFillColor(Color(238, 238, 238));
    changePasswordTitle.setStyle(Text::Bold);
    
    // Setup labels - Montserrat Medium, #BFD7EA
    usernameLabel.setFillColor(Color(191, 215, 234));
    fullNameLabel.setFillColor(Color(191, 215, 234));
    birthDateLabel.setFillColor(Color(191, 215, 234));
    phoneLabel.setFillColor(Color(191, 215, 234));
    emailLabel.setFillColor(Color(191, 215, 234));
    oldPasswordLabel.setFillColor(Color(191, 215, 234));
    newPasswordLabel.setFillColor(Color(191, 215, 234));
    confirmPasswordLabel.setFillColor(Color(191, 215, 234));
    
    // Setup input boxes - Personal Info (376px x 40px)
    fullNameBox.setSize({376.f, 40.f});
    birthDateBox.setSize({376.f, 40.f});
    phoneBox.setSize({376.f, 40.f});
    emailBox.setSize({376.f, 40.f});
    
    // Input background: #F5F5F5
    usernameBox.setFillColor(Color(245, 245, 245));
    fullNameBox.setFillColor(Color(245, 245, 245));
    birthDateBox.setFillColor(Color(245, 245, 245));
    phoneBox.setFillColor(Color(245, 245, 245));
    emailBox.setFillColor(Color(245, 245, 245));
    
    usernameBox.setOutlineThickness(1.f);
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineThickness(1.f);
    emailBox.setOutlineThickness(1.f);
    
    // Viền thường: #C9D6E2
    usernameBox.setOutlineColor(Color(201, 214, 226));
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineColor(Color(201, 214, 226));
    emailBox.setOutlineColor(Color(201, 214, 226));
    
    // Setup input text - chữ: #0D1B2A
    usernameText.setFillColor(Color(13, 27, 42));
    fullNameText.setFillColor(Color(13, 27, 42));
    birthDateText.setFillColor(Color(13, 27, 42));
    phoneText.setFillColor(Color(13, 27, 42));
    emailText.setFillColor(Color(13, 27, 42));
    
    // Setup input boxes - Password Change (376px x 40px)
    oldPasswordBox.setSize({376.f, 40.f});
    newPasswordBox.setSize({376.f, 40.f});
    confirmPasswordBox.setSize({376.f, 40.f});
    
    oldPasswordBox.setFillColor(Color(245, 245, 245));
    newPasswordBox.setFillColor(Color(245, 245, 245));
    confirmPasswordBox.setFillColor(Color(245, 245, 245));
    
    oldPasswordBox.setOutlineThickness(1.f);
    newPasswordBox.setOutlineThickness(1.f);
    confirmPasswordBox.setOutlineThickness(1.f);
    
    oldPasswordBox.setOutlineColor(Color(201, 214, 226));
    newPasswordBox.setOutlineColor(Color(201, 214, 226));
    confirmPasswordBox.setOutlineColor(Color(201, 214, 226));
    
    // Setup password text - chữ: #0D1B2A
    oldPasswordText.setFillColor(Color(13, 27, 42));
    newPasswordText.setFillColor(Color(13, 27, 42));
    confirmPasswordText.setFillColor(Color(13, 27, 42));
    
    // Setup buttons - spec: 160x40
    saveInfoBtn.setSize({160.f, 40.f});
    changePasswordBtn.setSize({160.f, 40.f});
    
    saveInfoBtn.setFillColor(Color(0, 153, 255)); // #0099FF
    changePasswordBtn.setFillColor(Color(0, 153, 255));
    
    saveInfoBtnText.setFillColor(Color::White);
    changePasswordBtnText.setFillColor(Color::White);
    saveInfoBtnText.setStyle(Text::Bold);
    changePasswordBtnText.setStyle(Text::Bold);
    
    // Setup cursor - màu cyan
    cursor.setSize({2.f, 20.f});
    cursor.setFillColor(Color(0, 153, 255));
    
    // Setup message texts
    infoMessage.setFillColor(Color(60, 160, 90)); // Green for success
    passwordMessage.setFillColor(Color(60, 160, 90));
    
    // Setup sidebar - spec: 240px width
    sidebarBg.setSize({240.f, 550.f});
    sidebarBg.setFillColor(Color(15, 35, 65, 200));
    
    // Setup menu items
    menuItem1.setSize({220.f, 52.f});
    menuItem2.setSize({220.f, 52.f});
    menuItem3.setSize({220.f, 52.f});
    
    menuItem1.setFillColor(Color(60, 130, 210, 200));
    menuItem2.setFillColor(Color(25, 45, 80, 150));
    menuItem3.setFillColor(Color(25, 45, 80, 150));
    
    // Setup menu texts - màu sáng phù hợp với background xanh
    menuText1.setFillColor(Color(220, 240, 255)); // Light blue-white for active
    menuText2.setFillColor(Color(160, 180, 210)); // Lighter gray-blue for inactive
    menuText3.setFillColor(Color(160, 180, 210));
    menuText1.setStyle(Text::Bold);
    
    // Setup menu icon backgrounds - lighter blue
    menuIcon1Bg.setSize({40.f, 40.f});
    menuIcon2Bg.setSize({40.f, 40.f});
    menuIcon3Bg.setSize({40.f, 40.f});
    
    menuIcon1Bg.setFillColor(Color(100, 160, 220, 150)); // Active light blue
    menuIcon2Bg.setFillColor(Color(50, 80, 120, 100)); // Inactive mid blue
    menuIcon3Bg.setFillColor(Color(50, 80, 120, 100));
    
    // Setup icons (simple circles for now)
    menuIcon1.setRadius(7.f);
    menuIcon2.setRadius(7.f);
    menuIcon3.setRadius(7.f);
    
    menuIcon1.setFillColor(Color::White);
    menuIcon2.setFillColor(Color(160, 180, 200));
    menuIcon3.setFillColor(Color(160, 180, 200));
}

void AccountScreen::setCurrentUser(const string& email) {
    currentUserEmail = email;
    currentUser = authService->getUser(email);
    
    if (currentUser) {
        // Set text displays
        usernameText.setString(currentUser->username);  // Show username
        fullNameText.setString(currentUser->fullName);
        birthDateText.setString(currentUser->birthDate);
        phoneText.setString(currentUser->phone);
        emailText.setString(currentUser->email);
        
        // Initialize editable inputs (không bao gồm username và email)
        fullNameInput = wstring(currentUser->fullName.begin(), currentUser->fullName.end());
        birthDateInput = wstring(currentUser->birthDate.begin(), currentUser->birthDate.end());
        phoneInput = wstring(currentUser->phone.begin(), currentUser->phone.end());
    }
}

void AccountScreen::updatePositions(Vector2u windowSize) {
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Sidebar positioning - spec: 240px width
    float sidebarWidth = 240.f;
    sidebarBg.setPosition({50.f, 170.f});
    
    float sidebarX = sidebarBg.getPosition().x;
    float sidebarY = sidebarBg.getPosition().y;
    float menuStartY = sidebarY + 40.f;
    float menuItemHeight = 70.f;
    float menuItemWidth = 220.f;
    
    // Menu items
    menuItem1.setSize({menuItemWidth, 52.f});
    menuItem1.setPosition({sidebarX + 10.f, menuStartY});
    menuIcon1Bg.setPosition({menuItem1.getPosition().x + 10.f, menuItem1.getPosition().y + 6.f});
    menuIcon1.setPosition({menuIcon1Bg.getPosition().x + 13.f, menuIcon1Bg.getPosition().y + 13.f});
    menuText1.setPosition({menuItem1.getPosition().x + 58.f, menuItem1.getPosition().y + 14.f});
    
    menuItem2.setSize({menuItemWidth, 52.f});
    menuItem2.setPosition({sidebarX + 10.f, menuStartY + menuItemHeight});
    menuIcon2Bg.setPosition({menuItem2.getPosition().x + 10.f, menuItem2.getPosition().y + 6.f});
    menuIcon2.setPosition({menuIcon2Bg.getPosition().x + 13.f, menuIcon2Bg.getPosition().y + 13.f});
    menuText2.setPosition({menuItem2.getPosition().x + 58.f, menuItem2.getPosition().y + 14.f});
    
    menuItem3.setSize({menuItemWidth, 52.f});
    menuItem3.setPosition({sidebarX + 10.f, menuStartY + menuItemHeight * 2});
    menuIcon3Bg.setPosition({menuItem3.getPosition().x + 10.f, menuItem3.getPosition().y + 6.f});
    menuIcon3.setPosition({menuIcon3Bg.getPosition().x + 13.f, menuIcon3Bg.getPosition().y + 13.f});
    menuText3.setPosition({menuItem3.getPosition().x + 58.f, menuItem3.getPosition().y + 14.f});
    
    // Content area - spec cho 1366x768
    // contentLeft = sidebarWidth + 20 = 260px
    float contentLeft = 260.f;
    float contentTop = 150.f;
    float cardGap = 40.f;
    float cardWidth = 420.f;
    float cardHeight = 380.f;
    float cardPaddingLeft = 22.f;
    
    // Responsive adjustments cho màn hình lớn
    if (windowWidth >= 1280.f) {
        contentLeft = max(260.f, sidebarWidth + 0.05f * windowWidth);
        cardWidth = min(480.f, max(380.f, 0.30f * windowWidth));
        cardGap = min(56.f, max(32.f, 0.03f * windowWidth));
        contentTop = max(130.f, 0.2f * windowHeight);
    }
    
    // Personal Information Card position (260, 150)
    infoBg.setSize({ cardWidth, cardHeight });
    infoBg.setPosition({contentLeft, contentTop});
    
    // Title position (cardPaddingLeft, 18px)
    personalInfoTitle.setPosition({contentLeft + cardPaddingLeft, contentTop + 18.f});
    
    // Card content - padding 22px, label cách title 18px
    float leftCardX = contentLeft + cardPaddingLeft;
    float topY = contentTop + 18.f + 18.f; // title + 18px spacing
    float inputVGap = 14.f; // Spacing giữa các input
    
    // Họ và tên - Row 1
    fullNameLabel.setPosition({leftCardX, topY});
    fullNameBox.setPosition({leftCardX, topY + 14.f + 8.f}); // label height + 8px
    fullNameText.setPosition({leftCardX + 10, topY + 14.f + 8.f + 11.f});
    
    // Ngày sinh - Row 2
    float row2Y = topY + 14.f + 8.f + 40.f + inputVGap;
    birthDateLabel.setPosition({leftCardX, row2Y});
    birthDateBox.setPosition({leftCardX, row2Y + 14.f + 8.f});
    birthDateText.setPosition({leftCardX + 10, row2Y + 14.f + 8.f + 11.f});
    
    // Số điện thoại - Row 3
    float row3Y = row2Y + 14.f + 8.f + 40.f + inputVGap;
    phoneLabel.setPosition({leftCardX, row3Y});
    phoneBox.setPosition({leftCardX, row3Y + 14.f + 8.f});
    phoneText.setPosition({leftCardX + 10, row3Y + 14.f + 8.f + 11.f});
    
    // Email - Row 4
    float row4Y = row3Y + 14.f + 8.f + 40.f + inputVGap;
    emailLabel.setPosition({leftCardX, row4Y});
    emailBox.setPosition({leftCardX, row4Y + 14.f + 8.f});
    emailText.setPosition({leftCardX + 10, row4Y + 14.f + 8.f + 11.f});
    
    // Save Info Button - 24px dưới input cuối
    float btnY = row4Y + 14.f + 8.f + 40.f + 24.f;
    saveInfoBtn.setPosition({leftCardX, btnY});
    FloatRect saveTextBounds = saveInfoBtnText.getLocalBounds();
    saveInfoBtnText.setPosition({
        leftCardX + 80.f - saveTextBounds.size.x / 2.f,
        btnY + 11.f
    });
    
    // Info message below save button
    infoMessage.setPosition({leftCardX, btnY + 45.f});
    
    // Password Change Card (Right side) - spec: (720, 150)
    float passwordCardX = contentLeft + cardWidth + cardGap;
    float rightCardX = passwordCardX + cardPaddingLeft;
    
    passwordBg.setSize({ cardWidth, cardHeight });
    passwordBg.setPosition({passwordCardX, contentTop});
    
    // Title
    changePasswordTitle.setPosition({passwordCardX + cardPaddingLeft, contentTop + 18.f});
    
    // Password fields - giống layout bên trái
    float pwTopY = contentTop + 18.f + 18.f;
    
    // Old Password - Row 1
    oldPasswordLabel.setPosition({rightCardX, pwTopY});
    oldPasswordBox.setPosition({rightCardX, pwTopY + 14.f + 8.f});
    oldPasswordText.setPosition({rightCardX + 10, pwTopY + 14.f + 8.f + 11.f});
    
    // New Password - Row 2
    float pwRow2Y = pwTopY + 14.f + 8.f + 40.f + inputVGap;
    newPasswordLabel.setPosition({rightCardX, pwRow2Y});
    newPasswordBox.setPosition({rightCardX, pwRow2Y + 14.f + 8.f});
    newPasswordText.setPosition({rightCardX + 10, pwRow2Y + 14.f + 8.f + 11.f});
    
    // Confirm Password - Row 3
    float pwRow3Y = pwRow2Y + 14.f + 8.f + 40.f + inputVGap;
    confirmPasswordLabel.setPosition({rightCardX, pwRow3Y});
    confirmPasswordBox.setPosition({rightCardX, pwRow3Y + 14.f + 8.f});
    confirmPasswordText.setPosition({rightCardX + 10, pwRow3Y + 14.f + 8.f + 11.f});
    
    // Change Password Button - 24px dưới input cuối
    float pwBtnY = pwRow3Y + 14.f + 8.f + 40.f + 24.f;
    changePasswordBtn.setPosition({rightCardX, pwBtnY});
    FloatRect changeTextBounds = changePasswordBtnText.getLocalBounds();
    changePasswordBtnText.setPosition({
        rightCardX + 80.f - changeTextBounds.size.x / 2.f,
        pwBtnY + 11.f
    });
    
    // Password message below change password button
    passwordMessage.setPosition({rightCardX, pwBtnY + 45.f});
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
    // Handle menu clicks
    if (mousePressed) {
        if (menuItem1.getGlobalBounds().contains(mousePos)) {
            currentTab = AccountTab::CUSTOMER_INFO;
            // Update menu colors - blue theme
            menuItem1.setFillColor(Color(60, 130, 210, 200)); // Active
            menuItem2.setFillColor(Color(25, 45, 80, 150)); // Inactive
            menuItem3.setFillColor(Color(25, 45, 80, 150));
            
            menuIcon1Bg.setFillColor(Color(100, 160, 220, 150));
            menuIcon2Bg.setFillColor(Color(50, 80, 120, 100));
            menuIcon3Bg.setFillColor(Color(50, 80, 120, 100));
            
            menuIcon1.setFillColor(Color::White);
            menuIcon2.setFillColor(Color(160, 180, 200));
            menuIcon3.setFillColor(Color(160, 180, 200));
            
            menuText1.setFillColor(Color(220, 240, 255));
            menuText2.setFillColor(Color(160, 180, 210));
            menuText3.setFillColor(Color(160, 180, 210));
            menuText1.setStyle(Text::Bold);
            menuText2.setStyle(Text::Regular);
            menuText3.setStyle(Text::Regular);
        }
        else if (menuItem2.getGlobalBounds().contains(mousePos)) {
            currentTab = AccountTab::PURCHASE_HISTORY;
            menuItem1.setFillColor(Color(25, 45, 80, 150));
            menuItem2.setFillColor(Color(60, 130, 210, 200)); // Active
            menuItem3.setFillColor(Color(25, 45, 80, 150));
            
            menuIcon1Bg.setFillColor(Color(50, 80, 120, 100));
            menuIcon2Bg.setFillColor(Color(100, 160, 220, 150));
            menuIcon3Bg.setFillColor(Color(50, 80, 120, 100));
            
            menuIcon1.setFillColor(Color(160, 180, 200));
            menuIcon2.setFillColor(Color::White);
            menuIcon3.setFillColor(Color(160, 180, 200));
            
            menuText1.setFillColor(Color(160, 180, 210));
            menuText2.setFillColor(Color(220, 240, 255));
            menuText3.setFillColor(Color(160, 180, 210));
            menuText1.setStyle(Text::Regular);
            menuText2.setStyle(Text::Bold);
            menuText3.setStyle(Text::Regular);
        }
        else if (menuItem3.getGlobalBounds().contains(mousePos)) {
            currentTab = AccountTab::MY_GIFTS;
            menuItem1.setFillColor(Color(25, 45, 80, 150));
            menuItem2.setFillColor(Color(25, 45, 80, 150));
            menuItem3.setFillColor(Color(60, 130, 210, 200)); // Active
            
            menuIcon1Bg.setFillColor(Color(50, 80, 120, 100));
            menuIcon2Bg.setFillColor(Color(50, 80, 120, 100));
            menuIcon3Bg.setFillColor(Color(100, 160, 220, 150));
            
            menuIcon1.setFillColor(Color(160, 180, 200));
            menuIcon2.setFillColor(Color(160, 180, 200));
            menuIcon3.setFillColor(Color::White);
            
            menuText1.setFillColor(Color(160, 180, 210));
            menuText2.setFillColor(Color(160, 180, 210));
            menuText3.setFillColor(Color(220, 240, 255));
            menuText1.setStyle(Text::Regular);
            menuText2.setStyle(Text::Regular);
            menuText3.setStyle(Text::Bold);
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
    
    // Check button hovers - hover cyan sáng hơn
    isSaveInfoHovered = saveInfoBtn.getGlobalBounds().contains(mousePos);
    isChangePasswordHovered = changePasswordBtn.getGlobalBounds().contains(mousePos);
    
    // Update button colors - #00BFFF khi hover
    saveInfoBtn.setFillColor(isSaveInfoHovered ? Color(0, 191, 255) : Color(0, 153, 255));
    changePasswordBtn.setFillColor(isChangePasswordHovered ? Color(0, 191, 255) : Color(0, 153, 255));
    
    // Handle button clicks
    if (mousePressed) {
        if (isChangePasswordHovered) {
            savePasswordChange();
        }
        else if (isSaveInfoHovered) {
            saveInfoChange();
        }
    }
}

void AccountScreen::draw(RenderWindow& window) {
    // Don't clear or draw header - HomeScreen already did that
    // Background gradient is already beautiful, no need for overlay!
    
    updatePositions(window.getSize());
    
    // Draw Sidebar
    window.draw(sidebarBg);
    
    // Draw menu items
    window.draw(menuItem1);
    window.draw(menuIcon1Bg);
    window.draw(menuIcon1);
    window.draw(menuText1);
    
    window.draw(menuItem2);
    window.draw(menuIcon2Bg);
    window.draw(menuIcon2);
    window.draw(menuText2);
    
    window.draw(menuItem3);
    window.draw(menuIcon3Bg);
    window.draw(menuIcon3);
    window.draw(menuText3);
    
    // Draw content based on active tab
    if (currentTab == AccountTab::CUSTOMER_INFO) {
        // Định nghĩa kích thước và vị trí cards theo spec
        float contentLeft = 260.f;
        float contentTop = 150.f;
        float cardGap = 40.f;
        float cardWidth = 420.f;
        float cardHeight = 380.f;
        
        // Responsive adjustments
        if (window.getSize().x >= 1280) {
            contentLeft = max(260.f, 240.f + 0.05f * window.getSize().x);
            cardWidth = min(480.f, max(380.f, 0.30f * window.getSize().x));
            cardGap = min(56.f, max(32.f, 0.03f * window.getSize().x));
            contentTop = max(130.f, 0.2f * window.getSize().y);
        }
        
        // Shadow cho personal info card - spec: (0, 6px, 20px, #00000033)
        RectangleShape personalShadow({cardWidth + 8.f, cardHeight + 8.f});
        personalShadow.setPosition({contentLeft - 4.f, contentTop + 2.f});
        personalShadow.setFillColor(Color(0, 0, 0, 51)); // #00000033
        window.draw(personalShadow);
        
        // Personal info card - nền: #001830 alpha 0.92
        RectangleShape personalCard({cardWidth, cardHeight});
        personalCard.setPosition({contentLeft, contentTop});
        personalCard.setFillColor(Color(0, 24, 48, 235)); // rgba(0,24,48,0.92)
        personalCard.setOutlineThickness(1.f);
        personalCard.setOutlineColor(Color(10, 51, 92)); // #0A335C
        window.draw(personalCard);
        
        // Shadow cho password card
        float passwordCardX = contentLeft + cardWidth + cardGap;
        RectangleShape passwordShadow({cardWidth + 8.f, cardHeight + 8.f});
        passwordShadow.setPosition({passwordCardX - 4.f, contentTop + 2.f});
        passwordShadow.setFillColor(Color(0, 0, 0, 51));
        window.draw(passwordShadow);
        
        // Password card - nền: #001830 alpha 0.92
        RectangleShape passwordCard({cardWidth, cardHeight});
        passwordCard.setPosition({passwordCardX, contentTop});
        passwordCard.setFillColor(Color(0, 24, 48, 235));
        passwordCard.setOutlineThickness(1.f);
        passwordCard.setOutlineColor(Color(10, 51, 92));
        window.draw(passwordCard);
        
        // Draw Personal Info Section
        window.draw(personalInfoTitle);
        
        // Chỉ hiển thị 4 field theo layout dọc (không có username)
        window.draw(fullNameLabel);
        window.draw(fullNameBox);
        window.draw(fullNameText);
        
        window.draw(birthDateLabel);
        window.draw(birthDateBox);
        window.draw(birthDateText);
        
        window.draw(phoneLabel);
        window.draw(phoneBox);
        window.draw(phoneText);
        
        window.draw(emailLabel);
        window.draw(emailBox);
        window.draw(emailText);
        
        window.draw(saveInfoBtn);
        window.draw(saveInfoBtnText);
        
        // Draw Change Password Section
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
        
        window.draw(changePasswordBtn);
        window.draw(changePasswordBtnText);
        
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
    
    // Draw messages if active
    if (showInfoMessage) {
        window.draw(infoMessage);
    }
    if (showPasswordMessage) {
        window.draw(passwordMessage);
    }
    }
    else if (currentTab == AccountTab::PURCHASE_HISTORY) {
        // Placeholder for purchase history
        Text placeholderText(font, L"Lịch sử đặt vé", 32);
        placeholderText.setFillColor(Color::White);
        placeholderText.setStyle(Text::Bold);
        float contentX = sidebarBg.getPosition().x + sidebarBg.getSize().x + 100.f;
        placeholderText.setPosition({contentX, 200.f});
        window.draw(placeholderText);
        
        Text subText(font, L"Chức năng đang được phát triển...", 20);
        subText.setFillColor(Color(200, 200, 200));
        subText.setPosition({contentX, 250.f});
        window.draw(subText);
    }
    else if (currentTab == AccountTab::MY_GIFTS) {
        // Placeholder for my gifts
        Text placeholderText(font, L"Quà tặng của tôi", 32);
        placeholderText.setFillColor(Color::White);
        placeholderText.setStyle(Text::Bold);
        float contentX = sidebarBg.getPosition().x + sidebarBg.getSize().x + 100.f;
        placeholderText.setPosition({contentX, 200.f});
        window.draw(placeholderText);
        
        Text subText(font, L"Chức năng đang được phát triển...", 20);
        subText.setFillColor(Color(200, 200, 200));
        subText.setPosition({contentX, 250.f});
        window.draw(subText);
    }
}
