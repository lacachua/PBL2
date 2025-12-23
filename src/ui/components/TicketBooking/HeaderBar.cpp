// #include "UI/components/TicketBooking/HeaderBar.h"

// HeaderBar::HeaderBar(Font& f) 
//     :   headerFont("../assets/fonts/BEBAS_NEUE_ZSMALL.ttf"),
//         mainView({940.f, 600.f}), summaryBox({400.f, 300.f}),
//         back_button(f, L"←", 100.f, 50.f, 30),
//         continue_button(f, L"Tiếp tục", 280.f, 50.f, 25)
// {
//     String titles[5] = {
//         L"SUẤT CHIẾU",
//         L"CHỌN GHẾ", 
//         L"BẮP NƯỚC",
//         L"THANH TOÁN",
//         L"XÁC NHẬN"
//     };
    
//     float startX = 174.f;
//     float startY = 140.f;
//     float boxWidth = 180.f;
//     float boxHeight = 50.f;
//     float spacing = 120.f;
    
//     for (int i = 0; i < 5; i++) {
//         RectangleShape box(Vector2f(boxWidth, boxHeight));
//         box.setPosition(Vector2f(startX + i * (boxWidth + spacing), startY));
//         box.setFillColor(i == 0 ? Color(20, 118, 172) : Color(80, 80, 90));
//         box.setOutlineThickness(1.f);
//         box.setOutlineColor(Color::White);
//         boxes.push_back(box);
        
//         Text text(headerFont);
//         text.setString(titles[i]);
//         text.setCharacterSize(36);
//         text.setFillColor(Color::White);
        
//         FloatRect textBounds = text.getLocalBounds();
//         float textX = box.getPosition().x + (boxWidth - textBounds.size.x) / 2.f;
//         float textY = box.getPosition().y + (boxHeight - textBounds.size.y) / 2.f - 7.f;
//         text.setPosition(Vector2f(textX, textY));
        
//         texts.push_back(text);
//     }

//     mainView.setPosition({174.f, 220.f});
//     mainView.setOutlineThickness(1.f);
//     mainView.setOutlineColor(Color::White);
//     mainView.setFillColor(Color(1, 3, 33));

//     summaryBox.setPosition({1154.f, 220.f});
//     summaryBox.setOutlineThickness(1.f);
//     summaryBox.setOutlineColor(Color::White);
//     summaryBox.setFillColor(Color(1, 3, 33));

//     back_button.setPosition({1154.f, 540.f});
//     back_button.setOutlineThickness(1.f);
//     back_button.setOutlineColor(Color::White);
//     back_button.setFillColor(Color(1, 3, 33));

//     continue_button.setPosition({1274.f, 540.f});
//     continue_button.setOutlineThickness(1.f);
//     continue_button.setOutlineColor(Color::White);
//     continue_button.setFillColor(Color(1, 3, 33));
// }

// void HeaderBar::update(Vector2f mousePos, bool mousePressed, BookingState currentState) {
//     int currentIndex = static_cast<int>(currentState);
    
//     for (int i = 0; i < boxes.getSize(); i++) {
//         if (i == currentIndex) 
//             boxes[i].setFillColor(Color(20, 118, 172));
//         else if (i < currentIndex) 
//             boxes[i].setFillColor(Color(50, 150, 50));
//         else 
//             boxes[i].setFillColor(Color(80, 80, 90));
//     }
//     back_button.update(mousePos, mousePressed);
//     continue_button.update(mousePos, mousePressed);
//     if (continue_button.isHovered())
//         continue_button.setFillColor(Color(71, 246, 74));
//     if (back_button.isHovered()) 
//         back_button.setFillColor(Color::Red);
// }

// void HeaderBar::draw(RenderWindow& window) {
//     for (int i = 0; i < boxes.getSize(); i++) {
//         window.draw(boxes[i]);
//         window.draw(texts[i]);
        
//         if (i < boxes.getSize() - 1) {
//             float arrowX = boxes[i].getPosition().x + boxes[i].getSize().x + 48.f;
//             float arrowY = boxes[i].getPosition().y + 25.f;
            
//             ConvexShape arrow(3);
//             arrow.setPoint(0, Vector2f(arrowX, arrowY - 10.f));
//             arrow.setPoint(1, Vector2f(arrowX + 30.f, arrowY));
//             arrow.setPoint(2, Vector2f(arrowX, arrowY + 10.f));
//             arrow.setFillColor(Color::White);
//             window.draw(arrow);
//         }
//     }
//     window.draw(mainView);
//     window.draw(summaryBox);
//     back_button.draw(window);
//     continue_button.draw(window);
// }

#include "UI/components/TicketBooking/HeaderBar.h"
#include <vector>
using namespace std;

