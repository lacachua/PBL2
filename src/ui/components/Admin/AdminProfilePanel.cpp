#include "UI/components/Admin/AdminProfilePanel.h"
#include "utils/PasswordHasher.h"
#include "utils/Validator.h"
#include <algorithm>
#include <codecvt>
#include <cwctype>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace sf;

namespace {
static wstring utf8_to_wstring(const string& str) {
    if (str.empty()) return L"";
#ifdef _WIN32
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
    wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
    return wstr;
#else
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
#endif
}

static string wstring_to_utf8(const wstring& wstr) {
    if (wstr.empty()) return "";
#ifdef _WIN32
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &str[0], size_needed, nullptr, nullptr);
    return str;
#else
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
#endif
}
}

AdminProfilePanel::AdminProfilePanel(Font& f, float w, float h, AuthService* auth)
        : font(f),
            width(w),
            height(h),
            position(Vector2f(0.f, 0.f)),
            authService(auth),
            panelBackground(Vector2f(w, h)),
            infoCard(),
            passwordCard(),
            heroBanner(),
            avatarCircle(),
            personalInfoTitle(f, L"Thông tin cá nhân", 22),
            personalInfoHint(f, L"Thông tin này hiển thị cho toàn bộ hệ thống", 16),
            passwordTitle(f, L"Bảo mật", 22),
            passwordHint(f, L"Đảm bảo mật khẩu đủ mạnh và duy nhất", 16),
            heroNameText(f, L"", 22),
            heroMetaText(f, L"", 15),
            avatarInitialsText(f, L"", 20),
      fullNameLabel(f, L"Họ và tên", 17),
      birthDateLabel(f, L"Ngày sinh", 17),
      phoneLabel(f, L"Số điện thoại", 17),
      emailLabel(f, L"Email", 17),
      fullNameBox(Vector2f(300.f, 48.f)),
      birthDateBox(Vector2f(300.f, 48.f)),
      phoneBox(Vector2f(300.f, 48.f)),
      emailBox(Vector2f(300.f, 48.f)),
      fullNameText(f, L"", 17),
      birthDateText(f, L"", 17),
      phoneText(f, L"", 17),
      emailText(f, L"", 17),
      oldPasswordLabel(f, L"Mật khẩu hiện tại", 17),
      newPasswordLabel(f, L"Mật khẩu mới", 17),
      confirmPasswordLabel(f, L"Xác nhận mật khẩu", 17),
      oldPasswordBox(Vector2f(400.f, 48.f)),
      newPasswordBox(Vector2f(400.f, 48.f)),
    confirmPasswordBox(Vector2f(400.f, 48.f)),
    oldPasswordText(f, "", 17),
    newPasswordText(f, "", 17),
    confirmPasswordText(f, "", 17),
      saveInfoBtn(f, L"LƯU HỒ SƠ", 220.f, 48.f, 18),
      changePasswordBtn(f, L"CẬP NHẬT MẬT KHẨU", 260.f, 48.f, 18),
      infoMessage(f, L"", 16),
    passwordMessage(f, L"", 16)
{
    Color panelColor(241, 245, 250);
    Color cardColor(255, 255, 255);
    Color outlineColor(214, 222, 230);
    Color mutedText(108, 122, 140);
    Color headingColor(19, 33, 58);
    Color inputBg(248, 250, 252);
    Color heroBg(227, 233, 243);
    Color chipColor(20, 118, 172);
    Color dividerColor(211, 220, 232);

    panelBackground.setFillColor(panelColor);

    infoCard.setFillColor(cardColor);
    infoCard.setOutlineColor(outlineColor);
    infoCard.setOutlineThickness(1.2f);

    passwordCard.setFillColor(cardColor);
    passwordCard.setOutlineColor(outlineColor);
    passwordCard.setOutlineThickness(1.2f);

    personalInfoTitle.setFillColor(headingColor);
    personalInfoTitle.setStyle(Text::Bold);
    personalInfoHint.setFillColor(mutedText);
    passwordTitle.setFillColor(headingColor);
    passwordTitle.setStyle(Text::Bold);
    passwordHint.setFillColor(mutedText);
    heroNameText.setFillColor(headingColor);
    heroNameText.setStyle(Text::Bold);
    heroMetaText.setFillColor(mutedText);
    avatarInitialsText.setFillColor(chipColor);
    avatarInitialsText.setStyle(Text::Bold);

    fullNameLabel.setFillColor(mutedText);
    birthDateLabel.setFillColor(mutedText);
    phoneLabel.setFillColor(mutedText);
    emailLabel.setFillColor(mutedText);
    oldPasswordLabel.setFillColor(mutedText);
    newPasswordLabel.setFillColor(mutedText);
    confirmPasswordLabel.setFillColor(mutedText);

    auto setupBox = [&](RectangleShape& box, bool readOnly = false) {
        box.setSize(Vector2f(320.f, 48.f));
        box.setFillColor(readOnly ? Color(238, 241, 245) : inputBg);
        box.setOutlineColor(outlineColor);
        box.setOutlineThickness(1.f);
    };

    setupBox(fullNameBox);
    setupBox(birthDateBox);
    setupBox(phoneBox);
    setupBox(emailBox, true);
    setupBox(oldPasswordBox);
    setupBox(newPasswordBox);
    setupBox(confirmPasswordBox);

    fullNameText.setFillColor(headingColor);
    birthDateText.setFillColor(headingColor);
    phoneText.setFillColor(headingColor);
    emailText.setFillColor(Color(120, 130, 145));

    oldPasswordText.setFillColor(headingColor);
    newPasswordText.setFillColor(headingColor);
    confirmPasswordText.setFillColor(headingColor);

    saveInfoBtn.setTextColor(Color::White);
    changePasswordBtn.setTextColor(Color::White);

    cursor.setSize(Vector2f(2.f, 26.f));
    cursor.setFillColor(Color(20, 118, 172));

    infoMessage.setFillColor(Color(63, 151, 90));
    passwordMessage.setFillColor(Color(63, 151, 90));

    heroBanner.setFillColor(heroBg);
    heroBanner.setOutlineColor(outlineColor);
    heroBanner.setOutlineThickness(1.f);

    avatarCircle.setRadius(26.f);
    avatarCircle.setFillColor(chipColor);
    avatarCircle.setOutlineColor(chipColor);
    avatarCircle.setOutlineThickness(0.f);

    infoBoxes = { &fullNameBox, &birthDateBox, &phoneBox };
    infoTexts = { &fullNameText, &birthDateText, &phoneText };
    passwordBoxes = { &oldPasswordBox, &newPasswordBox, &confirmPasswordBox };

    refreshUser();
    layoutElements();
}

