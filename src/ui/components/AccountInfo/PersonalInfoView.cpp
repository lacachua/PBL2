#include "UI/components/AccountInfo/PersonalInfoView.h"
#include "utils/PasswordHasher.h"
#include "utils/Validator.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

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

PersonalInfoView::PersonalInfoView(const Font& f, AuthService& auth)
    : font(f),
      authService(&auth),
      personalInfoTitle(f, L"THÔNG TIN CÁ NHÂN", 22),
      fullNameLabel(f, L"Họ và tên", 16),
      birthDateLabel(f, L"Ngày sinh", 16),
      phoneLabel(f, L"Số điện thoại", 16),
      emailLabel(f, L"Email", 16),
      fullNameText(f, "", 16),
      birthDateText(f, "", 16),
      phoneText(f, "", 16),
      emailText(f, "", 16),
      changePasswordTitle(f, L"ĐỔI MẬT KHẨU", 22),
      oldPasswordLabel(f, L"Mật khẩu cũ *", 16),
      newPasswordLabel(f, L"Mật khẩu mới *", 16),
      confirmPasswordLabel(f, L"Xác thực mật khẩu *", 16),
      oldPasswordText(f, "", 16),
      newPasswordText(f, "", 16),
      confirmPasswordText(f, "", 16),
      saveInfoBtn(f, L"LƯU THÔNG TIN", 260.f, 50.f, 18),
      changePasswordBtn(f, L"ĐỔI MẬT KHẨU", 260.f, 50.f, 18),
      infoMessage(f, "", 15),
      passwordMessage(f, "", 15)
{
    // Setup section titles
    personalInfoTitle.setFillColor(Color(238, 238, 238));
    personalInfoTitle.setStyle(Text::Bold);
    changePasswordTitle.setFillColor(Color(238, 238, 238));
    changePasswordTitle.setStyle(Text::Bold);
    
    // Setup labels
    fullNameLabel.setFillColor(Color(191, 215, 234));
    birthDateLabel.setFillColor(Color(191, 215, 234));
    phoneLabel.setFillColor(Color(191, 215, 234));
    emailLabel.setFillColor(Color(191, 215, 234));
    oldPasswordLabel.setFillColor(Color(191, 215, 234));
    newPasswordLabel.setFillColor(Color(191, 215, 234));
    confirmPasswordLabel.setFillColor(Color(191, 215, 234));
    
    // Setup input boxes
    fullNameBox.setSize({280.f, 42.f});
    birthDateBox.setSize({280.f, 42.f});
    phoneBox.setSize({280.f, 42.f});
    emailBox.setSize({280.f, 42.f});
    
    fullNameBox.setFillColor(Color::White);
    birthDateBox.setFillColor(Color::White);
    phoneBox.setFillColor(Color::White);
    emailBox.setFillColor(Color(233, 236, 239)); // Read-only
    
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineThickness(1.f);
    emailBox.setOutlineThickness(1.f);
    
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineColor(Color(201, 214, 226));
    emailBox.setOutlineColor(Color(200, 208, 217));
    
    // Setup input text
    fullNameText.setFillColor(Color(13, 27, 42));
    birthDateText.setFillColor(Color(13, 27, 42));
    phoneText.setFillColor(Color(13, 27, 42));
    emailText.setFillColor(Color(107, 114, 128)); // Read-only
    
    // Setup password boxes
    oldPasswordBox.setSize({604.f, 42.f});
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
    
    // Setup buttons
    saveInfoBtn.setFillColor(Color(0, 153, 255));
    saveInfoBtn.setTextColor(Color::White);
    changePasswordBtn.setFillColor(Color(0, 153, 255));
    changePasswordBtn.setTextColor(Color::White);
    
    // Setup cursor
    cursor.setSize({2.f, 20.f});
    cursor.setFillColor(Color(0, 153, 255));
    
    // Setup messages
    infoMessage.setFillColor(Color(60, 160, 90));
    passwordMessage.setFillColor(Color(60, 160, 90));
}

