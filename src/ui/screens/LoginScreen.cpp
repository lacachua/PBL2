#include "UI/screens/LoginScreen.h"    
#include "core/AppState.h"
#include "core/AppRole.h"
#include <SFML/Graphics.hpp>

using namespace sf;

LoginScreen::LoginScreen(const Font& font, AuthService& authRef)
    : auth(authRef),
      overlay({0.f, 0.f}),
      card({460.f, 380.f}),
      emailBox({360.f, 44.f}),
      passBox({360.f, 44.f}),
      btn({360.f, 44.f}),
      eyeToggleArea({40.f, 44.f}),
    caret({0.f, 0.f}),
    title(font, L"Đăng nhập", 30),
      emailPH(font, L"Nhập email của bạn", 18),
      passPH(font, L"Nhập mật khẩu", 18),
      btnText(font, L"Tiếp tục", 20),
      linkForgot(font, L"Quên mật khẩu?", 16),
      linkCreate(font, L"Tạo tài khoản ngay", 16),
      closeX(font, L"X", 24),
      emailDisplay(font, L"", 18),
      passDisplay(font, L"", 18),
      msg(font, L"", 16)
{
    overlay.setFillColor(Color(0, 0, 0, 170));

    card.setFillColor(Color(255, 255, 255, 240));
    card.setOrigin(card.getSize() / 2.f);

    title.setFillColor(Color::Black);

    emailBox.setFillColor(Color(238, 238, 238));
    emailBox.setOutlineThickness(2.f);
    emailBox.setOutlineColor(Color(200,200,200));
    emailPH.setFillColor(Color(100,100,100));

    passBox = emailBox;
    passPH.setFillColor(Color(100,100,100));

    btn.setFillColor(Color(100, 149, 237));
    btnText.setFillColor(Color::White);

    linkForgot.setFillColor(Color(90, 90, 90));
    linkCreate.setFillColor(Color(36, 95, 180));
    closeX.setFillColor(Color(90, 90, 90));

    emailDisplay.setFillColor(Color::Black);
    passDisplay.setFillColor(Color::Black);

    msg.setFillColor(Color(200,60,60)); // mặc định đỏ

    // caret setup: thin vertical bar similar to a native text cursor
    caret.setSize({2.f, 24.f});
    caret.setFillColor(Color::Black);

    // Eye toggle setup - load textures
    eyeToggleArea.setFillColor(Color::Transparent);
    if (!eyeOpenTexture.loadFromFile("../assets/elements/eye-open.png")) {
        // Fallback if texture not found
    }
    if (!eyeClosedTexture.loadFromFile("../assets/elements/eye-close.png")) {
        // Fallback if texture not found
    }
    eyeOpenTexture.setSmooth(true);
    eyeClosedTexture.setSmooth(true);
    eyeSprite = make_unique<Sprite>(eyeClosedTexture);  // Default: password hidden
}

wstring LoginScreen::bullets(size_t n) { 
    return wstring(n, L'\u2022'); 
}

void LoginScreen::handleEvent(const Event& event, AppState& state, string& user, string& email, bool& successFlag) {
    (void)user;
    (void)email;
    if (const auto* keyEvent = event.getIf<Event::KeyPressed>()) {
        auto code = keyEvent->code;
        if (code == Keyboard::Key::Escape) {
            state = AppState::HOME;
            // Reset logic...
            return;
        }
        if (code == Keyboard::Key::Tab) {
            bool e = emailActive, p = passActive;
            emailActive = !e && !p ? true : !e;
            passActive  = !emailActive;
        } 
        else if (code == Keyboard::Key::Backspace) {
            if (emailActive && !emailInput.empty()) emailInput.pop_back();
            else if (passActive && !passInput.empty()) passInput.pop_back();
        } 
        else if (code == Keyboard::Key::Enter) {
            string uStr(emailInput.begin(), emailInput.end());
            string pStr(passInput.begin(), passInput.end());
            
            if (auth.login(uStr, pStr)) {
                msg.setFillColor(Color(60, 160, 90));
                msg.setString(L"Đăng nhập thành công.");
                loginSuccess = true;
                successFlag = true;
                loggedUser = uStr;
                loginClock.restart();
            } 
            else {
                msg.setFillColor(Color(200, 60, 60));
                msg.setString(L"Sai thông tin đăng nhập.");
            }
        }
    }
    
    if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
        char32_t unicode = textEvent->unicode;
        if (unicode >= 32 && unicode != 127) {
            if (emailActive && emailInput.size() < 48) emailInput.push_back((wchar_t)unicode);
            else if (passActive && passInput.size() < 48) passInput.push_back((wchar_t)unicode);
        }
    }
}