void AdminProfilePanel::setPosition(Vector2f pos) {
    position = pos;
    layoutElements();
}

void AdminProfilePanel::layoutElements() {
    panelBackground.setSize(Vector2f(width, height));
    panelBackground.setPosition(position);

    float outerPadding = 28.f;
    float heroPadding = 20.f;
    float cardSpacing = 24.f;
    float heroHeight = 72.f;  // Compact header
    float contentWidth = width - outerPadding * 2.f;

    Vector2f heroPos(position.x + outerPadding, position.y + outerPadding);
    heroBanner.setPosition(heroPos);
    heroBanner.setSize(Vector2f(contentWidth, heroHeight));

    // Avatar centered vertically in hero banner
    float avatarRadius = avatarCircle.getRadius();
    float avatarY = heroPos.y + (heroHeight - avatarRadius * 2.f) / 2.f;
    avatarCircle.setPosition(Vector2f(heroPos.x + heroPadding, avatarY));
    
    // Center initials in avatar
    FloatRect initialsBounds = avatarInitialsText.getLocalBounds();
    float avatarCenterX = avatarCircle.getPosition().x + avatarRadius;
    float avatarCenterY = avatarCircle.getPosition().y + avatarRadius;
    avatarInitialsText.setPosition(Vector2f(
        avatarCenterX - (initialsBounds.size.x / 2.f) - initialsBounds.position.x,
        avatarCenterY - (initialsBounds.size.y / 2.f) - initialsBounds.position.y
    ));

    // Text next to avatar
    float textStartX = avatarCircle.getPosition().x + avatarRadius * 2.f + 16.f;
    float textCenterY = heroPos.y + heroHeight / 2.f;
    
    // Two lines: name on top, meta below
    heroNameText.setPosition(Vector2f(textStartX, textCenterY - 20.f));
    heroMetaText.setPosition(Vector2f(textStartX, textCenterY + 6.f));

    float cardsStartY = heroPos.y + heroHeight + 16.f;
    Vector2f contentPos(heroPos.x, cardsStartY);
    float availableHeight = height - cardsStartY - outerPadding;
    float cardHeight = std::max(320.f, (availableHeight - cardSpacing) * 0.55f);
    float passwordHeight = std::max(260.f, availableHeight - cardHeight - cardSpacing);
    if (passwordHeight < 260.f) {
        passwordHeight = 260.f;
        cardHeight = std::max(280.f, availableHeight - passwordHeight - cardSpacing);
    }
    if (cardHeight <= 0.f || passwordHeight <= 0.f) {
        cardHeight = std::max(260.f, (availableHeight - cardSpacing) * 0.5f);
        passwordHeight = std::max(220.f, availableHeight - cardHeight - cardSpacing);
    }

    infoCard.setPosition(contentPos);
    infoCard.setSize(Vector2f(contentWidth, cardHeight));

    // Layout constants - easily adjustable
    struct LayoutConfig {
        float innerPadding = 28.f;
        float columnSpacing = 36.f;
        float labelToBoxGap = 8.f;      // Gap between label and input box
        float rowSpacing = 24.f;         // Gap between rows
        float fieldHeight = 48.f;
        float labelHeight = 22.f;        // Approximate label text height
        float buttonSpacing = 20.f;
        float titleToHintGap = 28.f;
        float hintToFieldsGap = 32.f;
    };
    LayoutConfig cfg;

    float infoColumnWidth = (infoCard.getSize().x - cfg.innerPadding * 2.f - cfg.columnSpacing) / 2.f;
    Vector2f infoOrigin(infoCard.getPosition().x + cfg.innerPadding, 
                        infoCard.getPosition().y + cfg.innerPadding);

    // Info card header
    personalInfoTitle.setPosition(infoOrigin);
    personalInfoHint.setPosition(Vector2f(infoOrigin.x, infoOrigin.y + cfg.titleToHintGap));

    // Calculate field row start position
    float firstRowLabelY = personalInfoHint.getPosition().y + cfg.hintToFieldsGap;
    float col1X = infoOrigin.x;
    float col2X = infoOrigin.x + infoColumnWidth + cfg.columnSpacing;

    // Helper lambda to place a field (label above box)
    auto placeField = [&](Text& label, RectangleShape& box, Text& text, 
                          float x, float labelY, float colWidth) {
        // Position label
        label.setPosition(Vector2f(x, labelY));
        
        // Position box below label with gap
        float boxY = labelY + cfg.labelHeight + cfg.labelToBoxGap;
        box.setPosition(Vector2f(x, boxY));
        box.setSize(Vector2f(colWidth, cfg.fieldHeight));
        
        // Center text vertically inside box
        FloatRect textBounds = text.getLocalBounds();
        float textY = boxY + (cfg.fieldHeight - textBounds.size.y) / 2.f - textBounds.position.y;
        text.setPosition(Vector2f(x + 12.f, textY));
    };

    // Row 1: Full name + Birth date
    placeField(fullNameLabel, fullNameBox, fullNameText, col1X, firstRowLabelY, infoColumnWidth);
    placeField(birthDateLabel, birthDateBox, birthDateText, col2X, firstRowLabelY, infoColumnWidth);

    // Row 2: Phone + Email (calculate Y based on row 1 box bottom)
    float row2LabelY = fullNameBox.getPosition().y + cfg.fieldHeight + cfg.rowSpacing;
    placeField(phoneLabel, phoneBox, phoneText, col1X, row2LabelY, infoColumnWidth);
    placeField(emailLabel, emailBox, emailText, col2X, row2LabelY, infoColumnWidth);

    // Save button position
    float saveBtnY = phoneBox.getPosition().y + cfg.fieldHeight + cfg.buttonSpacing;
    saveInfoBtn.setPosition(Vector2f(col1X, saveBtnY));
    infoMessage.setPosition(Vector2f(col1X, saveBtnY + saveInfoBtn.getSize().y + 10.f));

    // Adjust info card height if needed
    float infoNeededHeight = (saveBtnY + saveInfoBtn.getSize().y + cfg.innerPadding) - infoCard.getPosition().y;
    if (infoNeededHeight > infoCard.getSize().y) {
        infoCard.setSize(Vector2f(contentWidth, infoNeededHeight));
    }

    // Password card positioning
    Vector2f passwordPos(contentPos.x, infoCard.getPosition().y + infoCard.getSize().y + cardSpacing);
    passwordCard.setPosition(passwordPos);
    passwordCard.setSize(Vector2f(contentWidth, passwordHeight));

    Vector2f pwdOrigin(passwordCard.getPosition().x + cfg.innerPadding, 
                       passwordCard.getPosition().y + cfg.innerPadding);

    // Password card header
    passwordTitle.setPosition(pwdOrigin);
    passwordHint.setPosition(Vector2f(pwdOrigin.x, pwdOrigin.y + cfg.titleToHintGap));

    // Password fields - 3 columns
    float pwdColSpacing = 28.f;
    float pwdColumnWidth = (passwordCard.getSize().x - cfg.innerPadding * 2.f - pwdColSpacing * 2.f) / 3.f;
    float pwdRowLabelY = passwordHint.getPosition().y + cfg.hintToFieldsGap;
    
    float pwdCol1X = pwdOrigin.x;
    float pwdCol2X = pwdCol1X + pwdColumnWidth + pwdColSpacing;
    float pwdCol3X = pwdCol2X + pwdColumnWidth + pwdColSpacing;

    placeField(oldPasswordLabel, oldPasswordBox, oldPasswordText, pwdCol1X, pwdRowLabelY, pwdColumnWidth);
    placeField(newPasswordLabel, newPasswordBox, newPasswordText, pwdCol2X, pwdRowLabelY, pwdColumnWidth);
    placeField(confirmPasswordLabel, confirmPasswordBox, confirmPasswordText, pwdCol3X, pwdRowLabelY, pwdColumnWidth);

    // Change password button
    float pwdBtnY = oldPasswordBox.getPosition().y + cfg.fieldHeight + cfg.buttonSpacing;
    changePasswordBtn.setPosition(Vector2f(pwdOrigin.x, pwdBtnY));
    passwordMessage.setPosition(Vector2f(pwdOrigin.x, pwdBtnY + changePasswordBtn.getSize().y + 10.f));

    // Adjust password card height if needed
    float pwdNeededHeight = (pwdBtnY + changePasswordBtn.getSize().y + cfg.innerPadding) - passwordCard.getPosition().y;
    if (pwdNeededHeight > passwordCard.getSize().y) {
        passwordCard.setSize(Vector2f(contentWidth, pwdNeededHeight));
    }
}

