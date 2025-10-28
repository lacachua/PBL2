#include "RegisterScreen.h"

RegisterScreen::RegisterScreen(const Font& font, AuthService& authRef)
    : auth(authRef),
      overlay({0.f, 0.f}),
      card({500.f, 650.f}),
      title(font, L"Đăng ký tài khoản", 30),
      registerBtn({252.f, 44.f}),
      backBtn({141.f, 44.f}),
      registerBtnText(font, L"Đăng ký", 20),
      backBtnText(font, L"Quay lại", 20),
      closeX(font, L"X", 24),
      msg(font, L"", 16),
      cursor({2.f, 20.f})
{
    overlay.setFillColor(Color(0, 0, 0, 170));
    card.setFillColor(Color(255, 255, 255, 240));
    card.setOrigin(card.getSize() / 2.f);
    title.setFillColor(Color::Black);

    inputBoxes.reserve(6);
    labels.reserve(6);
    placeholders.reserve(6);
    displays.reserve(6);

    wstring labelTexts[] = {
        L"Họ và tên*",
        L"Ngày sinh (dd/mm/yyyy)*",
        L"Email*",
        L"Số điện thoại*",
        L"Mật khẩu*",
        L"Xác nhận mật khẩu*"
    };

    wstring placeholderTexts[] = {
        L"Nhập họ và tên",
        L"dd/mm/yyyy",
        L"example@email.com",
        L"0123456789",
        L"Nhập mật khẩu",
        L"Nhập lại mật khẩu"
    };

    for (int i = 0; i < 6; i++) {
        inputBoxes.emplace_back(Vector2f{400.f, 40.f});
        inputBoxes.back().setFillColor(Color(238, 238, 238));
        inputBoxes.back().setOutlineThickness(2.f);
        inputBoxes.back().setOutlineColor(Color(200, 200, 200));

        labels.emplace_back(font, labelTexts[i], 16);
        labels.back().setFillColor(Color(60, 60, 60));

        placeholders.emplace_back(font, placeholderTexts[i], 18);
        placeholders.back().setFillColor(Color(120, 120, 120));

        displays.emplace_back(font, L"", 18);
        displays.back().setFillColor(Color::Black);
    }

    registerBtn.setFillColor(Color(100, 149, 237));
    registerBtnText.setFillColor(Color::White);

    backBtn.setFillColor(Color(90, 90, 90));
    backBtnText.setFillColor(Color::White);

    closeX.setFillColor(Color(90, 90, 90));

    FloatRect regBounds = registerBtnText.getLocalBounds();
    registerBtnText.setOrigin({regBounds.size.x / 2.f, regBounds.size.y / 2.f});

    FloatRect backBounds = backBtnText.getLocalBounds();
    backBtnText.setOrigin({backBounds.size.x / 2.f, backBounds.size.y / 2.f});

    cursor.setFillColor(Color::Black);
    msg.setFillColor(Color(200, 60, 60));
}

wstring RegisterScreen::bullets(size_t n) {
    return wstring(n, L'•');
}

bool RegisterScreen::validateInputs() {
    // Field 0: Full name
    // Field 1: Birthdate
    // Field 2: Email
    // Field 3: Phone
    // Field 4: Password
    // Field 5: Confirm password
    
    // Check empty fields
    for (int i = 0; i < 6; i++) {
        if (inputs[i].empty()) {
            msg.setFillColor(Color(200, 60, 60));
            msg.setString(L"Vui lòng điền đầy đủ thông tin.");
            return false;
        }
    }

    // Validate birthdate (Field 1)
    string birthDate(inputs[1].begin(), inputs[1].end());
    if (!Validator::isValidDate(birthDate)) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Ngày sinh không hợp lệ (dd/mm/yyyy).");
        return false;
    }

    // Validate email (Field 2)
    string email(inputs[2].begin(), inputs[2].end());
    if (!Validator::isValidEmail(email)) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Email không hợp lệ.");
        return false;
    }

    // Check if email exists
    if (auth.emailExists(email)) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Email đã được sử dụng.");
        return false;
    }

    // Validate phone (Field 3)
    string phone(inputs[3].begin(), inputs[3].end());
    if (!Validator::isValidPhone(phone)) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Số điện thoại không hợp lệ (10 số, bắt đầu bằng 0).");
        return false;
    }

    // Validate password strength (Field 4)
    string password(inputs[4].begin(), inputs[4].end());
    if (!Validator::isStrongPassword(password)) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Mật khẩu phải có ít nhất 8 ký tự, chữ hoa, chữ thường và số.");
        return false;
    }

    // Check password confirmation (Field 5)
    if (inputs[4] != inputs[5]) {
        msg.setFillColor(Color(200, 60, 60));
        msg.setString(L"Mật khẩu xác nhận không khớp.");
        return false;
    }

    return true;
}

