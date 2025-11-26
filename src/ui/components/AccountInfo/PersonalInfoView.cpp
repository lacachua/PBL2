#include "UI/components/AccountInfo/PersonalInfoView.h"

PersonalInfoView::PersonalInfoView(const Font& f, AuthService& auth)
    : profileCard_(f, auth)  // Use default dark theme config
{
}

void PersonalInfoView::setUser(const string& email) {
    profileCard_.setUser(email);
}

void PersonalInfoView::update(Vector2f mousePos, bool mousePressed, const Event* event, Vector2f cardPos) {
    profileCard_.setPosition(cardPos);
    
    if (event) {
        profileCard_.handleEvent(*event, mousePos, mousePressed);
    }
    
    profileCard_.update(mousePos, mousePressed);
}

void PersonalInfoView::draw(RenderWindow& window) {
    profileCard_.draw(window);
}
