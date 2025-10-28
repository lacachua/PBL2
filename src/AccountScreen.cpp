#include "AccountScreen.h"
#include "PasswordHasher.h"

AccountScreen::AccountScreen(const Font& font, AuthService& auth) 
    : authService(&auth),
      titleText(font, L"THÔNG TIN KHÁCH HÀNG", 28),
      personalInfoTitle(font, L"Thông tin cá nhân", 20),
      usernameLabel(font, L"Username", 16),
      fullNameLabel(font, L"Họ và tên", 16),
      birthDateLabel(font, L"Ngày sinh", 16),
      phoneLabel(font, L"Số điện thoại", 16),
      emailLabel(font, L"Email", 16),
      usernameText(font, "", 16),
      fullNameText(font, "", 16),
      birthDateText(font, "", 16),
      phoneText(font, "", 16),
      emailText(font, "", 16),
      changePasswordTitle(font, L"Đổi mật khẩu", 20),
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
    
    // Setup section titles
    personalInfoTitle.setFillColor(Color::Black);
    personalInfoTitle.setStyle(Text::Bold);
    changePasswordTitle.setFillColor(Color::Black);
    changePasswordTitle.setStyle(Text::Bold);
    
    // Setup labels
    usernameLabel.setFillColor(Color::Black);
    fullNameLabel.setFillColor(Color::Black);
    birthDateLabel.setFillColor(Color::Black);
    phoneLabel.setFillColor(Color::Black);
    emailLabel.setFillColor(Color::Black);
    oldPasswordLabel.setFillColor(Color::Black);
    newPasswordLabel.setFillColor(Color::Black);
    confirmPasswordLabel.setFillColor(Color::Black);
    
    // Setup input boxes - Personal Info (editable except username and email)
    usernameBox.setSize({310.f, 45.f});
    fullNameBox.setSize({310.f, 45.f});
    birthDateBox.setSize({310.f, 45.f});
    phoneBox.setSize({650.f, 45.f});
    emailBox.setSize({310.f, 45.f});
    
    usernameBox.setFillColor(Color(240, 240, 240)); // Gray for read-only
    fullNameBox.setFillColor(Color::White);
    birthDateBox.setFillColor(Color::White);
    phoneBox.setFillColor(Color::White);
    emailBox.setFillColor(Color(240, 240, 240)); // Gray for read-only
    
    usernameBox.setOutlineThickness(1.f);
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineThickness(1.f);
    emailBox.setOutlineThickness(1.f);
    
    usernameBox.setOutlineColor(Color(150, 150, 150));
    fullNameBox.setOutlineColor(Color(150, 150, 150));
    birthDateBox.setOutlineColor(Color(150, 150, 150));
    phoneBox.setOutlineColor(Color(150, 150, 150));
    emailBox.setOutlineColor(Color(150, 150, 150));
    
    // Setup input text - Personal Info
    usernameText.setFillColor(Color(100, 100, 100)); // Gray for read-only
    fullNameText.setFillColor(Color::Black);
    birthDateText.setFillColor(Color::Black);
    phoneText.setFillColor(Color::Black);
    emailText.setFillColor(Color(100, 100, 100)); // Gray for read-only
    
    // Setup input boxes - Password Change
    oldPasswordBox.setSize({580.f, 45.f});
    newPasswordBox.setSize({580.f, 45.f});
    confirmPasswordBox.setSize({580.f, 45.f});
    
    oldPasswordBox.setFillColor(Color::White);
    newPasswordBox.setFillColor(Color::White);
    confirmPasswordBox.setFillColor(Color::White);
    
    oldPasswordBox.setOutlineThickness(1.f);
    newPasswordBox.setOutlineThickness(1.f);
    confirmPasswordBox.setOutlineThickness(1.f);
    
    oldPasswordBox.setOutlineColor(Color(150, 150, 150));
    newPasswordBox.setOutlineColor(Color(150, 150, 150));
    confirmPasswordBox.setOutlineColor(Color(150, 150, 150));
    
    // Setup password text
    oldPasswordText.setFillColor(Color(100, 100, 100));
    newPasswordText.setFillColor(Color(100, 100, 100));
    confirmPasswordText.setFillColor(Color(100, 100, 100));
    
    // Setup buttons
    saveInfoBtn.setSize({150.f, 45.f});
    changePasswordBtn.setSize({150.f, 45.f});
    
    saveInfoBtn.setFillColor(Color(20, 118, 172)); // #1476AC
    changePasswordBtn.setFillColor(Color(20, 118, 172));
    
    saveInfoBtnText.setFillColor(Color::White);
    changePasswordBtnText.setFillColor(Color::White);
    saveInfoBtnText.setStyle(Text::Bold);
    changePasswordBtnText.setStyle(Text::Bold);
    
    // Setup cursor
    cursor.setSize({2.f, 20.f});
    cursor.setFillColor(Color::Black);
    
    // Setup message texts
    infoMessage.setFillColor(Color(60, 160, 90)); // Green for success
    passwordMessage.setFillColor(Color(60, 160, 90));
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
    float centerX = windowSize.x / 2.f;
    
    // Personal Info Card (Left side)
    float leftCardX = centerX - 700.f;
    float topY = 180.f;
    
    personalInfoTitle.setPosition({leftCardX, topY});
    
    // Username (read-only) - Row 1 Left
    usernameLabel.setPosition({leftCardX, topY + 50});
    usernameBox.setPosition({leftCardX, topY + 75});
    usernameText.setPosition({leftCardX + 10, topY + 85});
    
    // Email (read-only) - Row 1 Right
    emailLabel.setPosition({leftCardX + 340, topY + 50});
    emailBox.setPosition({leftCardX + 340, topY + 75});
    emailText.setPosition({leftCardX + 350, topY + 85});
    
    // Full Name (editable) - Row 2 Left
    fullNameLabel.setPosition({leftCardX, topY + 150});
    fullNameBox.setPosition({leftCardX, topY + 175});
    fullNameText.setPosition({leftCardX + 10, topY + 185});
    
    // Birth Date (editable) - Row 2 Right
    birthDateLabel.setPosition({leftCardX + 340, topY + 150});
    birthDateBox.setPosition({leftCardX + 340, topY + 175});
    birthDateText.setPosition({leftCardX + 350, topY + 185});
    
    // Phone (editable) - Row 3
    phoneLabel.setPosition({leftCardX, topY + 250});
    phoneBox.setPosition({leftCardX, topY + 275});
    phoneText.setPosition({leftCardX + 10, topY + 285});
    
    // Save Info Button
    saveInfoBtn.setPosition({leftCardX, topY + 350});
    FloatRect saveTextBounds = saveInfoBtnText.getLocalBounds();
    saveInfoBtnText.setPosition({
        leftCardX + 75.f - saveTextBounds.size.x / 2.f,
        topY + 362.f
    });
    
    // Info message below save button
    infoMessage.setPosition({leftCardX + 160, topY + 362.f});
    
    // Password Change Card (Right side)
    float rightCardX = centerX + 60.f;
    
    changePasswordTitle.setPosition({rightCardX, topY});
    
    // Old Password
    oldPasswordLabel.setPosition({rightCardX, topY + 50});
    oldPasswordBox.setPosition({rightCardX, topY + 75});
    oldPasswordText.setPosition({rightCardX + 10, topY + 85});
    
    // New Password
    newPasswordLabel.setPosition({rightCardX, topY + 150});
    newPasswordBox.setPosition({rightCardX, topY + 175});
    newPasswordText.setPosition({rightCardX + 10, topY + 185});
    
    // Confirm Password
    confirmPasswordLabel.setPosition({rightCardX, topY + 250});
    confirmPasswordBox.setPosition({rightCardX, topY + 275});
    confirmPasswordText.setPosition({rightCardX + 10, topY + 285});
    
    // Change Password Button
    changePasswordBtn.setPosition({rightCardX, topY + 350});
    FloatRect changeTextBounds = changePasswordBtnText.getLocalBounds();
    changePasswordBtnText.setPosition({
        rightCardX + 75.f - changeTextBounds.size.x / 2.f,
        topY + 362.f
    });
    
    // Password message below change password button
    passwordMessage.setPosition({rightCardX + 160, topY + 362.f});
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
    
    // Validate inputs
    if (!newBirthDate.empty() && !Validator::isValidDate(newBirthDate)) {
        infoMessage.setFillColor(Color(200, 60, 60)); // Red for error
        infoMessage.setString(L"Ngày sinh không hợp lệ (dd/mm/yyyy)!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        return;
    }
    
    if (!newPhone.empty() && !Validator::isValidPhone(newPhone)) {
        infoMessage.setFillColor(Color(200, 60, 60));
        infoMessage.setString(L"Số điện thoại không hợp lệ!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        return;
    }
    
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
    
    // Update info box outlines based on active field
    fullNameBox.setOutlineColor(activeInfoField == 0 ? Color(20, 118, 172) : Color(150, 150, 150));
    birthDateBox.setOutlineColor(activeInfoField == 1 ? Color(20, 118, 172) : Color(150, 150, 150));
    phoneBox.setOutlineColor(activeInfoField == 2 ? Color(20, 118, 172) : Color(150, 150, 150));
    
    fullNameBox.setOutlineThickness(activeInfoField == 0 ? 2.f : 1.f);
    birthDateBox.setOutlineThickness(activeInfoField == 1 ? 2.f : 1.f);
    phoneBox.setOutlineThickness(activeInfoField == 2 ? 2.f : 1.f);
    
    // Update password box outlines based on active field
    oldPasswordBox.setOutlineColor(activeField == 0 ? Color(20, 118, 172) : Color(150, 150, 150));
    newPasswordBox.setOutlineColor(activeField == 1 ? Color(20, 118, 172) : Color(150, 150, 150));
    confirmPasswordBox.setOutlineColor(activeField == 2 ? Color(20, 118, 172) : Color(150, 150, 150));
    
    oldPasswordBox.setOutlineThickness(activeField == 0 ? 2.f : 1.f);
    newPasswordBox.setOutlineThickness(activeField == 1 ? 2.f : 1.f);
    confirmPasswordBox.setOutlineThickness(activeField == 2 ? 2.f : 1.f);
    
    // Check button hovers
    isSaveInfoHovered = saveInfoBtn.getGlobalBounds().contains(mousePos);
    isChangePasswordHovered = changePasswordBtn.getGlobalBounds().contains(mousePos);
    
    // Update button colors
    saveInfoBtn.setFillColor(isSaveInfoHovered ? Color(45, 156, 219) : Color(20, 118, 172));
    changePasswordBtn.setFillColor(isChangePasswordHovered ? Color(45, 156, 219) : Color(20, 118, 172));
    
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
    
    // Draw semi-transparent overlay for better card visibility
    RectangleShape overlay({(float)window.getSize().x, (float)window.getSize().y});
    overlay.setFillColor(Color(0, 0, 0, 180)); // Dark overlay
    overlay.setPosition({0.f, 130.f}); // Start below header
    window.draw(overlay);
    
    // Draw white content cards
    float centerX = window.getSize().x / 2.f;
    
    // Personal info card (left)
    RectangleShape personalCard({680.f, 430.f});
    personalCard.setPosition({centerX - 720.f, 160.f});
    personalCard.setFillColor(Color(255, 255, 255, 250));
    window.draw(personalCard);
    
    // Password change card (right)
    RectangleShape passwordCard({620.f, 430.f});
    passwordCard.setPosition({centerX + 40.f, 160.f});
    passwordCard.setFillColor(Color(255, 255, 255, 250));
    window.draw(passwordCard);
    
    updatePositions(window.getSize());
    
    // Draw Personal Info Section
    window.draw(personalInfoTitle);
    
    window.draw(usernameLabel);
    window.draw(usernameBox);
    window.draw(usernameText);
    
    window.draw(emailLabel);
    window.draw(emailBox);
    window.draw(emailText);
    
    window.draw(fullNameLabel);
    window.draw(fullNameBox);
    window.draw(fullNameText);
    
    window.draw(birthDateLabel);
    window.draw(birthDateBox);
    window.draw(birthDateText);
    
    window.draw(phoneLabel);
    window.draw(phoneBox);
    window.draw(phoneText);
    
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