void AdminProfilePanel::refreshUser() {
    if (!authService) return;
    User* user = authService->getCurrentUser();
    if (!user) return;

    if (userDataLoaded && currentEmail == user->getEmail()) {
        return;
    }

    currentUser = user;
    currentEmail = user->getEmail();

    fullNameInput = utf8_to_wstring(user->getFullName());
    birthDateInput = utf8_to_wstring(user->getBirthDate());
    phoneInput = utf8_to_wstring(user->getPhone());

    fullNameText.setString(fullNameInput);
    birthDateText.setString(birthDateInput);
    phoneText.setString(phoneInput);
    emailText.setString(utf8_to_wstring(user->getEmail()));

    updateHeroContents();

    userDataLoaded = true;
}

void AdminProfilePanel::handleEvent(const Event& event, const RenderWindow&) {
    if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
        if (activeInfoField >= 0) {
            handleInfoInput(textEvent->unicode);
        } else if (activePasswordField >= 0) {
            handlePasswordInput(textEvent->unicode);
        }
    } else if (const auto* keyEvent = event.getIf<Event::KeyPressed>()) {
        if (keyEvent->code == Keyboard::Key::Tab) {
            if (activeInfoField >= 0) {
                activeInfoField = (activeInfoField + 1) % 3;
            } else if (activePasswordField >= 0) {
                activePasswordField = (activePasswordField + 1) % 3;
            } else {
                activeInfoField = 0;
            }
            activePasswordField = (activeInfoField >= 0) ? -1 : activePasswordField;
            showCursor = true;
            cursorClock.restart();
        } else if (keyEvent->code == Keyboard::Key::Escape) {
            activeInfoField = -1;
            activePasswordField = -1;
        } else if (keyEvent->code == Keyboard::Key::Enter) {
            if (activePasswordField >= 0) {
                savePasswordChange();
            } else if (activeInfoField >= 0) {
                saveInfoChange();
            }
        }
    }
}

