#include "AdminScreen.h"

AdminScreen::AdminScreen(RenderWindow& window) : 
    uppercase_font("../assets/Montserrat_SemiBold.ttf"),
    lowercase_font("../assets/quicksand_medium.ttf"),
    sidebar({300.f, (float)window.getSize().y}),
    admin_logo({300.f, 80.f}),
    admin_logoText(uppercase_font, "ADMIN", 36),
    arrow_down("../assets/elements/arrow_down.png"),
    arrow_up("../assets/elements/arrow_up.png"),
    arrow_down_tex(arrow_down),
    arrow_up_tex(arrow_up)
{
    sidebar.setFillColor(Color(13, 16, 69));
    admin_logo.setFillColor(Color(59, 108, 177));
    admin_logoText.setOutlineColor(Color(105, 75, 160));
    admin_logoText.setOutlineThickness(2.f);
    admin_logoText.setPosition({
        admin_logo.getPosition().x + admin_logo.getSize().x / 2 - admin_logoText.getGlobalBounds().size.x / 2, 
        admin_logo.getPosition().y + admin_logo.getSize().y / 2 - admin_logoText.getGlobalBounds().size.y + 8.f});

    menuItems = {
        {lowercase_font, L"Dashboard", 300.f, 60.f},
        {lowercase_font, L"Quản lý rạp phim", 300.f, 60.f},
        {lowercase_font, L"Quản lý phim", 300.f, 60.f},
        {lowercase_font, L"Quản lý lịch chiếu", 300.f, 60.f},
        {lowercase_font, L"Quản lý suất chiếu", 300.f, 60.f},
        {lowercase_font, L"Quản lý đơn hàng", 300.f, 60.f},
        {lowercase_font, L"Quản lý giá vé", 300.f, 60.f},
        {lowercase_font, L"Quản lý khuyến mãi", 300.f, 60.f},
        {lowercase_font, L"Quản lý user", 300.f, 60.f},
        {lowercase_font, L"Quản lý combo - nước", 300.f, 60.f},
    };

    expanded.resize(menuItems.size(), false);
    subItems.resize(menuItems.size());
    
    subItems[0].push_back(Button(lowercase_font, L"Thống kê tổng quan", 300.f, 40.f, 16));

    subItems[1].push_back(Button(lowercase_font, L"Danh sách rạp", 300.f, 40.f, 16));
    subItems[1].push_back(Button(lowercase_font, L"Thêm rạp mới", 300.f, 40.f, 16));
    subItems[1].push_back(Button(lowercase_font, L"Sửa thông tin rạp", 300.f, 40.f, 16));

    subItems[2].push_back(Button(lowercase_font, L"Danh sách phim", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Thêm phim mới", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Chỉnh sửa phim", 300.f, 40.f, 16));
    subItems[2].push_back(Button(lowercase_font, L"Xoá phim", 300.f, 40.f, 16));
    
    subItems[3].push_back(Button(lowercase_font, L"Danh sách lịch chiếu", 300.f, 40.f, 16));
    subItems[3].push_back(Button(lowercase_font, L"Thêm lịch chiếu", 300.f, 40.f, 16));
    
    subItems[4].push_back(Button(lowercase_font, L"Danh sách suất chiếu", 300.f, 40.f, 16));
    subItems[4].push_back(Button(lowercase_font, L"Thêm suất chiếu", 300.f, 40.f, 16));
    
    subItems[5].push_back(Button(lowercase_font, L"Danh sách đơn hàng", 300.f, 40.f, 16));
    
    subItems[6].push_back(Button(lowercase_font, L"Danh sách giá vé", 300.f, 40.f, 16));
    subItems[6].push_back(Button(lowercase_font, L"Thêm giá vé", 300.f, 40.f, 16));
    
    subItems[7].push_back(Button(lowercase_font, L"Danh sách khuyến mãi", 300.f, 40.f, 16));
    subItems[7].push_back(Button(lowercase_font, L"Thêm mã giảm giá", 300.f, 40.f, 16));
    
    subItems[8].push_back(Button(lowercase_font, L"Danh sách user", 300.f, 40.f, 16));
    
    subItems[9].push_back(Button(lowercase_font, L"Danh sách combo", 300.f, 40.f, 16));
    subItems[9].push_back(Button(lowercase_font, L"Thêm combo mới", 300.f, 40.f, 16));
    
    for (int i = 0; i < menuItems.size(); i++) {
        menuItems[i].setPosition({0.f, 80.f + i * 60.f});
        menuItems[i].setNormalColor(Color(13, 16, 69));
        menuItems[i].setHoverColor(Color(57, 145, 222));
        menuItems[i].setOutlineThickness(0);
    }

    for (int i = 0; i < subItems.size(); i++) {
        for (int j = 0; j < subItems[i].size(); j++) {
            subItems[i][j].setNormalColor(Color(10, 15, 56));
            subItems[i][j].setHoverColor(Color(20, 25, 76));
            subItems[i][j].setTextFillColor(Color::White);
            subItems[i][j].setOutlineThickness(0);
        }
    }

    arrow_down_tex.setScale({0.5, 0.5});
    arrow_up_tex.setScale({0.5, 0.5});

    windowPtr = &window;
}