bool RegisterScreen::update(Vector2f mouse, bool mousePressed, const Event& event) {
    if (cursorClock.getElapsedTime().asMilliseconds() > 530) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    if (showSuccessMessage && messageTimer.getElapsedTime().asSeconds() > 1.0f) {
        showSuccessMessage = false;
        reset();
        return true;
    }

    if (mousePressed && !card.getGlobalBounds().contains(mouse)) return true;
    if (mousePressed && closeX.getGlobalBounds().contains(mouse)) return true;

    if (mousePressed) {
        activeField = -1;
        for (int i = 0; i < 6; i++) {
            if (inputBoxes[i].getGlobalBounds().contains(mouse)) {
                activeField = i;
                showCursor = true;
                cursorClock.restart();
                break;
            }
        }

        if (registerBtn.getGlobalBounds().contains(mouse)) {
            if (validateInputs()) {
                string fullName(inputs[0].begin(), inputs[0].end());
                string birthDate(inputs[1].begin(), inputs[1].end());
                string email(inputs[2].begin(), inputs[2].end());
                string phone(inputs[3].begin(), inputs[3].end());
                string password(inputs[4].begin(), inputs[4].end());

                if (auth.registerUser(email, password, fullName, birthDate, phone)) {
                    msg.setFillColor(Color(60, 160, 90));
                    msg.setString(L"Đăng ký thành công! Đang chuyển sang đăng nhập...");
                    showSuccessMessage = true;
                    messageTimer.restart();
                } else {
                    msg.setFillColor(Color(200, 60, 60));
                    msg.setString(L"Đăng ký thất bại. Vui lòng thử lại.");
                }
            }
        } else if (backBtn.getGlobalBounds().contains(mouse)) {
            return true;
        }
    }

    if (event.is<Event::KeyPressed>()) {
        auto keyEvent = event.getIf<Event::KeyPressed>();
        auto code = keyEvent->code;

        if (code == Keyboard::Key::Escape) return true;
        if (code == Keyboard::Key::Tab) {
            activeField = (activeField + 1) % 6;
            showCursor = true;
            cursorClock.restart();
        } else if (code == Keyboard::Key::Backspace && activeField >= 0) {
            if (!inputs[activeField].empty()) {
                inputs[activeField].pop_back();
                showCursor = true;
                cursorClock.restart();
            }
        } else if (code == Keyboard::Key::Enter) {
            if (validateInputs()) {
                string fullName(inputs[0].begin(), inputs[0].end());
                string birthDate(inputs[1].begin(), inputs[1].end());
                string email(inputs[2].begin(), inputs[2].end());
                string phone(inputs[3].begin(), inputs[3].end());
                string password(inputs[4].begin(), inputs[4].end());

                if (auth.registerUser(email, password, fullName, birthDate, phone)) {
                    msg.setFillColor(Color(60, 160, 90));
                    msg.setString(L"Đăng ký thành công! Đang chuyển sang đăng nhập...");
                    showSuccessMessage = true;
                    messageTimer.restart();
                }
            }
        }
    }

    if (event.is<Event::TextEntered>() && activeField >= 0) {
        char32_t unicode = event.getIf<Event::TextEntered>()->unicode;
        // Allow Vietnamese and Unicode (skip control chars and Delete)
        if (unicode >= 32 && unicode != 127 && inputs[activeField].size() < 50) {
            inputs[activeField].push_back((wchar_t)unicode);
            showCursor = true;
            cursorClock.restart();
        }
    }

    return false;
}

void RegisterScreen::draw(RenderWindow& window) {
    auto size = window.getSize();
    overlay.setSize({(float)size.x, (float)size.y});
    Vector2f center({size.x * 0.5f, size.y * 0.5f});

    card.setPosition(center);
    title.setPosition({center.x - 120.f, center.y - 292.f});
    closeX.setPosition({card.getPosition().x + card.getSize().x/2.f - 30.f, card.getPosition().y - card.getSize().y/2.f + 10.f});

    for (int i = 0; i < 6; i++) {
        // Giãn spacing
        float y = center.y - 215.f + i * 80.f; // ↑ tăng spacing từ 60 → 70

        // Căn label cao hơn và gần box hơn
        labels[i].setPosition({center.x - 200.f, y - 24.f});

        // Input box
        inputBoxes[i].setPosition({center.x - 200.f, y});

        // Text bên trong box
        Vector2f textPos = {center.x - 190.f, y + 10.f};
        placeholders[i].setPosition(textPos);
        displays[i].setPosition(textPos);
    }
    // Nút bấm
    registerBtn.setPosition({center.x - 201.f, center.y + 240.f});
    registerBtnText.setPosition({
        registerBtn.getPosition().x + registerBtn.getSize().x / 2.f,
        registerBtn.getPosition().y + registerBtn.getSize().y / 2.f - 3
    });

    backBtn.setPosition({center.x + 59.f, center.y + 240.f});
    backBtnText.setPosition({
        backBtn.getPosition().x + backBtn.getSize().x / 2.f,
        backBtn.getPosition().y + backBtn.getSize().y / 2.f - 3
    });

    // Thông báo lỗi / thành công
    msg.setPosition({center.x - 200.f, center.y + 285.f});

    window.draw(overlay);
    window.draw(card);
    window.draw(title);
    window.draw(closeX);

    for (int i = 0; i < 6; i++) {
        window.draw(labels[i]);
        window.draw(inputBoxes[i]);

        if (inputs[i].empty()) {
            window.draw(placeholders[i]);
        } else {
            if (i == 4 || i == 5)
                displays[i].setString(bullets(inputs[i].size()));
            else
                displays[i].setString(inputs[i]);
            window.draw(displays[i]);
        }

        if (activeField == i && showCursor) {
            Vector2f textPos = displays[i].getPosition();
            FloatRect bounds = displays[i].getLocalBounds();
            cursor.setPosition({textPos.x + bounds.size.x + 2.f, textPos.y - 2.f});
            window.draw(cursor);
        }
    }

    window.draw(registerBtn);
    window.draw(registerBtnText);
    window.draw(backBtn);
    window.draw(backBtnText);
    window.draw(msg);
}

void RegisterScreen::reset() {
    for (int i = 0; i < 6; i++) inputs[i].clear();
    activeField = -1;
    msg.setString(L"");
    showSuccessMessage = false;
}