void PersonalInfoView::setUser(const string& email) {
    if (userDataLoaded && currentUserEmail == email) {
        return;
    }
    
    currentUserEmail = email;
    currentUser = authService->getUser(email);
    
    if (currentUser) {
        fullNameInput = utf8_to_wstring(currentUser->fullName);
        birthDateInput = utf8_to_wstring(currentUser->birthDate);
        phoneInput = utf8_to_wstring(currentUser->phone);
        
        fullNameText.setString(fullNameInput);
        birthDateText.setString(birthDateInput);
        phoneText.setString(phoneInput);
        emailText.setString(String::fromUtf8(currentUser->email.begin(), currentUser->email.end()));
        
        userDataLoaded = true;
    } else {
        fullNameText.setString("");
        birthDateText.setString("");
        phoneText.setString("");
        emailText.setString("");
        fullNameInput.clear();
        birthDateInput.clear();
        phoneInput.clear();
        userDataLoaded = false;
    }
}

void PersonalInfoView::handlePasswordInput(const Event* event) {
    if (!event) return;
    
    if (auto* textEvent = event->getIf<Event::TextEntered>()) {
        showPasswordMessage = false;
        
        char c = static_cast<char>(textEvent->unicode);
        
        if (c == '\b') {
            if (activeField == 0 && !oldPasswordInput.empty())
                oldPasswordInput.pop_back();
            else if (activeField == 1 && !newPasswordInput.empty())
                newPasswordInput.pop_back();
            else if (activeField == 2 && !confirmPasswordInput.empty())
                confirmPasswordInput.pop_back();
        }
        else if (c >= 32 && c < 127) {
            if (activeField == 0)
                oldPasswordInput += c;
            else if (activeField == 1)
                newPasswordInput += c;
            else if (activeField == 2)
                confirmPasswordInput += c;
        }
        
        if (activeField == 0)
            oldPasswordText.setString(string(oldPasswordInput.length(), '*'));
        else if (activeField == 1)
            newPasswordText.setString(string(newPasswordInput.length(), '*'));
        else if (activeField == 2)
            confirmPasswordText.setString(string(confirmPasswordInput.length(), '*'));
    }
}

void PersonalInfoView::handleInfoInput(const Event* event) {
    if (!event) return;
    
    if (auto* textEvent = event->getIf<Event::TextEntered>()) {
        showInfoMessage = false;
        
        wchar_t c = static_cast<wchar_t>(textEvent->unicode);
        
        if (c == '\b') {
            if (activeInfoField == 0 && !fullNameInput.empty())
                fullNameInput.pop_back();
            else if (activeInfoField == 1 && !birthDateInput.empty())
                birthDateInput.pop_back();
            else if (activeInfoField == 2 && !phoneInput.empty())
                phoneInput.pop_back();
        }
        else if (c >= 32 && c != 127) {
            if (activeInfoField == 0)
                fullNameInput += c;
            else if (activeInfoField == 1 && birthDateInput.length() < 10)
                birthDateInput += c;
            else if (activeInfoField == 2 && phoneInput.length() < 15)
                phoneInput += c;
        }
        
        if (activeInfoField == 0)
            fullNameText.setString(fullNameInput);
        else if (activeInfoField == 1)
            birthDateText.setString(birthDateInput);
        else if (activeInfoField == 2)
            phoneText.setString(phoneInput);
    }
}