AdminScreen::~AdminScreen() {}

void AdminScreen::draw(RenderWindow& window) {
    window.clear(Color::Black);
    window.draw(sidebar);
    window.draw(admin_logo);
    window.draw(admin_logoText);

    auto mouse_pos = Vector2f(Mouse::getPosition(window));
    float currentY = 80.f;
    for (int i = 0; i < menuItems.size(); i++) {
        menuItems[i].setPosition({0.f, currentY});
        menuItems[i].setTextPosition(20.f, 15.f);
        menuItems[i].draw(window);

        if (expanded[i]) 
            arrow_up_tex.setPosition({150.f, currentY - 95.f});
        else 
            arrow_down_tex.setPosition({150.f, currentY - 95.f});
        window.draw(expanded[i] ?  arrow_up_tex : arrow_down_tex);

        float extraHeight = 0.f;
        if (expanded[i]) {
            for (int j = 0; j < subItems[i].size(); j++) {
                float subY = currentY + 60.f + j * 40.f;
                subItems[i][j].setPosition({0.f, subY});
                subItems[i][j].setTextPosition(40.f, 10.f);
                subItems[i][j].draw(window);
            }
            extraHeight += 40.f * subItems[i].size();
        }
        currentY += 60.f + extraHeight;
    }
}

void AdminScreen::update(Vector2f mousePos, bool mousePressed) {
    for (int i = 0; i < menuItems.size(); i++) 
        menuItems[i].update(mousePos);

    for (int i = 0; i < subItems.size(); i++) 
        if (expanded[i]) 
            for (int j = 0; j < subItems[i].size(); j++) 
                subItems[i][j].update(mousePos);

    static bool mouseReleased = true;
    if (mousePressed && mouseReleased) {
        for (int i = 0; i < menuItems.size(); i++) {
            if (menuItems[i].isClicked(mousePos, mousePressed)) {
                expanded[i] = !expanded[i];
                break;
            }
        }
        
        // Handle sub-item clicks
        for (int i = 0; i < subItems.size(); i++) {
            if (expanded[i]) {
                for (int j = 0; j < subItems[i].size(); j++) {
                    if (subItems[i][j].isClicked(mousePos, mousePressed)) {
                        // Select the clicked sub-item and deselect others
                        for (int ii = 0; ii < (int)subItems.size(); ++ii) {
                            for (int jj = 0; jj < (int)subItems[ii].size(); ++jj) {
                                subItems[ii][jj].setSelected(ii == i && jj == j);
                            }
                        }
                        break;
                    }
                }
            }
        }
        mouseReleased = false;
    }
    else if (!mousePressed) 
        mouseReleased = true;
}

