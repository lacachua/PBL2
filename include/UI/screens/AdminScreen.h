#ifndef ADMIN_SCREEN_H
#define ADMIN_SCREEN_H

#include "UI/screens/BaseScreen.h"
#include "UI/components/Admin/AdminSidebar.h"
#include "UI/components/Admin/MoviePanel.h"
#include "services/AuthService.h"
#include <memory>

/**
 * @brief Main Admin Screen với Sidebar + Dynamic Panel
 * 
 * Layout:
 * [Sidebar 260px] [Main Panel - remaining width]
 */
class AdminScreen : public BaseScreen {
private:
    RenderWindow& window;
    AppState& appState;
    AuthService* authService;
    unique_ptr<AdminSidebar> sidebar;
    
    // Current panel
    AppState currentPanelState;
    unique_ptr<MoviePanel> moviePanel;
    
    // Background
    RectangleShape mainPanelBg;
    
    void switchPanel(AppState newState);
    void handleAdminLogout();
    
public:
    AdminScreen(Font& font, RenderWindow& win, AppState& state, AuthService* auth);
    
    void handleEvent(const Event& event) override;
    void update(Vector2f mousePos, bool mousePressed, AppState& state) override;
    void render();
};

#endif