void AdminProfilePanel::handleInfoInput(std::uint32_t unicode) {
    if (unicode == 9 || unicode == 13) return;
    wchar_t c = static_cast<wchar_t>(unicode);

    if (c == 8) {
        if (activeInfoField == 0 && !fullNameInput.empty()) fullNameInput.pop_back();
        else if (activeInfoField == 1 && !birthDateInput.empty()) birthDateInput.pop_back();
        else if (activeInfoField == 2 && !phoneInput.empty()) phoneInput.pop_back();
    } else if (c >= 32 && c != 127) {
        if (activeInfoField == 0) {
            fullNameInput += c;
        } else if (activeInfoField == 1 && birthDateInput.length() < 10) {
            birthDateInput += c;
        } else if (activeInfoField == 2 && phoneInput.length() < 15) {
            phoneInput += c;
        }
    }

    if (activeInfoField == 0) fullNameText.setString(fullNameInput);
    else if (activeInfoField == 1) birthDateText.setString(birthDateInput);
    else if (activeInfoField == 2) phoneText.setString(phoneInput);

    showCursor = true;
    cursorClock.restart();
    showInfoMessage = false;
}

void AdminProfilePanel::handlePasswordInput(std::uint32_t unicode) {
    if (unicode == 9 || unicode == 13) return;
    char c = static_cast<char>(unicode);

    if (c == '\b') {
        if (activePasswordField == 0 && !oldPasswordInput.empty()) oldPasswordInput.pop_back();
        else if (activePasswordField == 1 && !newPasswordInput.empty()) newPasswordInput.pop_back();
        else if (activePasswordField == 2 && !confirmPasswordInput.empty()) confirmPasswordInput.pop_back();
    } else if (c >= 32 && c < 127) {
        if (activePasswordField == 0) oldPasswordInput += c;
        else if (activePasswordField == 1) newPasswordInput += c;
        else if (activePasswordField == 2) confirmPasswordInput += c;
    }

    auto mask = [](size_t len) { return string(len, '*'); };
    oldPasswordText.setString(mask(oldPasswordInput.length()));
    newPasswordText.setString(mask(newPasswordInput.length()));
    confirmPasswordText.setString(mask(confirmPasswordInput.length()));

    showCursor = true;
    cursorClock.restart();
    showPasswordMessage = false;
}

