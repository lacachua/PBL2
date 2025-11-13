#include "UI/screens/AdminScreen.h"
#include <iostream>

AdminScreen::AdminScreen(Font& font, RenderWindow& win, AppState& state, AuthService* auth)
    : BaseScreen(font), window(win), appState(state), authService(auth), currentPanelState(AppState::ADMIN_DASHBOARD) {
    
    // Get current user
    User* currentUser = authService ? authService->getCurrentUser() : nullptr;
    string userName = currentUser ? currentUser->getFullName() : "Admin";
    
    // Create sidebar
    sidebar = make_unique<AdminSidebar>(260, window.getSize().y, userName);
    sidebar->setPosition(Vector2f(0, 0));
    
    // Set callbacks
    sidebar->setOnStateChange([this](AppState newState) {
        switchPanel(newState);
    });
    
    sidebar->setOnLogout([this]() {
        handleAdminLogout();
    });
    
    // Main panel background
    mainPanelBg.setSize(Vector2f(window.getSize().x - 260, window.getSize().y));
    mainPanelBg.setPosition(Vector2f(260, 0));
    mainPanelBg.setFillColor(Color(20, 30, 48)); // Slightly lighter than sidebar
    
    // Start with dashboard (or first panel)
    switchPanel(AppState::ADMIN_MOVIES);
}

void AdminScreen::switchPanel(AppState newState) {
    currentPanelState = newState;
    sidebar->setActiveState(newState);
    
    float panelWidth = window.getSize().x - 260 - 40;
    float panelHeight = window.getSize().y - 40;
    
    // Create appropriate panel based on state
    switch (newState) {
        case AppState::ADMIN_MOVIES:
            moviePanel = make_unique<MoviePanel>(font, panelWidth, panelHeight);
            moviePanel->setPosition(Vector2f(280, 20));
            break;
            
        case AppState::ADMIN_ROOMS:
            // TODO: Implement RoomPanel
            cout << "[AdminScreen] RoomPanel not implemented yet\n";
            moviePanel = nullptr;
            break;
            
        case AppState::ADMIN_SHOWTIMES:
        case AppState::ADMIN_TICKETS:
        case AppState::ADMIN_COMBOS:
        case AppState::ADMIN_STAFF:
        case AppState::ADMIN_CUSTOMERS:
        case AppState::ADMIN_REVENUE:
        case AppState::ADMIN_SOLD_TICKETS:
        case AppState::ADMIN_CHANGE_PASSWORD:
            // TODO: Implement other panels
            cout << "[AdminScreen] Panel not implemented yet\n";
            moviePanel = nullptr;
            break;
            
        default:
            moviePanel = nullptr;
            break;
    }
}

void AdminScreen::handleAdminLogout() {
    cout << "[AdminScreen] Logging out...\n";
    authService->logout();
    appState = AppState::LOGIN;
}

void AdminScreen::handleEvent(const Event& event) {
    sidebar->handleEvent(event, window);
    
    // Pass events to current panel
    if (moviePanel) {
        moviePanel->handleEvent(event, window);
    }
}

void AdminScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    sidebar->update(window);
    appState = state; // Sync state changes
    
    // Update current panel
    if (moviePanel) {
        moviePanel->update(mousePos, mousePressed);
    }
}

void AdminScreen::render() {
    // Draw main panel background
    window.draw(mainPanelBg);
    
    // Draw current panel
    if (moviePanel) {
        moviePanel->render(window);
    }
    
    // Draw sidebar on top
    sidebar->render(window);
}