void PersonalInfoView::savePasswordChange() {
    if (!currentUser) return;
    
    if (oldPasswordInput.empty() || newPasswordInput.empty() || confirmPasswordInput.empty()) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Vui lòng điền đầy đủ thông tin!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    if (!PasswordHasher::verifyPassword(oldPasswordInput, currentUser->passwordHash)) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Mật khẩu cũ không đúng!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    if (!Validator::isStrongPassword(newPasswordInput)) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Mật khẩu mới phải có ít nhất 8 ký tự, 1 chữ hoa, 1 chữ thường, 1 số!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    if (newPasswordInput != confirmPasswordInput) {
        passwordMessage.setFillColor(Color(200, 60, 60));
        passwordMessage.setString(L"Mật khẩu mới không khớp!");
        showPasswordMessage = true;
        messageTimerPassword.restart();
        return;
    }
    
    currentUser->passwordHash = PasswordHasher::hashPassword(newPasswordInput);
    authService->saveUsers();
    
    passwordMessage.setFillColor(Color(60, 160, 90));
    passwordMessage.setString(L"Đổi mật khẩu thành công!");
    showPasswordMessage = true;
    messageTimerPassword.restart();
    
    oldPasswordInput.clear();
    newPasswordInput.clear();
    confirmPasswordInput.clear();
    oldPasswordText.setString("");
    newPasswordText.setString("");
    confirmPasswordText.setString("");
    activeField = -1;
}

