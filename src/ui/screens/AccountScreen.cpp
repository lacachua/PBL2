#include "UI/screens/AccountScreen.h"
#include "models/MovieRepository.h"
#include <iostream>

using namespace std;

AccountScreen::AccountScreen(Font& f, AuthService& auth) 
    : BaseScreen(f),
      authService(&auth),
      menuItem1(f, L"THÔNG TIN KHÁCH HÀNG", 300.f, 60.f, 18),
      menuItem2(f, L"LỊCH SỬ ĐẶT VÉ", 300.f, 60.f, 18),
      menuItem3(f, L"VOUCHER CỦA TÔI", 300.f, 60.f, 18)
{
    personalInfoView = make_unique<PersonalInfoView>(f, auth);
    purchaseHistoryView = make_unique<PurchaseHistoryView>(f);
    voucherListView = make_unique<VoucherListView>(f);

    MovieRepository repo("../data/movies.txt");
    initializeGlobalSearch(repo.getAllMovies());
    
    mainCardBg.setFillColor(Color(0, 24, 48, 235));
    mainCardBg.setOutlineThickness(1.f);
    mainCardBg.setOutlineColor(Color(10, 51, 92));
    
    sidebarBg.setSize({340.f, 768.f});
    sidebarBg.setFillColor(Color(20, 35, 60, 240));
    
    menuItem1.setFillColor(Color(65, 135, 220, 255));
    menuItem1.setTextColor(Color::White);
    menuItem1.setOutlineThickness(0.f);

    menuItem2.setFillColor(Color(35, 55, 85, 220));
    menuItem2.setTextColor(Color(180, 195, 215));
    menuItem2.setOutlineThickness(0.f);

    menuItem3.setFillColor(Color(35, 55, 85, 220));
    menuItem3.setTextColor(Color(180, 195, 215));
    menuItem3.setOutlineThickness(0.f);
    
    currentTab = AccountTab::CUSTOMER_INFO;
}

void AccountScreen::setCurrentUser(const string& email) {
    if (email.empty() || email == currentUserEmail) return;

    currentUserEmail = email;
    personalInfoView->setUser(email);
    purchaseHistoryView->setUserEmail(email);
    purchaseHistoryView->loadTickets();
    voucherListView->setUser(email);
}

void AccountScreen::updatePositions(Vector2u windowSize) {
    float windowW = static_cast<float>(windowSize.x);
    float windowH = static_cast<float>(windowSize.y);
    
    float sidebarWidth = windowW * 0.22f;
    float sidebarX = 0.f;
    float sidebarY = 125.f;
    
    sidebarBg.setSize({sidebarWidth, windowH - sidebarY});
    sidebarBg.setPosition({sidebarX, sidebarY});
    
    float menuItemWidth = sidebarWidth * 0.85f;
    float menuStartY = sidebarY + 100.f;
    float menuSpacing = 90.f;
    float menuX = sidebarX + (sidebarWidth - menuItemWidth) / 2.f;
    
    menuItem1.setPosition({menuX, menuStartY});
    menuItem2.setPosition({menuX, menuStartY + menuSpacing});
    menuItem3.setPosition({menuX, menuStartY + menuSpacing * 2});
    
    float baseLeft = sidebarWidth + 30.f;
    float rightMargin = 50.f;
    float cardY = sidebarY;
    float cardWidth = 1010.f;
    float cardHeight = 766.f;
    
    float available = max(0.f, windowW - baseLeft - rightMargin);
    float cardX = baseLeft + max(0.f, (available - cardWidth) / 2.f);
    
    mainCardBg.setSize({cardWidth, cardHeight});
    mainCardBg.setPosition({cardX, cardY});
}

