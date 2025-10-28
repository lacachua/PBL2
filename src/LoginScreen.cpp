#include "LoginScreen.h"
#include "AppState.h"
#include <SFML/Graphics.hpp>
using namespace sf;

static Text makeText(const Font& font, const String& str, unsigned size) {
    Text t(font, str, size);
    t.setFillColor(Color::Black);
    return t;
}

LoginScreen::LoginScreen(const Font& font, AuthService& authRef)
    : auth(authRef),
      overlay({0.f, 0.f}),
      card({460.f, 380.f}),
      title(font, L"Đăng nhập", 30),
      emailBox({360.f, 44.f}),
      passBox({360.f, 44.f}),
      btn({360.f, 44.f}),
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
    caret.setSize({2.f, 20.f});
    caret.setFillColor(Color::Black);
}

std::wstring LoginScreen::bullets(std::size_t n) { return std::wstring(n, L'\u2022'); }

bool LoginScreen::update(Vector2f mouse, bool mousePressed, const Event& event, string& currentUser, AppState& state) {
    // click ra ngoài card -> đóng
    if (mousePressed && !card.getGlobalBounds().contains(mouse)) return true;

    // click X -> đóng
    if (mousePressed && closeX.getGlobalBounds().contains(mouse)) return true;

    // chọn ô active
    if (mousePressed) {
        emailActive = emailBox.getGlobalBounds().contains(mouse);
        passActive  = passBox.getGlobalBounds().contains(mouse);

        // click Continue
        if (btn.getGlobalBounds().contains(mouse)) {
            // verify
            std::string u(emailInput.begin(), emailInput.end());
            std::string p(passInput.begin(), passInput.end());
            if (auth.verify(u, p)) {
                msg.setFillColor(Color(60, 160, 90));
                msg.setString(L"Đăng nhập thành công.");

                // ensure button flow matches Enter key flow
                loginSuccess = true;
                loggedUser = u;
                loginClock.restart();
            } else {
                msg.setFillColor(Color(200, 60, 60));
                msg.setString(L"Sai tài khoản hoặc mật khẩu.");
            }
        }

        // click "Tạo tài khoản ngay" -> nếu chưa tồn tại sẽ đăng ký luôn
        if (linkCreate.getGlobalBounds().contains(mouse)) 
            state = AppState::REGISTER;
    }

    // phím
    if (event.is<Event::KeyPressed>()) {
        auto code = event.getIf<Event::KeyPressed>()->code;
        if (code == Keyboard::Key::Escape) return true;              // đóng
        if (code == Keyboard::Key::Tab) {                            // chuyển ô
            bool e = emailActive, p = passActive;
            emailActive = !e && !p ? true : !e;
            passActive  = !emailActive;
        } else if (code == Keyboard::Key::Backspace) {               // xóa
            if (emailActive && !emailInput.empty()) emailInput.pop_back();
            else if (passActive && !passInput.empty()) passInput.pop_back();
        } else if (code == Keyboard::Key::Enter) {                   // xác nhận
            std::string email(emailInput.begin(), emailInput.end());
            std::string password(passInput.begin(), passInput.end());
            
            // Validate email format first
            if (!Validator::isValidEmail(email)) {
                msg.setFillColor(Color(200,60,60));
                msg.setString(L"Email không hợp lệ.");
                return false;
            }
            
            // Verify credentials
            if (auth.verify(email, password)) {
                msg.setFillColor(Color(60, 160, 90));
                msg.setString(L"Đăng nhập thành công.");
                
                loginSuccess = true;
                loggedUser = email;
                loginClock.restart();
            } else {
                msg.setFillColor(Color(200,60,60));
                
                // More specific error message
                if (!auth.emailExists(email)) {
                    msg.setString(L"Email không tồn tại.");
                } else {
                    msg.setString(L"Mật khẩu không đúng.");
                }
            }
        }
    }

    if (event.is<Event::TextEntered>()) {
        char32_t unicode = event.getIf<Event::TextEntered>()->unicode;
        // Allow Vietnamese and Unicode (skip control chars and Delete)
        if (unicode >= 32 && unicode != 127) {
            if (emailActive && emailInput.size() < 48) emailInput.push_back((wchar_t)unicode);
            else if (passActive && passInput.size() < 48)  passInput.push_back((wchar_t)unicode);
        }
    }

    if (caretClock.getElapsedTime().asSeconds() >= 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }

    if (loginSuccess && loginClock.getElapsedTime().asSeconds() >= 1.f) {
        currentUser = loggedUser;
        return true;   // báo cho App biết là đóng login và quay lại home
    }
    return false; // không đóng
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

    passDisplay.setString(bullets(passInput.size()));
    passDisplay.setPosition({passBox.getPosition().x + 12.f, passBox.getPosition().y + 10.f});

    // placeholder / text thật
    if (emailInput.empty()) window.draw(emailPH); else window.draw(emailDisplay);
    if (passInput.empty()) window.draw(passPH); else window.draw(passDisplay);

    // Draw a thin rectangle caret instead of a '|' text glyph
    if (caretVisible) {
        if (emailActive) {
            Vector2f textPos = emailDisplay.getPosition();
            FloatRect bounds = emailDisplay.getLocalBounds();
            caret.setSize({2.f, static_cast<float>(emailDisplay.getCharacterSize()) + 4.f});
            caret.setPosition({textPos.x + bounds.size.x + 2.f, textPos.y - 2.f});
            window.draw(caret);
        } else if (passActive) {
            Vector2f textPos = passDisplay.getPosition();
            FloatRect bounds = passDisplay.getLocalBounds();
            caret.setSize({2.f, static_cast<float>(passDisplay.getCharacterSize()) + 4.f});
            caret.setPosition({textPos.x + bounds.size.x + 2.f, textPos.y - 2.f});
            window.draw(caret);
        }
    }
}