void PersonalInfoView::saveInfoChange() {
    if (!currentUser) return;
    
    string newFullName = wstring_to_utf8(fullNameInput);
    string newBirthDate = wstring_to_utf8(birthDateInput);
    string newPhone = wstring_to_utf8(phoneInput);
    
    if (newFullName.empty()) {
        infoMessage.setFillColor(Color(255, 68, 68));
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
    
    if (!newBirthDate.empty() && !Validator::isValidDate(newBirthDate)) {
        infoMessage.setFillColor(Color(255, 68, 68));
        infoMessage.setString(L"Ngày sinh không hợp lệ (dd/mm/yyyy)!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        birthDateBox.setOutlineColor(Color(255, 68, 68));
        birthDateBox.setOutlineThickness(2.f);
        return;
    }
    
    if (!newPhone.empty() && !Validator::isValidPhone(newPhone)) {
        infoMessage.setFillColor(Color(255, 68, 68));
        infoMessage.setString(L"Số điện thoại phải có 10 chữ số!");
        showInfoMessage = true;
        messageTimerInfo.restart();
        phoneBox.setOutlineColor(Color(255, 68, 68));
        phoneBox.setOutlineThickness(2.f);
        return;
    }
    
    fullNameBox.setOutlineColor(Color(201, 214, 226));
    fullNameBox.setOutlineThickness(1.f);
    birthDateBox.setOutlineColor(Color(201, 214, 226));
    birthDateBox.setOutlineThickness(1.f);
    phoneBox.setOutlineColor(Color(201, 214, 226));
    phoneBox.setOutlineThickness(1.f);
    
    currentUser->fullName = newFullName;
    currentUser->birthDate = newBirthDate;
    currentUser->phone = newPhone;
    
    authService->saveUsers();
    
    userDataLoaded = false;
    setUser(currentUserEmail);
    
    infoMessage.setFillColor(Color(60, 160, 90));
    infoMessage.setString(L"Cập nhật thông tin thành công!");
    showInfoMessage = true;
    messageTimerInfo.restart();
}

void PersonalInfoView::update(Vector2f mousePos, bool mousePressed, const Event* event, Vector2f cardPos) {
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
            
            if (code == Keyboard::Key::Tab) {
                if (activeInfoField >= 0) {
                    activeInfoField = (activeInfoField + 1) % 3;
                    activeField = -1;
                } else if (activeField >= 0) {
                    activeField = (activeField + 1) % 3;
                    activeInfoField = -1;
                }
                showCursor = true;
                cursorClock.restart();
            }
            else if (code == Keyboard::Key::Escape) {
                activeInfoField = -1;
                activeField = -1;
            }
        }
    }
    
    handlePasswordInput(event);
    handleInfoInput(event);
    
    // Update positions
    float cardPadding = 26.f;
    float section1Y = cardPos.y + cardPadding;
    float labelHeight = 20.f;
    float labelInputGap = 10.f;
    float rowGap = 18.f;
    
    float col1X = cardPos.x + cardPadding;
    float colGap = 24.f;
    float col2Shift = 12.f;
    float col2X = cardPos.x + cardPadding + 280.f + colGap + col2Shift;
    float inputStartY = section1Y + 45.f;
    
    // Position section titles
    personalInfoTitle.setPosition({col1X, section1Y});
    
    // Column 1
    fullNameLabel.setPosition({col1X, inputStartY});
    fullNameBox.setPosition({col1X, inputStartY + labelHeight + labelInputGap});
    fullNameText.setPosition({col1X + 10.f, inputStartY + labelHeight + labelInputGap + 11.f});
    
    float row2Y = inputStartY + labelHeight + labelInputGap + 42.f + rowGap;
    phoneLabel.setPosition({col1X, row2Y});
    phoneBox.setPosition({col1X, row2Y + labelHeight + labelInputGap});
    phoneText.setPosition({col1X + 10.f, row2Y + labelHeight + labelInputGap + 11.f});
    
    // Column 2
    birthDateLabel.setPosition({col2X, inputStartY});
    birthDateBox.setPosition({col2X, inputStartY + labelHeight + labelInputGap});
    birthDateText.setPosition({col2X + 10.f, inputStartY + labelHeight + labelInputGap + 11.f});
    
    emailLabel.setPosition({col2X, row2Y});
    emailBox.setPosition({col2X, row2Y + labelHeight + labelInputGap});
    emailText.setPosition({col2X + 10.f, row2Y + labelHeight + labelInputGap + 11.f});
    
    // Save button
    float saveInfoBtnY = row2Y + labelHeight + labelInputGap + 42.f + 24.f;
    saveInfoBtn.setPosition({col1X, saveInfoBtnY});
    infoMessage.setPosition({col1X, saveInfoBtnY + 60.f});
    
    // Section 2: Password
    float sectionGap = 24.f;
    float section2Y = saveInfoBtnY + 70.f + sectionGap;
    changePasswordTitle.setPosition({col1X, section2Y});
    
    float pwdInputStartY = section2Y + 45.f;
    
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
    
    float changePwdBtnY = pwdRow3Y + labelHeight + labelInputGap + 42.f + 24.f;
    changePasswordBtn.setPosition({col1X, changePwdBtnY});
    passwordMessage.setPosition({col1X, changePwdBtnY + 60.f});
    
    // Handle field clicks
    if (mousePressed) {
        if (fullNameBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 0;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
            showInfoMessage = false;
        }
        else if (birthDateBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 1;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
            showInfoMessage = false;
        }
        else if (phoneBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = 2;
            activeField = -1;
            showCursor = true;
            cursorClock.restart();
            showInfoMessage = false;
        }
        else if (oldPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 0;
            showCursor = true;
            cursorClock.restart();
            showPasswordMessage = false;
        }
        else if (newPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 1;
            showCursor = true;
            cursorClock.restart();
            showPasswordMessage = false;
        }
        else if (confirmPasswordBox.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = 2;
            showCursor = true;
            cursorClock.restart();
            showPasswordMessage = false;
        }
        else if (!saveInfoBtn.getGlobalBounds().contains(mousePos) && 
                 !changePasswordBtn.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activeField = -1;
        }
    }
    
    // Update box outlines
    fullNameBox.setOutlineColor(activeInfoField == 0 ? Color(0, 153, 255) : Color(201, 214, 226));
    birthDateBox.setOutlineColor(activeInfoField == 1 ? Color(0, 153, 255) : Color(201, 214, 226));
    phoneBox.setOutlineColor(activeInfoField == 2 ? Color(0, 153, 255) : Color(201, 214, 226));
    emailBox.setOutlineColor(Color(200, 208, 217));
    
    fullNameBox.setOutlineThickness(activeInfoField == 0 ? 2.f : 1.f);
    birthDateBox.setOutlineThickness(activeInfoField == 1 ? 2.f : 1.f);
    phoneBox.setOutlineThickness(activeInfoField == 2 ? 2.f : 1.f);
    emailBox.setOutlineThickness(1.f);
    
    oldPasswordBox.setOutlineColor(activeField == 0 ? Color(0, 153, 255) : Color(201, 214, 226));
    newPasswordBox.setOutlineColor(activeField == 1 ? Color(0, 153, 255) : Color(201, 214, 226));
    confirmPasswordBox.setOutlineColor(activeField == 2 ? Color(0, 153, 255) : Color(201, 214, 226));
    
    oldPasswordBox.setOutlineThickness(activeField == 0 ? 2.f : 1.f);
    newPasswordBox.setOutlineThickness(activeField == 1 ? 2.f : 1.f);
    confirmPasswordBox.setOutlineThickness(activeField == 2 ? 2.f : 1.f);
    
    // Update buttons
    Color btnHover(0, 191, 255);
    Color btnNormal(0, 153, 255);
    saveInfoBtn.update(mousePos, mousePressed, btnHover, btnNormal);
    changePasswordBtn.update(mousePos, mousePressed, btnHover, btnNormal);
    
    // Handle button clicks with debounce
    bool mouseJustPressed = mousePressed && !wasMousePressed;
    wasMousePressed = mousePressed;
    
    if (mouseJustPressed) {
        if (changePasswordBtn.isClicked(mousePos, true)) {
            savePasswordChange();
        }
        else if (saveInfoBtn.isClicked(mousePos, true)) {
            saveInfoChange();
        }
    }
}

