#include "UI/components/Admin/AdminProfilePanel.h"
#include <algorithm>
#include <cwctype>

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
}

// Create light theme config for Admin
static ProfileInfoCard::Config createAdminConfig() {
    ProfileInfoCard::Config cfg;
    cfg.darkMode = false;
    cfg.backgroundColor = Color(255, 255, 255);
    cfg.labelColor = Color(108, 122, 140);
    cfg.titleColor = Color(19, 33, 58);
    cfg.inputBgColor = Color(248, 250, 252);
    cfg.inputTextColor = Color(19, 33, 58);
    cfg.outlineColor = Color(214, 222, 230);
    cfg.accentColor = Color(20, 118, 172);
    cfg.readOnlyBgColor = Color(238, 241, 245);
    cfg.readOnlyTextColor = Color(120, 130, 145);
    cfg.successColor = Color(63, 151, 90);
    cfg.errorColor = Color(209, 67, 67);
    return cfg;
}

AdminProfilePanel::AdminProfilePanel(Font& f, float w, float h, AuthService* auth)
    : font(f),
      width(w),
      height(h),
      position(Vector2f(0.f, 0.f)),
      authService(auth),
      panelBackground(Vector2f(w, h)),
      contentCard(),
      heroBanner(),
      avatarCircle(),
      heroNameText(f, L"", 22),
      heroMetaText(f, L"", 15),
      avatarInitialsText(f, L"", 20),
      profileCard_(f, *auth, createAdminConfig())
{
    // Colors
    Color panelColor(241, 245, 250);
    Color cardColor(255, 255, 255);
    Color outlineColor(214, 222, 230);
    Color mutedText(108, 122, 140);
    Color headingColor(19, 33, 58);
    Color heroBg(227, 233, 243);
    Color chipColor(20, 118, 172);

    panelBackground.setFillColor(panelColor);

    contentCard.setFillColor(cardColor);
    contentCard.setOutlineColor(outlineColor);
    contentCard.setOutlineThickness(1.2f);

    heroBanner.setFillColor(heroBg);
    heroBanner.setOutlineColor(outlineColor);
    heroBanner.setOutlineThickness(1.f);

    avatarCircle.setRadius(26.f);
    avatarCircle.setFillColor(chipColor);

    heroNameText.setFillColor(headingColor);
    heroNameText.setStyle(Text::Bold);
    heroMetaText.setFillColor(mutedText);
    avatarInitialsText.setFillColor(Color::White);
    avatarInitialsText.setStyle(Text::Bold);

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
    float heroHeight = 72.f;
    float contentWidth = width - outerPadding * 2.f;

    // Hero banner
    Vector2f heroPos(position.x + outerPadding, position.y + outerPadding);
    heroBanner.setPosition(heroPos);
    heroBanner.setSize(Vector2f(contentWidth, heroHeight));

    // Avatar
    float avatarRadius = avatarCircle.getRadius();
    float avatarY = heroPos.y + (heroHeight - avatarRadius * 2.f) / 2.f;
    avatarCircle.setPosition(Vector2f(heroPos.x + 20.f, avatarY));
    
    // Center initials in avatar
    FloatRect initialsBounds = avatarInitialsText.getLocalBounds();
    float avatarCenterX = avatarCircle.getPosition().x + avatarRadius;
    float avatarCenterY = avatarCircle.getPosition().y + avatarRadius;
    avatarInitialsText.setPosition(Vector2f(
        avatarCenterX - (initialsBounds.size.x / 2.f) - initialsBounds.position.x,
        avatarCenterY - (initialsBounds.size.y / 2.f) - initialsBounds.position.y
    ));

    // Hero text
    float textStartX = avatarCircle.getPosition().x + avatarRadius * 2.f + 16.f;
    float textCenterY = heroPos.y + heroHeight / 2.f;
    heroNameText.setPosition(Vector2f(textStartX, textCenterY - 20.f));
    heroMetaText.setPosition(Vector2f(textStartX, textCenterY + 6.f));

    // Content card
    float cardStartY = heroPos.y + heroHeight + 16.f;
    contentCard.setPosition({heroPos.x, cardStartY});
    contentCard.setSize({contentWidth, height - cardStartY - outerPadding + position.y});

    // Profile card inside content card
    profileCard_.setPosition({heroPos.x + 28.f, cardStartY + 28.f});
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
    
    // Update profile card
    profileCard_.setUser(currentEmail);
    
    updateHeroContents();
    userDataLoaded = true;
}

void AdminProfilePanel::handleEvent(const Event& event, const RenderWindow& window) {
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    bool mousePressed = Mouse::isButtonPressed(Mouse::Button::Left);
    
    profileCard_.handleEvent(event, mousePos, mousePressed);
}

void AdminProfilePanel::update(Vector2f mousePos, bool mousePressed) {
    refreshUser();
    layoutElements();
    
    profileCard_.update(mousePos, mousePressed);
}

void AdminProfilePanel::render(RenderWindow& window) {
    window.draw(panelBackground);
    window.draw(heroBanner);
    window.draw(avatarCircle);
    window.draw(avatarInitialsText);
    window.draw(heroNameText);
    window.draw(heroMetaText);
    window.draw(contentCard);
    
    profileCard_.draw(window);
}

void AdminProfilePanel::reload() {
    userDataLoaded = false;
    refreshUser();
}

void AdminProfilePanel::updateHeroContents() {
    if (!currentUser) return;

    // Display name
    wstring fullName = utf8_to_wstring(currentUser->getFullName());
    wstring displayName = fullName.empty() 
        ? utf8_to_wstring(currentUser->getEmail().substr(0, currentUser->getEmail().find('@')))
        : fullName;
    if (!displayName.empty()) {
        displayName[0] = towupper(displayName[0]);
    }
    heroNameText.setString(L"HỒ SƠ CỦA " + displayName);

    // Meta line
    wstring roleDescription = getRoleLabel(currentUser->getRole());
    wstring emailW = utf8_to_wstring(currentUser->getEmail());
    heroMetaText.setString(emailW + L"  -  " + roleDescription);

    // Avatar initials
    wstring initials = buildInitials(currentUser->getFullName());
    if (initials.empty()) {
        initials = buildInitials(currentUser->getEmail());
    }
    if (initials.length() > 1) initials = initials.substr(0, 1);
    avatarInitialsText.setString(initials);
}

wstring AdminProfilePanel::getRoleLabel(AppRole role) const {
    switch (role) {
        case AppRole::Admin: return L"Quản trị viên";
        case AppRole::Customer: return L"Khách hàng";
        default: return L"Khách";
    }
}

wstring AdminProfilePanel::buildInitials(const string& source) const {
    wstring wide = utf8_to_wstring(source);
    wstring result;
    bool takeNext = true;
    for (wchar_t ch : wide) {
        if (iswalpha(ch) || iswdigit(ch)) {
            if (takeNext) {
                result.push_back(towupper(ch));
                takeNext = false;
                if (result.size() == 2) break;
            }
        } else if (ch == L' ' || ch == L'-') {
            takeNext = true;
        }
    }
    if (result.empty() && !wide.empty()) {
        result.push_back(towupper(wide.front()));
    }
    return result;
}
