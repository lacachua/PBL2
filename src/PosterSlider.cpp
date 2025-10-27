#include "PosterSlider.h"
#include "Movie.h"
#include <cmath>

PosterSlider::PosterSlider(Font& font, RenderWindow& window) : 
    font(font),
    leftArrow(font, "<", 48),
    rightArrow(font, ">", 48)
{
    rightButton.setSize({60.f, 100.f});
    rightButton.setFillColor(Color(0, 0, 0, 60));
    rightButton.setPosition({1400, (float)window.getSize().y / 2.f - 50.f});

    leftButton.setSize({60.f, 100.f});
    leftButton.setFillColor(Color(0, 0, 0, 60));
    leftButton.setPosition({268, (float)window.getSize().y / 2.f - 50.f});

    leftArrow.setFillColor(Color(255, 255, 255, 180));
    rightArrow.setFillColor(Color(255, 255, 255, 180));
    leftArrow.setPosition(leftButton.getPosition() + Vector2f(20.f, 15.f));
    rightArrow.setPosition(rightButton.getPosition() + Vector2f(20.f, 15.f));

    setClip(268.f, 100.f, 1192.f, 900.f, window);
}

void PosterSlider::loadPosters(const vector<string>& paths, const Font& font) {
    textures.clear();
    slides.clear();

    for (auto& p : paths) {
        Texture tex(p);
        textures.push_back(move(tex));
    }

    for (auto& tex : textures) {
        Slide slide(tex, font);
        slides.push_back(move(slide));
    }

    float dotRadius = 7.f;
    float spacing = 25.f;
    float totalWidth = (textures.size() - 1) * spacing;
    float startX = (1700 - totalWidth) / 2.f + 7;
    float yPos = 972 - 45.f;

    dots.clear();
    for (size_t i = 0; i < textures.size(); ++i) {
        CircleShape dot(dotRadius);
        dot.setOutlineThickness(0);
        dot.setOutlineColor(Color::White);
        dot.setFillColor(Color(120, 120, 120));
        dot.setPosition({ startX + i * spacing, yPos });
        dots.push_back(dot);
    }
}

void PosterSlider::update(float dt, RenderWindow& window) {
    if (slides.empty()) return;

    float slideWidth = slides[0].getPosterSprite().getGlobalBounds().size.x;
    float gap = 40.f;
    float totalItemWidth = slideWidth + gap;
    float centerY = (window.getSize().y - slides[0].getPosterSprite().getGlobalBounds().size.y) / 2.f;
    float centerX = window.getSize().x / 2.f;
    
    float mainSlotX = centerX - slideWidth - (gap / 2.f);
    int numSlides = slides.size();

    auto calculateCircularOffset = [&](int index1, int index2) {
        int delta = index1 - index2;
        if (delta > numSlides / 2) delta -= numSlides;
        if (delta < -numSlides / 2) delta += numSlides;
        return delta;
    };

    if (animating) {
        elapsed += dt;
        float t = min(elapsed / animTime, 1.f);
        float p = easeInOutCubic(t);

        int delta = targetIndex - currentIndex;
        if (!clickedDot) {
            delta = calculateCircularOffset(targetIndex, currentIndex);
        }

        // Tổng quãng đường cần di chuyển
        float totalDisplacement = -delta * totalItemWidth;
        // Quãng đường đã di chuyển được tại thời điểm t
        float currentDisplacement = totalDisplacement * p;

        for (int i = 0; i < numSlides; ++i) {
            // Vị trí tương đối của mỗi slide so với slide chính
            float initialOffset = calculateCircularOffset(i, currentIndex) * totalItemWidth;
            // Vị trí mới = Vị trí gốc lúc bắt đầu anim + Vị trí tương đối + Quãng đường đã đi
            slides[i].setPosition({startX_anim + initialOffset + currentDisplacement, centerY});
        }

        if (t >= 1.f) {
            animating = false;
            currentIndex = targetIndex;
            clickedDot = false;
        }
    } 
    else {
        for (int i = 0; i < numSlides; i++) {
            float offset = calculateCircularOffset(i, currentIndex) * totalItemWidth;
            slides[i].setPosition({mainSlotX + offset, centerY});
        }
    }
}