void AdminProfilePanel::update(Vector2f mousePos, bool mousePressed) {
    refreshUser();
    layoutElements();

    if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    if (showInfoMessage && infoMessageClock.getElapsedTime().asSeconds() > 3.f) {
        showInfoMessage = false;
    }
    if (showPasswordMessage && passwordMessageClock.getElapsedTime().asSeconds() > 3.f) {
        showPasswordMessage = false;
    }

    Color accent(20, 118, 172);
    Color neutral(214, 222, 230);
    Color errorColor(209, 67, 67);

    auto updateOutline = [&](RectangleShape& box, bool active) {
        box.setOutlineColor(active ? accent : neutral);
        box.setOutlineThickness(active ? 2.f : 1.f);
    };

    for (size_t i = 0; i < infoBoxes.size(); ++i) {
        updateOutline(*infoBoxes[i], static_cast<int>(i) == activeInfoField);
    }
    emailBox.setOutlineColor(neutral);
    emailBox.setOutlineThickness(1.f);

    for (size_t i = 0; i < passwordBoxes.size(); ++i) {
        updateOutline(*passwordBoxes[i], static_cast<int>(i) == activePasswordField);
    }

    Color btnNormal(20, 118, 172);
    Color btnHover(35, 150, 210);
    saveInfoBtn.update(mousePos, mousePressed, btnHover, btnNormal);
    changePasswordBtn.update(mousePos, mousePressed, btnHover, btnNormal);

    bool mouseJustPressed = mousePressed && !wasMousePressed;
    wasMousePressed = mousePressed;

    if (mouseJustPressed) {
        auto focusInfo = [&](int fieldIndex) {
            activeInfoField = fieldIndex;
            activePasswordField = -1;
            showCursor = true;
            cursorClock.restart();
        };
        auto focusPassword = [&](int fieldIndex) {
            activePasswordField = fieldIndex;
            activeInfoField = -1;
            showCursor = true;
            cursorClock.restart();
        };

        if (fullNameBox.getGlobalBounds().contains(mousePos)) focusInfo(0);
        else if (birthDateBox.getGlobalBounds().contains(mousePos)) focusInfo(1);
        else if (phoneBox.getGlobalBounds().contains(mousePos)) focusInfo(2);
        else if (oldPasswordBox.getGlobalBounds().contains(mousePos)) focusPassword(0);
        else if (newPasswordBox.getGlobalBounds().contains(mousePos)) focusPassword(1);
        else if (confirmPasswordBox.getGlobalBounds().contains(mousePos)) focusPassword(2);
        else if (!saveInfoBtn.getGlobalBounds().contains(mousePos) && !changePasswordBtn.getGlobalBounds().contains(mousePos)) {
            activeInfoField = -1;
            activePasswordField = -1;
        }

        if (saveInfoBtn.isClicked(mousePos, mouseJustPressed)) {
            saveInfoChange();
        } else if (changePasswordBtn.isClicked(mousePos, mouseJustPressed)) {
            savePasswordChange();
        }
    }
}