bool LoginScreen::update(Vector2f mouse, bool mousePressed, string& currentUser, string& currentUserEmail, AppState& state) {
    const Vector2f center(864.f, 486.f);
    card.setPosition(center);
    closeX.setPosition({card.getPosition().x + card.getSize().x / 2.f - 28.f,
                        card.getPosition().y - card.getSize().y / 2.f + 8.f});

    if (mousePressed && closeX.getGlobalBounds().contains(mouse)) {
        state = AppState::HOME;
        return false;
    }

    if (mousePressed) {
        if (eyeToggleArea.getGlobalBounds().contains(mouse)) showPassword = !showPassword;
        emailActive = emailBox.getGlobalBounds().contains(mouse);
        passActive  = passBox.getGlobalBounds().contains(mouse);

        if (btn.getGlobalBounds().contains(mouse)) {
            string u(emailInput.begin(), emailInput.end());
            string p(passInput.begin(), passInput.end());
            if (auth.login(u, p)) {
                msg.setFillColor(Color(60, 160, 90));
                msg.setString(L"Đăng nhập thành công.");
                loginSuccess = true;
                loggedUser = u;
                loginClock.restart();
            } 
            else {
                msg.setFillColor(Color(200, 60, 60));
                msg.setString(L"Sai thông tin đăng nhập.");
            }
        }

        if (linkCreate.getGlobalBounds().contains(mouse)) state = AppState::REGISTER;
    }

    if (caretClock.getElapsedTime().asSeconds() >= 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }

    if (loginSuccess && loginClock.getElapsedTime().asSeconds() >= 0.5f) {
        currentUserEmail = loggedUser;
        User* userObj = auth.getUser(loggedUser);
        if (userObj) {
            currentUser = userObj->getUsername();
            state = (userObj->getRole() == AppRole::Admin) ? AppState::ADMIN_DASHBOARD : AppState::HOME;
            loginSuccess = false; // Reset for next login
        }
        return true; 
    }
    return false;
}

void LoginScreen::draw(RenderWindow& window) {
    auto size = window.getSize();
    overlay.setSize({(float)size.x, (float)size.y});
    Vector2f center({size.x * 0.5f, size.y * 0.5f});
    card.setPosition(center);

    title.setPosition({center.x - 90.f, center.y - 160.f});

    emailBox.setPosition({center.x - 180.f, center.y - 96.f});
    emailPH.setPosition({emailBox.getPosition().x + 12.f, emailBox.getPosition().y + 10.f});

    passBox.setPosition({center.x - 180.f, center.y - 42.f});
    passPH.setPosition({passBox.getPosition().x + 12.f, passBox.getPosition().y + 10.f});

    // Eye toggle - show/hide password icon
    const float eyeIconSize = 20.f;
    const float eyePadding = 12.f;  // Padding from right edge
    
    eyeToggleArea.setSize({eyeIconSize + eyePadding * 2.f, passBox.getSize().y});
    eyeToggleArea.setPosition({
        passBox.getPosition().x + passBox.getSize().x - eyeToggleArea.getSize().x,
        passBox.getPosition().y
    });
    
    if (eyeSprite) {
        eyeSprite->setTexture(showPassword ? eyeOpenTexture : eyeClosedTexture);
        
        // Scale to target size
        auto texSize = eyeSprite->getTexture().getSize();
        float scale = eyeIconSize / static_cast<float>(std::max(texSize.x, texSize.y));
        eyeSprite->setScale({scale, scale});
        
        // Position icon with padding from right edge of passBox
        float scaledW = texSize.x * scale;
        float scaledH = texSize.y * scale;
        eyeSprite->setPosition({
            passBox.getPosition().x + passBox.getSize().x - scaledW - eyePadding,
            passBox.getPosition().y + (passBox.getSize().y - scaledH) / 2.f
        });
    }

    btn.setPosition({center.x - 181.f, center.y + 14.f});
    btnText.setPosition({btn.getPosition().x + 125.f, btn.getPosition().y + 6.f});

    linkForgot.setPosition({center.x - 180.f, center.y + 70.f});
    linkCreate.setPosition({center.x - 180.f, center.y + 96.f});

    closeX.setPosition({card.getPosition().x + card.getSize().x/2.f - 28.f,
                        card.getPosition().y - card.getSize().y/2.f + 8.f});

    msg.setPosition({center.x - 180.f, center.y + 132.f});

    // vẽ
    window.draw(overlay);
    window.draw(card);

    window.draw(title);
    window.draw(emailBox);
    window.draw(passBox);
    window.draw(btn);
    window.draw(btnText);
    window.draw(linkForgot);
    window.draw(linkCreate);
    window.draw(closeX);
    window.draw(msg);

    // Prepare displays (set strings regardless of drawing placeholders)
    emailDisplay.setString(emailInput);
    emailDisplay.setPosition({emailBox.getPosition().x + 12.f, emailBox.getPosition().y + 10.f});

    // Show password as text or bullets based on toggle
    if (showPassword) {
        passDisplay.setString(passInput);
    } else {
        passDisplay.setString(bullets(passInput.size()));
    }
    passDisplay.setPosition({passBox.getPosition().x + 12.f, passBox.getPosition().y + 10.f});

    // placeholder / text thật
    if (emailInput.empty()) window.draw(emailPH); else window.draw(emailDisplay);
    if (passInput.empty()) window.draw(passPH); else window.draw(passDisplay);

    // Draw eye toggle icon
    window.draw(eyeToggleArea);
    if (eyeSprite) {
        window.draw(*eyeSprite);
    }

    // Draw a thin rectangle caret instead of a '|' text glyph
    if (caretVisible) {
        if (emailActive) {
            Vector2f textPos = emailDisplay.getPosition();
            FloatRect bounds = emailDisplay.getLocalBounds();
            caret.setSize({2.f, emailBox.getSize().y - 8.f});
            float caretY = emailBox.getPosition().y + (emailBox.getSize().y - caret.getSize().y) / 2.f;
            caret.setPosition({textPos.x + bounds.size.x + 2.f, caretY});
            window.draw(caret);
        } else if (passActive) {
            Vector2f textPos = passDisplay.getPosition();
            FloatRect bounds = passDisplay.getLocalBounds();
            caret.setSize({2.f, passBox.getSize().y - 8.f});
            float caretY = passBox.getPosition().y + (passBox.getSize().y - caret.getSize().y) / 2.f;
            caret.setPosition({textPos.x + bounds.size.x + 2.f, caretY});
            window.draw(caret);
        }
    }
}