void AccountScreen::update(Vector2f mousePos, bool mousePressed, const Event* event, AppState& state) {
    BaseScreen::update(mousePos, mousePressed, state);
    if (event) BaseScreen::handleEvent(*event);
    if (globalSearchBar && globalSearchBar->isInputActive()) return;
    
    Color hoverColor(50, 70, 100, 230);
    Color activeColor = Color(65, 135, 220, 255);
    Color inactiveColor = Color(35, 55, 85, 220);
    
    Color color1 = (currentTab == AccountTab::CUSTOMER_INFO) ? activeColor : inactiveColor;
    Color color2 = (currentTab == AccountTab::PURCHASE_HISTORY) ? activeColor : inactiveColor;
    Color color3 = (currentTab == AccountTab::MY_GIFTS) ? activeColor : inactiveColor;
    
    Color hover1 = (currentTab == AccountTab::CUSTOMER_INFO) ? activeColor : hoverColor;
    Color hover2 = (currentTab == AccountTab::PURCHASE_HISTORY) ? activeColor : hoverColor;
    Color hover3 = (currentTab == AccountTab::MY_GIFTS) ? activeColor : hoverColor;
    
    menuItem1.update(mousePos, mousePressed, hover1, color1);
    menuItem2.update(mousePos, mousePressed, hover2, color2);
    menuItem3.update(mousePos, mousePressed, hover3, color3);
    
    bool mouseJustPressed = mousePressed && !wasMousePressed;
    wasMousePressed = mousePressed;
    
    if (mouseJustPressed) {
        if (menuItem1.isClicked(mousePos, true)) {
            currentTab = AccountTab::CUSTOMER_INFO;
            menuItem1.setFillColor(Color(65, 135, 220, 255));
            menuItem1.setTextColor(Color::White);
            menuItem2.setFillColor(Color(35, 55, 85, 220));
            menuItem2.setTextColor(Color(180, 195, 215));
            menuItem3.setFillColor(Color(35, 55, 85, 220));
            menuItem3.setTextColor(Color(180, 195, 215));
        }
        else if (menuItem2.isClicked(mousePos, true)) {
            currentTab = AccountTab::PURCHASE_HISTORY;
            menuItem1.setFillColor(Color(35, 55, 85, 220));
            menuItem1.setTextColor(Color(180, 195, 215));
            menuItem2.setFillColor(Color(65, 135, 220, 255));
            menuItem2.setTextColor(Color::White);
            menuItem3.setFillColor(Color(35, 55, 85, 220));
            menuItem3.setTextColor(Color(180, 195, 215));
            
            if (purchaseHistoryView) purchaseHistoryView->loadTickets(true);
        }
        else if (menuItem3.isClicked(mousePos, true)) {
            currentTab = AccountTab::MY_GIFTS;
            menuItem1.setFillColor(Color(35, 55, 85, 220));
            menuItem1.setTextColor(Color(180, 195, 215));
            menuItem2.setFillColor(Color(35, 55, 85, 220));
            menuItem2.setTextColor(Color(180, 195, 215));
            menuItem3.setFillColor(Color(65, 135, 220, 255));
            menuItem3.setTextColor(Color::White);
            
            if (voucherListView) voucherListView->refresh();
        }
    }
    
    Vector2f cardPos = mainCardBg.getPosition();
    
    if (currentTab == AccountTab::CUSTOMER_INFO) 
        personalInfoView->update(mousePos, mousePressed, event, cardPos);
    else if (currentTab == AccountTab::PURCHASE_HISTORY) 
        purchaseHistoryView->update(mousePos, mouseJustPressed, cardPos);
    else if (currentTab == AccountTab::MY_GIFTS && voucherListView) 
        voucherListView->update(mousePos, mouseJustPressed, cardPos, mainCardBg.getSize());
}

void AccountScreen::draw(RenderWindow& window) {
    BaseScreen::draw(window);
    
    updatePositions(window.getSize());
    
    window.draw(sidebarBg);
    menuItem1.draw(window);
    menuItem2.draw(window);
    menuItem3.draw(window);
    
    window.draw(mainCardBg);
    
    if (currentTab == AccountTab::CUSTOMER_INFO) 
        personalInfoView->draw(window);
    else if (currentTab == AccountTab::PURCHASE_HISTORY) 
        purchaseHistoryView->draw(window, mainCardBg.getPosition());
    else if (currentTab == AccountTab::MY_GIFTS && voucherListView) 
        voucherListView->draw(window, mainCardBg.getPosition(), mainCardBg.getSize());
}