void AdminProfilePanel::saveInfoChange() {
    if (!currentUser) return;

    string newFullName = wstring_to_utf8(fullNameInput);
    string newBirthDate = wstring_to_utf8(birthDateInput);
    string newPhone = wstring_to_utf8(phoneInput);

    Color errorColor(209, 67, 67);
    Color successColor(63, 151, 90);

    auto showError = [&](const wstring& msg, RectangleShape* highlightBox = nullptr) {
        infoMessage.setFillColor(errorColor);
        infoMessage.setString(msg);
        showInfoMessage = true;
        infoMessageClock.restart();
        if (highlightBox) {
            highlightBox->setOutlineColor(errorColor);
            highlightBox->setOutlineThickness(2.f);
        }
    };

    if (newFullName.empty()) {
        showError(L"Họ tên không được để trống", &fullNameBox);
        return;
    }

    if (!Validator::isValidFullName(newFullName)) {
        showError(L"Họ tên không được chứa ký tự số", &fullNameBox);
        return;
    }

    if (!newBirthDate.empty() && !Validator::isValidDate(newBirthDate)) {
        showError(L"Ngày sinh không hợp lệ (dd/mm/yyyy)", &birthDateBox);
        return;
    }

    if (!newPhone.empty() && !Validator::isValidPhone(newPhone)) {
        showError(L"Số điện thoại phải gồm 10 chữ số", &phoneBox);
        return;
    }

    currentUser->setFullName(newFullName);
    currentUser->setBirthDate(newBirthDate);
    currentUser->setPhone(newPhone);
    persistUser();

    userDataLoaded = false;
    refreshUser();

    infoMessage.setFillColor(successColor);
    infoMessage.setString(L"Đã lưu thông tin hồ sơ");
    showInfoMessage = true;
    infoMessageClock.restart();
}

