#include "HomeScreen.h"

HomeScreen::HomeScreen(Font& f)
: font(f),
  buttons{
      TextButton(font, L"CiNeXíNè", 50, Vector2f(60.f, 40.f)),
      TextButton(font, L"Đăng nhập | Đăng ký", 22, Vector2f(1360.f, 50.f)),
      TextButton(font, L"Đặt vé ngay", 23, Vector2f(1150.f, 50.f))
  },
  background("../assets/background.png"),
  searchbar("../assets/search_bar.png"),
  sprite1(background),
  sprite2(searchbar),
  dropdownBox({210.f, 120.f}),
  myticketsButton(font, L"Vé của tôi", 18, Vector2f(0.f, 0.f)),
  accountButton(font, L"Tài khoản của tôi", 18, Vector2f(0.f, 0.f)),
  logoutButton(font, L"Đăng xuất", 18, Vector2f(0.f, 0.f)),
  searchBox(nullptr),
  searchManager(nullptr),
  selectedMovieIndex(-1)
{
    Color accent(20, 118, 172);

    sprite2.setPosition({720.f, 50.f});
    sprite2.setScale({0.2f, 0.2f});

    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].setOutlineColor(accent);
        buttons[i].setOutlineThickness(i == 0 ? 5.f : 2.f);
    }

    dropdownBox.setFillColor(Color(40, 40, 40, 240));
    dropdownBox.setOutlineColor(Color(100, 100, 100));
    dropdownBox.setOutlineThickness(1.f);
    
    // Initialize search box (positioned over the search bar image)
    // search_bar.png is at position 720,50 with scale 0.2
    // Calculate the actual size of the scaled image and position the search box on it
    FloatRect searchBarBounds = sprite2.getGlobalBounds();
    float searchBoxX = searchBarBounds.position.x + 10.f; // Small padding from left edge
    float searchBoxY = searchBarBounds.position.y + (searchBarBounds.size.y - 40.f) / 2.f; // Center vertically
    float searchBoxWidth = searchBarBounds.size.x - 20.f; // Full width minus padding
    
    searchBox = new SearchBox(font, Vector2f(searchBoxX, searchBoxY), Vector2f(searchBoxWidth, 40.f));
}

HomeScreen::~HomeScreen() {
    delete searchBox;
    delete searchManager;
}

void HomeScreen::initializeSearch(const vector<Movie>& movies) {
    if (!searchManager) {
        searchManager = new MovieSearchManager();
    }
    searchManager->loadMovies(movies);
    searchBox->setSearchManager(searchManager);
}

// ✅ Hàm phụ cập nhật vị trí dropdown ngay bên dưới nút "Xin chào"
void HomeScreen::updateDropdownPosition() {
    Vector2f loginButtonPos = buttons[1].getPosition();
    FloatRect loginButtonBounds = buttons[1].getGlobalBounds();

    float dropdownX = loginButtonBounds.position.x + loginButtonBounds.size.x - dropdownBox.getSize().x;
    float dropdownY = loginButtonPos.y + 35.f;

    Vector2f dropdownPos = {dropdownX, dropdownY};
    dropdownBox.setPosition(dropdownPos);
    myticketsButton.setPosition({dropdownPos.x + 15.f, dropdownPos.y + 15.f});
    accountButton.setPosition({dropdownPos.x + 15.f, dropdownPos.y + 45.f});
    logoutButton.setPosition({dropdownPos.x + 15.f, dropdownPos.y + 75.f});
}

void HomeScreen::update(Vector2f mouse, bool mousePressed, AppState& state, const Event* event) {
    // Handle search box events and updates
    if (searchBox) {
        if (event) {
            searchBox->handleEvent(*event);
        }
        searchBox->update(mouse, mousePressed);
        
        // Check if a movie was selected from search
        int movieIdx;
        if (searchBox->hasSelectedMovie(movieIdx)) {
            selectedMovieIndex = movieIdx;
            state = AppState::MOVIE_DETAILS;
            return;
        }
    }
    
    // Don't process other UI if search box is active
    if (searchBox && searchBox->isInputActive()) {
        return;
    }
    
    // Nếu dropdown đang bật thì cập nhật vị trí & xử lý click
    if (isUserLoggedIn && showDropdown) {
        updateDropdownPosition(); // ✅ đảm bảo luôn ở đúng vị trí

        myticketsButton.update(mouse);
        accountButton.update(mouse);
        logoutButton.update(mouse);

        if (mousePressed) {
            if (myticketsButton.isClicked(mouse, mousePressed)) {
                showDropdown = false;
            }
            else if (accountButton.isClicked(mouse, mousePressed)) {
                showDropdown = false;
            }
            else if (logoutButton.isClicked(mouse, mousePressed)) {
                setLoggedUser("");
                showDropdown = false;
            }
            else if (!dropdownBox.getGlobalBounds().contains(mouse) &&
                     !buttons[1].getGlobalBounds().contains(mouse)) {
                showDropdown = false;
            }
        }
    }

    // Xử lý hover + click các nút header
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].update(mouse);

        if (buttons[i].isClicked(mouse, mousePressed)) {
            switch (i) {
                case 0:
                    state = AppState::HOME;
                    break;

                case 1: // Nút "Đăng nhập | Đăng ký" hoặc "Xin chào"
                    if (isUserLoggedIn) {
                        showDropdown = !showDropdown;
                        if (showDropdown) {
                            updateDropdownPosition(); // ✅ cập nhật NGAY khi bật lần đầu
                        }
                    } else {
                        state = AppState::LOGIN;
                    }
                    break;

                case 2:
                    state = AppState::BOOKING;
                    break;
            }
        }
    }
}

void HomeScreen::draw(RenderWindow& window) {
    window.draw(sprite1);
    window.draw(sprite2);

    for (int i = 0; i < BUTTON_COUNT; i++)
        buttons[i].draw(window);
    
    // Note: SearchBox sẽ được vẽ sau slider trong App::render() để không bị che
}

void HomeScreen::drawSearchBox(RenderWindow& window) {
    // Vẽ search box SAU CÙNG để không bị che bởi poster
    if (searchBox) {
        searchBox->draw(window);
    }
}

void HomeScreen::drawDropdown(RenderWindow& window) {
    if (isUserLoggedIn && showDropdown) {
        window.draw(dropdownBox);
        myticketsButton.draw(window);
        accountButton.draw(window);
        logoutButton.draw(window);
    }
}

void HomeScreen::setLoggedUser(const string& username) {
    currentUser = username;
    isUserLoggedIn = !username.empty();

    if (isUserLoggedIn) {
        buttons[1].setString(L"Xin chào, " + String::fromUtf8(username.begin(), username.end()) + L"!");
    } else {
        buttons[1].setString(L"Đăng nhập | Đăng ký");
        showDropdown = false;
    }
}
