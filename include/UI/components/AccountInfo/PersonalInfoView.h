#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "services/AuthService.h"
#include "UI/components/ProfileInfoCard.h"

using namespace sf;
using namespace std;

class PersonalInfoView {
private:
    ProfileInfoCard profileCard_;
    
public:
    PersonalInfoView(const Font& f, AuthService& auth);
    
    void setUser(const string& email);
    void update(Vector2f mousePos, bool mousePressed, const Event* event, Vector2f cardPos);
    void draw(RenderWindow& window);
};