void AdminProfilePanel::savePasswordChange() {
    if (!currentUser) return;

    Color errorColor(209, 67, 67);
    Color successColor(63, 151, 90);

    auto showError = [&](const wstring& msg) {
        passwordMessage.setFillColor(errorColor);
        passwordMessage.setString(msg);
        showPasswordMessage = true;
        passwordMessageClock.restart();
    };

    if (oldPasswordInput.empty() || newPasswordInput.empty() || confirmPasswordInput.empty()) {
        showError(L"Vui lòng nhập đầy đủ các trường bắt buộc");
        return;
    }

    if (!PasswordHasher::verifyPassword(oldPasswordInput, currentUser->getPasswordHash())) {
        showError(L"Mật khẩu hiện tại không chính xác");
        return;
    }

    if (!Validator::isStrongPassword(newPasswordInput)) {
        showError(L"Mật khẩu mới phải >=8 ký tự, gồm chữ hoa, chữ thường và số");
        return;
    }

    if (newPasswordInput != confirmPasswordInput) {
        showError(L"Mật khẩu xác nhận không khớp");
        return;
    }

    currentUser->setPasswordHash(PasswordHasher::hashPassword(newPasswordInput));
    persistUser();

    passwordMessage.setFillColor(successColor);
    passwordMessage.setString(L"Đã cập nhật mật khẩu");
    showPasswordMessage = true;
    passwordMessageClock.restart();

    resetPasswordInputs();
}

void AdminProfilePanel::resetPasswordInputs() {
    oldPasswordInput.clear();
    newPasswordInput.clear();
    confirmPasswordInput.clear();
    oldPasswordText.setString("");
    newPasswordText.setString("");
    confirmPasswordText.setString("");
    activePasswordField = -1;
}

void AdminProfilePanel::persistUser() {
    if (!authService || !currentUser) return;
    if (auto* repo = authService->getRepository()) {
        repo->updateUser(*currentUser);
    }
}