void PosterSlider::handleEvent(const Vector2f& mousePos, bool mousePressed, AppState& state) {
    if (animating) return;

    for (size_t i = 0; i < slides.size(); ++i) {
        if (slides[i].isButtonClicked(mousePos, mousePressed)) {
            state = AppState::MOVIE_DETAILS;
            selectedIndex = i;
            return;
        }
    }

    if (!mousePressed) return;

    auto startAnimation = [this]() {
        animating = true;
        elapsed = 0.f;
        startX_anim = slides[currentIndex].getPosterSprite().getPosition().x;
    };

    if (leftArrow.getGlobalBounds().contains(mousePos) || leftButton.getGlobalBounds().contains(mousePos)) {
        targetIndex = (currentIndex - 1 + slides.size()) % slides.size();
        startAnimation();
        return;
    }

    if (rightArrow.getGlobalBounds().contains(mousePos) || rightButton.getGlobalBounds().contains(mousePos)) {
        targetIndex = (currentIndex + 1) % slides.size();
        startAnimation();
        return;
    }

    for (size_t i = 0; i < dots.size(); ++i) {
        if (dots[i].getGlobalBounds().contains(mousePos)) {
            if (i == currentIndex) return;
            targetIndex = i;
            clickedDot = true;
            startAnimation();
            return;
        }
    }
}

void PosterSlider::draw(RenderWindow& window) {
    if (slides.empty()) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    bool hoverLeft = leftButton.getGlobalBounds().contains(mousePos);
    bool hoverRight = rightButton.getGlobalBounds().contains(mousePos);

    slides[currentIndex].highlightButton(slides[currentIndex].isButtonHovered(mousePos));
    int nextIndex = (currentIndex + 1) % slides.size();
    slides[nextIndex].highlightButton(slides[nextIndex].isButtonHovered(mousePos));

    View defaultView = window.getView();

    window.setView(sliderView);
    slides[currentIndex].draw(window);
    slides[nextIndex].draw(window);
    window.setView(defaultView);

    Color idleColor(0, 0, 0, 60);   
    Color hoverColor(0, 0, 0, 180); 
    Color iconIdle(255, 255, 255, 180);
    Color iconHover(255, 255, 255, 255);

    leftButton.setFillColor(hoverLeft ? hoverColor : idleColor);
    rightButton.setFillColor(hoverRight ? hoverColor : idleColor);
    leftArrow.setFillColor(hoverLeft ? iconHover : iconIdle);
    rightArrow.setFillColor(hoverRight ? iconHover : iconIdle);

    window.draw(leftButton);
    window.draw(leftArrow);
    window.draw(rightButton);
    window.draw(rightArrow);

    for (size_t i = 0; i < dots.size(); ++i) {
        bool isHovered = dots[i].getGlobalBounds().contains(mousePos);
        bool isActive = (i == static_cast<size_t>(currentIndex));
        dots[i].setFillColor(isActive ? Color::White : isHovered ? Color(135, 139, 208, 255) : Color(120, 120, 120));
        dots[i].setOutlineColor(Color::White);
        dots[i].setOutlineThickness(isHovered ? 2 : 0);
        window.draw(dots[i]);
    }
}

float PosterSlider::easeInOutCubic(float x) const {
    return (x < 0.5f)
        ? 5.f * x * x * x
        : 1.f - pow(-2.f * x + 2.f, 3.f) / 2.2f;
}

Sprite& PosterSlider::getSlidePosterSprite(int idx){
    return slides[idx].getPosterSprite();
}

void PosterSlider::setClip(float x, float y, float w, float h, const RenderWindow& window) {
    clipPx = FloatRect({x, y}, {w, h});
    sliderView.setCenter(Vector2f(x + w / 2.f, y + h / 2.f));
    sliderView.setSize(Vector2f(w, h));
    updateViewport(window);
}


void PosterSlider::updateViewport(const RenderWindow& window) {
    const auto ws = window.getSize();
    sliderView.setViewport(FloatRect(
        {clipPx.position.x   / static_cast<float>(ws.x),
        clipPx.position.y    / static_cast<float>(ws.y)},
        {clipPx.size.x  / static_cast<float>(ws.x),
        clipPx.size.y / static_cast<float>(ws.y)}
    ));
}

void PosterSlider::onResize(const RenderWindow& window) {
    updateViewport(window);
}