HeaderBar::HeaderBar(Font& f) 
    :   headerFont("../assets/fonts/BEBAS_NEUE_ZSMALL.ttf"),
        mainView({940.f, 700.f}), summaryBox({400.f, 300.f}),
        back_button(f, L"←", 100.f, 50.f, 30),
        continue_button(f, L"Tiếp tục", 280.f, 50.f, 25)
{

}

void HeaderBar::draw(RenderWindow& window, BookingState currentState) {
    float startX = 174.f;
    float startY = 150.f;
    float stepWidth = 180.f;
    float stepHeight = 50.f;
    float spacing = 120.f;

    String titles[5] = {
        L"SUẤT CHIẾU",
        L"CHỌN GHẾ", 
        L"BẮP NƯỚC",
        L"THANH TOÁN",
        L"XÁC NHẬN"
    };

    mainView.setPosition({174.f, 220.f});
    mainView.setOutlineThickness(1.f);
    mainView.setOutlineColor(Color::White);
    mainView.setFillColor(Color(1, 3, 33));
    window.draw(mainView);

    summaryBox.setPosition({1154.f, 220.f});
    summaryBox.setOutlineThickness(1.f);
    summaryBox.setOutlineColor(Color::White);
    summaryBox.setFillColor(Color(1, 3, 33));
    window.draw(summaryBox);

    back_button.setPosition({1154.f, 540.f});
    back_button.setOutlineThickness(1.f);
    back_button.setOutlineColor(Color::White);
    back_button.draw(window);

    continue_button.setPosition({1274.f, 540.f});
    continue_button.setOutlineThickness(1.f);
    continue_button.setOutlineColor(Color::White);
    continue_button.draw(window);

    int currentStateIndex = static_cast<int>(currentState);
    
    for (int i = 0; i < 5; i++) {
        RectangleShape box({stepWidth, stepHeight});
        box.setPosition({startX + i * (stepWidth + spacing), startY});
        
        if (i == currentStateIndex)
            box.setFillColor(Color(20, 118, 172));
        else
            box.setFillColor(Color(80, 80, 90));
        
        box.setOutlineThickness(1.f);
        box.setOutlineColor(Color::White);

        Text text(headerFont, titles[i], 36);
        text.setFillColor(Color::White);
        FloatRect textBounds = text.getLocalBounds();
        float textX = box.getPosition().x + (stepWidth - textBounds.size.x) / 2.f;
        float textY = box.getPosition().y + (stepHeight - textBounds.size.y) / 2.f - 7.f;
        text.setPosition({textX, textY});

        window.draw(box);
        window.draw(text);
        if (i < 4) {
            float arrowX = box.getPosition().x + box.getSize().x + 48.f;
            float arrowY = box.getPosition().y + 25.f;
            
            ConvexShape arrow(3);
            arrow.setPoint(0, Vector2f(arrowX, arrowY - 10.f));
            arrow.setPoint(1, Vector2f(arrowX + 30.f, arrowY));
            arrow.setPoint(2, Vector2f(arrowX, arrowY + 10.f));
            arrow.setFillColor(Color::White);
            window.draw(arrow);
        }
    }
}

void HeaderBar::update(Vector2f mousePos, bool mousePressed, BookingState& currentState) {
    (void)currentState;
    back_button.update(mousePos, mousePressed, Color::Red, Color::White);
    continue_button.update(mousePos, mousePressed, Color(71, 246, 74), Color::White);
}

bool HeaderBar::isBackButtonClicked(Vector2f mousePos, bool mousePressed) {
    static bool wasPressed = false;
    bool justClicked = mousePressed && !wasPressed;
    wasPressed = mousePressed;

    if (!justClicked) return false;
    return back_button.getGlobalBounds().contains(mousePos);
}

bool HeaderBar::isContinueButtonClicked(Vector2f mousePos, bool mousePressed) {
    static bool wasPressed2 = false;
    bool justClicked = mousePressed && !wasPressed2;
    wasPressed2 = mousePressed;

    if (!justClicked) return false;
    return continue_button.getGlobalBounds().contains(mousePos);
}

void HeaderBar::handleNavigation(Vector2f mousePos, bool mousePressed, BookingState& currentState, bool canProceed) {
    if (isBackButtonClicked(mousePos, mousePressed)) {
        int current = static_cast<int>(currentState);
        if (current > 0)
            currentState = static_cast<BookingState>(current - 1);
        return;
    }
    
    if (isContinueButtonClicked(mousePos, mousePressed) && canProceed) {
        int current = static_cast<int>(currentState);
        if (current < 4)
            currentState = static_cast<BookingState>(current + 1);
        return;
    }
}