void AdminProfilePanel::render(RenderWindow& window) {
    window.draw(panelBackground);
    window.draw(heroBanner);
    window.draw(avatarCircle);
    window.draw(avatarInitialsText);
    window.draw(heroNameText);
    window.draw(heroMetaText);
    window.draw(infoCard);
    window.draw(passwordCard);

    window.draw(personalInfoTitle);
    window.draw(personalInfoHint);

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

    window.draw(passwordTitle);
    window.draw(passwordHint);

    window.draw(oldPasswordLabel);
    window.draw(oldPasswordBox);
    window.draw(oldPasswordText);

    window.draw(newPasswordLabel);
    window.draw(newPasswordBox);
    window.draw(newPasswordText);

    window.draw(confirmPasswordLabel);
    window.draw(confirmPasswordBox);
    window.draw(confirmPasswordText);

    if (showCursor) {
        bool cursorVisible = false;
        FloatRect bounds;
        float cursorHeight = 24.f;
        cursor.setSize(Vector2f(2.f, cursorHeight));
        
        // Helper to position cursor inside a box
        auto positionCursor = [&](const Text& text, const RectangleShape& box) {
            bounds = text.getLocalBounds();
            float textEndX = text.getPosition().x + bounds.size.x;
            float boxCenterY = box.getPosition().y + (box.getSize().y - cursorHeight) / 2.f;
            cursor.setPosition(Vector2f(textEndX + 2.f, boxCenterY));
        };

        if (activeInfoField == 0) {
            positionCursor(fullNameText, fullNameBox);
            cursorVisible = true;
        } else if (activeInfoField == 1) {
            positionCursor(birthDateText, birthDateBox);
            cursorVisible = true;
        } else if (activeInfoField == 2) {
            positionCursor(phoneText, phoneBox);
            cursorVisible = true;
        } else if (activePasswordField == 0) {
            positionCursor(oldPasswordText, oldPasswordBox);
            cursorVisible = true;
        } else if (activePasswordField == 1) {
            positionCursor(newPasswordText, newPasswordBox);
            cursorVisible = true;
        } else if (activePasswordField == 2) {
            positionCursor(confirmPasswordText, confirmPasswordBox);
            cursorVisible = true;
        }
        if (cursorVisible) {
            window.draw(cursor);
        }
    }

    saveInfoBtn.draw(window);
    changePasswordBtn.draw(window);

    if (showInfoMessage) window.draw(infoMessage);
    if (showPasswordMessage) window.draw(passwordMessage);
}

void AdminProfilePanel::reload() {
    userDataLoaded = false;
    refreshUser();
}

void AdminProfilePanel::updateHeroContents() {
    if (!currentUser) return;

    // Display name: use full name if available, otherwise email prefix
    std::wstring displayName = fullNameInput.empty() 
        ? utf8_to_wstring(currentUser->getEmail().substr(0, currentUser->getEmail().find('@')))
        : fullNameInput;
    // Uppercase first letter
    if (!displayName.empty()) {
        displayName[0] = std::towupper(displayName[0]);
    }
    heroNameText.setString(L"HỒ SƠ CỦA " + displayName);

    // Meta line: email - role
    std::wstring roleDescription;
    switch (currentUser->getRole()) {
        case AppRole::Admin: roleDescription = L"Quản trị viên"; break;
        case AppRole::Staff: roleDescription = L"Nhân viên"; break;
        case AppRole::Customer: roleDescription = L"Khách hàng"; break;
        default: roleDescription = L"Khách"; break;
    }
    std::wstring emailW = utf8_to_wstring(currentUser->getEmail());
    heroMetaText.setString(emailW + L"  -  " + roleDescription);

    // Avatar initials (single letter for compact design)
    std::wstring initials = buildInitials(currentUser->getFullName());
    if (initials.empty()) {
        initials = buildInitials(currentUser->getEmail());
    }
    if (initials.length() > 1) initials = initials.substr(0, 1);  // Single letter
    avatarInitialsText.setString(initials);
    avatarInitialsText.setFillColor(sf::Color::White);  // White on blue background
}

std::wstring AdminProfilePanel::getRoleLabel(AppRole role) const {
    switch (role) {
        case AppRole::Admin: return L"ADMIN";
        case AppRole::Staff: return L"STAFF";
        case AppRole::Customer: return L"CUSTOMER";
        default: return L"GUEST";
    }
}

std::wstring AdminProfilePanel::buildInitials(const std::string& source) const {
    std::wstring wide = utf8_to_wstring(source);
    std::wstring result;
    bool takeNext = true;
    for (wchar_t ch : wide) {
        if (std::iswalpha(ch) || std::iswdigit(ch)) {
            if (takeNext) {
                result.push_back(std::towupper(ch));
                takeNext = false;
                if (result.size() == 2) break;
            }
        } else if (ch == L' ' || ch == L'-') {
            takeNext = true;
        }
    }

    if (result.empty() && !wide.empty()) {
        result.push_back(std::towupper(wide.front()));
    }

    return result;
}
