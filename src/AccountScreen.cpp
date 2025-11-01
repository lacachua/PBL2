#include "AccountScreen.h"
#include "PasswordHasher.h"
#ifdef _WIN32
#include <windows.h>
#endif

// Helper function: Convert UTF-8 string to wstring
static std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return L"";
    
#ifdef _WIN32
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstrTo[0], size_needed);
    return wstrTo;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
#endif
}

// Helper function: Convert wstring to UTF-8 string
static std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    
#ifdef _WIN32
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
#endif
}

AccountScreen::AccountScreen(const Font& f, AuthService& auth) 
    : font(f),
      authService(&auth),
      titleText(f, L"THÔNG TIN KHÁCH HÀNG", 28),
      menuItem1(f, L"THÔNG TIN KHÁCH HÀNG", 300.f, 60.f, 18),
      menuItem2(f, L"LỊCH SỬ ĐẶT VÉ", 300.f, 60.f, 18),
      menuItem3(f, L"VOUCHER CỦA TÔI", 300.f, 60.f, 18),
      personalInfoTitle(f, L"THÔNG TIN CÁ NHÂN", 22),
      fullNameLabel(f, L"Họ và tên", 16),
      birthDateLabel(f, L"Ngày sinh", 16),
      phoneLabel(f, L"Số điện thoại", 16),
      emailLabel(f, L"Email", 16),
      fullNameText(font, "", 16),
      birthDateText(font, "", 16),
      phoneText(font, "", 16),
      emailText(font, "", 16),
      changePasswordTitle(font, L"ĐỔI MẬT KHẨU", 22),
      oldPasswordLabel(font, L"Mật khẩu cũ *", 16),
      newPasswordLabel(font, L"Mật khẩu mới *", 16),
      confirmPasswordLabel(font, L"Xác thực mật khẩu *", 16),
      oldPasswordText(font, "", 16),
      newPasswordText(font, "", 16),
      confirmPasswordText(font, "", 16),
      saveInfoBtn(f, L"LƯU THÔNG TIN", 260.f, 50.f, 18),
      changePasswordBtn(f, L"ĐỔI MẬT KHẨU", 260.f, 50.f, 18),
      infoMessage(font, "", 15),
      passwordMessage(font, "", 15)
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
    
    // Setup input boxes - tăng kích thước để cân đối
    fullNameBox.setSize({280.f, 42.f});
    birthDateBox.setSize({280.f, 42.f});
    phoneBox.setSize({280.f, 42.f});
    emailBox.setSize({280.f, 42.f});
    
    // Input background: White (editable), #E9ECEF (read-only email)
    fullNameBox.setFillColor(Color::White);
    birthDateBox.setFillColor(Color::White);
    phoneBox.setFillColor(Color::White);
    emailBox.setFillColor(Color(233, 236, 239)); // #E9ECEF - disabled/read-only
    
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineThickness(1.f);
    emailBox.setOutlineThickness(1.f);
    
    // Border color - xám nhạt, email darker
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineColor(Color(201, 214, 226));
    emailBox.setOutlineColor(Color(200, 208, 217)); // #C8D0D9 - read-only border
    
    // Setup input text - đen, email read-only màu xám
    fullNameText.setFillColor(Color(13, 27, 42));
    birthDateText.setFillColor(Color(13, 27, 42));
    phoneText.setFillColor(Color(13, 27, 42));
    emailText.setFillColor(Color(107, 114, 128)); // #6B7280 - read-only color
    
    // Setup input boxes - Password Change (width thẳng hàng với col2)
    // col2X - col1X + inputWidth = total width for password boxes
    oldPasswordBox.setSize({604.f, 42.f}); // 280 + 24 + 12 + 280 + 8 (rounded)
    newPasswordBox.setSize({604.f, 42.f});
    confirmPasswordBox.setSize({604.f, 42.f});
    
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
    
    // Setup sidebar - sẽ tính width động = 1/4 màn hình trong updatePositions
    sidebarBg.setSize({340.f, 768.f}); // Default size, sẽ update trong updatePositions
    sidebarBg.setFillColor(Color(20, 35, 60, 240)); // Navy đậm hơn
    
    // Setup menu items theo ảnh:
    // - Active item: xanh sáng (THÔNG TIN KHÁCH HÀNG)
    // - Inactive items: xanh đậm/navy tối
    menuItem1.setNormalColor(Color(65, 135, 220, 255)); // Active - xanh sáng nổi bật
    menuItem1.setHoverColor(Color(85, 155, 235, 255));
    menuItem1.setTextFillColor(Color::White); // Text trắng

    menuItem2.setNormalColor(Color(35, 55, 85, 220)); // Inactive - navy tối
    menuItem2.setHoverColor(Color(50, 70, 100, 230));
    menuItem2.setTextFillColor(Color(180, 195, 215)); // Text xám nhạt

    menuItem3.setNormalColor(Color(35, 55, 85, 220)); // Inactive - navy tối
    menuItem3.setHoverColor(Color(50, 70, 100, 230));
    menuItem3.setTextFillColor(Color(180, 195, 215)); // Text xám nhạt
    
    // Bỏ viền outline của menu items để mượt mà như ảnh
    menuItem1.setOutlineThickness(0.f);
    menuItem2.setOutlineThickness(0.f);
    menuItem3.setOutlineThickness(0.f);
}

