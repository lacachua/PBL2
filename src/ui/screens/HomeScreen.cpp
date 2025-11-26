#include "UI/screens/HomeScreen.h"

HomeScreen::HomeScreen(Font& font, RenderWindow& window)
    : BaseScreen(font), win(window)
{
    repo = make_unique<MovieRepository>("../data/movies.txt");
    slider = make_unique<PosterSlider>(font, win);
    slider->loadPosters(*repo, font);

    slider->setOnDetailRequested([&](int index) {
        repo->setSelectedIndex(index);
        win.setFramerateLimit(60);
    });
    
    // ✅ Initialize global search bar with movie data
    initializeGlobalSearch(repo->getAllMovies());
}

void HomeScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    BaseScreen::update(mousePos, mousePressed, state);
    
    // If movie was selected from global search, set it in repository
    int searchIdx = getSelectedMovieIndexFromSearch();
    if (searchIdx >= 0) {
        repo->setSelectedIndex(searchIdx);
        return; // State already changed to MOVIE_DETAILS in BaseScreen
    }
    
    // Don't update slider if search is active
    if (globalSearchBar && globalSearchBar->isInputActive()) return;

    float dt = clock.restart().asSeconds();
    slider->update(dt, win);
}

void HomeScreen::handleEvent(Vector2f mousePos, bool mousePressed, AppState& state, const Event* event) {
    // Handle global search bar events from BaseScreen
    if (event) BaseScreen::handleEvent(*event);
    
    // Only handle slider events if search is not active
    if (!globalSearchBar || !globalSearchBar->isInputActive()) {
        slider->handleEvent(mousePos, mousePressed, state);
    }
}

void HomeScreen::draw(RenderWindow& window) {
    window.draw(background_sprite);
    window.draw(searchBar_sprite);
    for (int i = 0; i < buttons.getSize(); i++)
        buttons[i].draw(window);
    
    slider->draw(window);
    
    // NOTE: GlobalSearchBar is drawn in drawOverlay() to ensure it's on top
    
    if (isUserLoggedIn() && showDropdown) {
        window.draw(dropdownBox);
        accountButton.draw(window);
        logoutButton.draw(window);
    }
}

void HomeScreen::drawHeaderOnly(RenderWindow& window) {
    window.draw(background_sprite);
    window.draw(searchBar_sprite);
    for (int i = 0; i < buttons.getSize(); i++)
        buttons[i].draw(window);
    
    // NOTE: GlobalSearchBar is drawn in drawOverlay() to ensure it's on top
    
    if (isUserLoggedIn() && showDropdown) {
        window.draw(dropdownBox);
        accountButton.draw(window);
        logoutButton.draw(window);
    }
}

void HomeScreen::setLoggedUser(const string& username) {
    currentUser = username;
    isLoggedIn = !username.empty();

    if (isLoggedIn) {
        string firstName = getFirstName(username);
        setAccountButtonText(L"Xin chào, " + sf::String::fromUtf8(firstName.begin(), firstName.end()) + L"!");
    } 
    else setAccountButtonText(L"Đăng nhập | Đăng ký");
}