void PersonalInfoView::draw(RenderWindow& window) {
    // Draw Section 1: Personal Info
    window.draw(personalInfoTitle);
    
    window.draw(fullNameLabel);
    window.draw(fullNameBox);
    window.draw(fullNameText);
    
    window.draw(phoneLabel);
    window.draw(phoneBox);
    window.draw(phoneText);
    
    window.draw(birthDateLabel);
    window.draw(birthDateBox);
    window.draw(birthDateText);
    
    window.draw(emailLabel);
    window.draw(emailBox);
    window.draw(emailText);
    
    saveInfoBtn.draw(window);
    
    // Draw Section 2: Password Change
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
    
    changePasswordBtn.draw(window);
    
    // Draw cursor
    if (showCursor) {
        if (activeInfoField == 0) {
            FloatRect bounds = fullNameText.getLocalBounds();
            cursor.setPosition({fullNameText.getPosition().x + bounds.size.x + 2.f, fullNameText.getPosition().y - 2.f});
            window.draw(cursor);
        }
        else if (activeInfoField == 1) {
            FloatRect bounds = birthDateText.getLocalBounds();
            cursor.setPosition({birthDateText.getPosition().x + bounds.size.x + 2.f, birthDateText.getPosition().y - 2.f});
            window.draw(cursor);
        }
        else if (activeInfoField == 2) {
            FloatRect bounds = phoneText.getLocalBounds();
            cursor.setPosition({phoneText.getPosition().x + bounds.size.x + 2.f, phoneText.getPosition().y - 2.f});
            window.draw(cursor);
        }
        else if (activeField == 0) {
            FloatRect bounds = oldPasswordText.getLocalBounds();
            cursor.setPosition({oldPasswordText.getPosition().x + bounds.size.x + 2.f, oldPasswordText.getPosition().y - 2.f});
            window.draw(cursor);
        }
        else if (activeField == 1) {
            FloatRect bounds = newPasswordText.getLocalBounds();
            cursor.setPosition({newPasswordText.getPosition().x + bounds.size.x + 2.f, newPasswordText.getPosition().y - 2.f});
            window.draw(cursor);
        }
        else if (activeField == 2) {
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