void AccountScreen::setCurrentUser(const std::string& email) {
    currentUserEmail = email;
    currentUser = authService->getUser(email);

    if (currentUser) {
        // Hiển thị: CHUYỂN UTF-8 -> sf::String
        fullNameText.setString(sf::String::fromUtf8(
            currentUser->fullName.begin(), currentUser->fullName.end()));
        birthDateText.setString(sf::String::fromUtf8(
            currentUser->birthDate.begin(), currentUser->birthDate.end()));
        phoneText.setString(sf::String::fromUtf8(
            currentUser->phone.begin(), currentUser->phone.end()));
        emailText.setString(sf::String::fromUtf8(
            currentUser->email.begin(), currentUser->email.end()));

        // Input editable: UTF-8 -> wstring (bạn đã có helper)
        fullNameInput  = utf8_to_wstring(currentUser->fullName);
        birthDateInput = utf8_to_wstring(currentUser->birthDate);
        phoneInput     = utf8_to_wstring(currentUser->phone);
    }
}


void AccountScreen::updatePositions(Vector2u windowSize) {
    float windowW = static_cast<float>(windowSize.x);
    float windowH = static_cast<float>(windowSize.y);
    
    // Sidebar chiếm khoảng 22% chiều rộng (hơi rộng hơn 1/4 một chút)
    float sidebarWidth = windowW * 0.22f;
    float sidebarX = 0.f; // Bắt đầu từ lề trái
    float sidebarY = 125.f; // Bắt đầu từ Y=100 để không che logo
    
    // Update sidebar size
    sidebarBg.setSize({sidebarWidth, windowH - sidebarY});
    sidebarBg.setPosition({sidebarX, sidebarY});
    
    // Menu items: căn giữa trong sidebar với khoảng cách đều
    float menuItemWidth = sidebarWidth * 0.85f; // 85% width sidebar
    float menuItemHeight = 60.f; // Tăng height để chữ thoải mái hơn
    float menuStartY = sidebarY + 100.f; // Bắt đầu từ Y=200 (100 + 100)
    float menuSpacing = 90.f; // Khoảng cách đều giữa các nút
    
    // Update size cho menu items
    menuItem1.setSize(menuItemWidth, menuItemHeight);
    menuItem2.setSize(menuItemWidth, menuItemHeight);
    menuItem3.setSize(menuItemWidth, menuItemHeight);
    
    // Căn giữa các menu items theo chiều ngang
    float menuX = sidebarX + (sidebarWidth - menuItemWidth) / 2.f;
    
    menuItem1.setPosition(menuX, menuStartY);
    menuItem2.setPosition(menuX, menuStartY + menuSpacing);
    menuItem3.setPosition(menuX, menuStartY + menuSpacing * 2);
    
    // Main card - mở rộng thêm ~20px width, ~40px height
    float baseLeft = sidebarWidth + 30.f; // bắt đầu vùng content sau sidebar
    float rightMargin = 50.f; // chừa lề phải

    float cardY = sidebarY; // 125.f - cùng tọa độ Y với sidebar
    float cardWidth = 1010.f; // 990 + 20 = 1010
    float cardHeight = 766.f; // 726 + 40 = 766
    float cardPadding = 26.f; // Padding 24-28px theo yêu cầu
    
    // Tính toạ độ X để căn giữa card trong phần còn lại
    float available = max(0.f, windowW - baseLeft - rightMargin);
    float cardX = baseLeft + max(0.f, (available - cardWidth) / 2.f);
    
    mainCardBg.setSize({cardWidth, cardHeight});
    mainCardBg.setPosition({cardX, cardY});
    
    // Section 1: Thông tin cá nhân (top section)
    float section1Y = cardY + cardPadding;
    personalInfoTitle.setPosition({cardX + cardPadding, section1Y});
    
    // Two columns layout for personal info
    float col1X = cardX + cardPadding;
    float colGap = 24.f; // Khoảng cách giữa 2 cột
    float col2Shift = 12.f; // Dịch col2 sang phải 10-14px
    float col2X = cardX + cardPadding + 280.f + colGap + col2Shift; // 280px box + 24px gap + shift
    float inputStartY = section1Y + 45.f; // Below title
    float labelInputGap = 10.f; // Label cách input 9-10px
    float rowGap = 18.f; // Khoảng cách giữa các hàng input 18px
    float rowHeight = 42.f + labelInputGap + rowGap; // inputHeight + gaps
    
    // Column 1: Họ và tên, Số điện thoại
    float labelHeight = 20.f; // Approximate label height
    fullNameLabel.setPosition({col1X, inputStartY});
    fullNameBox.setPosition({col1X, inputStartY + labelHeight + labelInputGap});
    fullNameText.setPosition({col1X + 10.f, inputStartY + labelHeight + labelInputGap + 11.f});
    
    float row2Y = inputStartY + labelHeight + labelInputGap + 42.f + rowGap;
    phoneLabel.setPosition({col1X, row2Y});
    phoneBox.setPosition({col1X, row2Y + labelHeight + labelInputGap});
    phoneText.setPosition({col1X + 10.f, row2Y + labelHeight + labelInputGap + 11.f});
    
    // Column 2: Ngày sinh, Email
    birthDateLabel.setPosition({col2X, inputStartY});
    birthDateBox.setPosition({col2X, inputStartY + labelHeight + labelInputGap});
    birthDateText.setPosition({col2X + 10.f, inputStartY + labelHeight + labelInputGap + 11.f});
    
    emailLabel.setPosition({col2X, row2Y});
    emailBox.setPosition({col2X, row2Y + labelHeight + labelInputGap});
    emailText.setPosition({col2X + 10.f, row2Y + labelHeight + labelInputGap + 11.f});
    
    // Save button for personal info - căn trái với input, tăng 110% height
    float saveInfoBtnY = row2Y + labelHeight + labelInputGap + 42.f + 24.f;
    saveInfoBtn.setSize(260.f * 1.1f, 50.f * 1.1f); // 286 x 55
    saveInfoBtn.setPosition(col1X, saveInfoBtnY);
    infoMessage.setPosition({col1X, saveInfoBtnY + 60.f});
    
    // Section 2: Đổi mật khẩu - dời xuống thêm 16-24px (section gap = 24px)
    float sectionGap = 24.f; // Khoảng cách giữa 2 section
    float section2Y = saveInfoBtnY + 70.f + sectionGap; // Thêm 24px
    changePasswordTitle.setPosition({cardX + cardPadding, section2Y});
    
    float pwdInputStartY = section2Y + 45.f;
    
    // Password fields - full width, áp dụng spacing đều
    oldPasswordLabel.setPosition({col1X, pwdInputStartY});
    oldPasswordBox.setPosition({col1X, pwdInputStartY + labelHeight + labelInputGap});
    oldPasswordText.setPosition({col1X + 10.f, pwdInputStartY + labelHeight + labelInputGap + 11.f});
    
    float pwdRow2Y = pwdInputStartY + labelHeight + labelInputGap + 42.f + rowGap;
    newPasswordLabel.setPosition({col1X, pwdRow2Y});
    newPasswordBox.setPosition({col1X, pwdRow2Y + labelHeight + labelInputGap});
    newPasswordText.setPosition({col1X + 10.f, pwdRow2Y + labelHeight + labelInputGap + 11.f});
    
    float pwdRow3Y = pwdRow2Y + labelHeight + labelInputGap + 42.f + rowGap;
    confirmPasswordLabel.setPosition({col1X, pwdRow3Y});
    confirmPasswordBox.setPosition({col1X, pwdRow3Y + labelHeight + labelInputGap});
    confirmPasswordText.setPosition({col1X + 10.f, pwdRow3Y + labelHeight + labelInputGap + 11.f});
    
    // Change password button - căn trái với input, tăng 110% height
    float changePwdBtnY = pwdRow3Y + labelHeight + labelInputGap + 42.f + 24.f;
    changePasswordBtn.setSize(260.f * 1.1f, 50.f * 1.1f); // 286 x 55
    changePasswordBtn.setPosition(col1X, changePwdBtnY);
    passwordMessage.setPosition({col1X, changePwdBtnY + 60.f});
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
    
    // Convert wstring to UTF-8 string using helper function
    string newFullName = wstring_to_utf8(fullNameInput);
    string newBirthDate = wstring_to_utf8(birthDateInput);
    string newPhone = wstring_to_utf8(phoneInput);
    
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
    
    // Handle menu clicks - update theo màu mới giống ảnh
    if (mousePressed) {
        if (menuItem1.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::CUSTOMER_INFO;
            // Active: xanh sáng, Inactive: navy tối
            menuItem1.setNormalColor(Color(65, 135, 220, 255)); // Active
            menuItem1.setTextFillColor(Color::White);
            
            menuItem2.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem2.setTextFillColor(Color(180, 195, 215));
            
            menuItem3.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem3.setTextFillColor(Color(180, 195, 215));
        }
        else if (menuItem2.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::PURCHASE_HISTORY;
            menuItem1.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem1.setTextFillColor(Color(180, 195, 215));
            
            menuItem2.setNormalColor(Color(65, 135, 220, 255)); // Active
            menuItem2.setTextFillColor(Color::White);
            
            menuItem3.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem3.setTextFillColor(Color(180, 195, 215));
        }
        else if (menuItem3.isClicked(mousePos, mousePressed)) {
            currentTab = AccountTab::MY_GIFTS;
            menuItem1.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem1.setTextFillColor(Color(180, 195, 215));
            
            menuItem2.setNormalColor(Color(35, 55, 85, 220)); // Inactive
            menuItem2.setTextFillColor(Color(180, 195, 215));
            
            menuItem3.setNormalColor(Color(65, 135, 220, 255)); // Active
            menuItem3.setTextFillColor(Color::White);
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
    
    // Check info field clicks (email is read-only, skip it)
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
        // Email box is read-only, no click handling
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
    
    // Update info box outlines - Focus: viền #09F (0099FF) 2px, email always read-only
    fullNameBox.setOutlineColor(activeInfoField == 0 ? Color(0, 153, 255) : Color(201, 214, 226));
    birthDateBox.setOutlineColor(activeInfoField == 1 ? Color(0, 153, 255) : Color(201, 214, 226));
    phoneBox.setOutlineColor(activeInfoField == 2 ? Color(0, 153, 255) : Color(201, 214, 226));
    // Email always has read-only border color #C8D0D9
    emailBox.setOutlineColor(Color(200, 208, 217));
    
    fullNameBox.setOutlineThickness(activeInfoField == 0 ? 2.f : 1.f);
    birthDateBox.setOutlineThickness(activeInfoField == 1 ? 2.f : 1.f);
    phoneBox.setOutlineThickness(activeInfoField == 2 ? 2.f : 1.f);
    emailBox.setOutlineThickness(1.f); // Email never gets focus thickness
    
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