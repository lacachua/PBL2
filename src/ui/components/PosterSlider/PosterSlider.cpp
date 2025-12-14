#include "UI/components/PosterSlider/PosterSlider.h"
#include "UI/components/PosterSlider/Slide.h"
#include <fstream>
#include <cmath>
using namespace std;
using namespace sf;

namespace {
float calcDotStepDurationSeconds(int remainingSteps) {
    // Arrow animation feels ~0.4s per slide in this project.
    // For dot jumps, keep the same animation style but increase speed when far away.
    // Speedup factor is capped to avoid becoming too fast/jerky.
    const float base = 0.4f;
    const int speedupCap = 4;
    int speedup = std::min(std::max(remainingSteps, 1), speedupCap);
    return base / static_cast<float>(speedup);
}

float calcDotTotalDurationSeconds(int steps) {
    steps = std::max(1, steps);
    return calcDotStepDurationSeconds(steps) * static_cast<float>(steps);
}
}

PosterSlider::PosterSlider(Font& f, RenderWindow& window)
    : font(f),
      leftButton(f, "<", 60.f, 100.f, 48),
      rightButton(f, ">", 60.f, 100.f, 48) 
{
    setClip(268.f, 100.f, 1192.f, 900.f, window);

    leftButton.setFillColor(Color(0, 0, 0, 60));
    leftButton.setTextColor(Color(255, 255, 255, 180));
    leftButton.setOutlineThickness(0.f);
    leftButton.setPosition({268.f, (float)window.getSize().y / 2.f - 50.f});
    
    rightButton.setFillColor(Color(0, 0, 0, 60));
    rightButton.setTextColor(Color(255, 255, 255, 180));
    rightButton.setOutlineThickness(0.f);
    rightButton.setPosition({1400.f, (float)window.getSize().y / 2.f - 50.f});
}

void PosterSlider::loadPosters(IPosterProvider& provider, const Font& font) {
    textures.clear(); slides.clear(); dots.clear();
    DLL<String> paths = provider.getPosterPaths();

    for (int i = 0; i < paths.getSize(); i++) {
        Texture tex = loader.loadTexture(paths[i]);
        if (tex.getSize().x == 0) continue;

        textures.push_back(move(tex));
        slides.push_back(Slide(textures[textures.getSize() - 1], font));
    }

    int numDots = slides.getSize();
    float startX = (1700.f - (numDots - 1) * 25.f) / 2.f;

    for (int i = 0; i < numDots; ++i) {
        CircleShape dot(7.f);
        dot.setOutlineThickness(0);
        dot.setOutlineColor(Color::White);
        dot.setFillColor(Color(120, 120, 120));
        dot.setPosition({startX + i * 25.f, 900.f});
        dots.push_back(dot);
    }

    if (slides.getSize() > 0)
        slides[0].setPosition({268.f, 150.f});
}

void PosterSlider::update(float dt, RenderWindow& window) {
    if (slides.getSize() == 0) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    bool mousePressed = Mouse::isButtonPressed(Mouse::Button::Left);

    leftButton.update(mousePos, mousePressed, Color(0, 0, 0, 180), Color(0, 0, 0, 60));
    rightButton.update(mousePos, mousePressed, Color(0, 0, 0, 180), Color(0, 0, 0, 60));

    float slideSpacing = 680.f;

    if (animator.isAnimating()) {
        animator.update(dt, currentIndex, targetIndex, slides.getSize(), slideSpacing, 
            [&](int i, float x) { slides[i].setPosition({x, 150.f}); });
    } 
    else {
        for (int i = 0; i < slides.getSize(); i++) {
            float offset = calcCircularOffset(i, currentIndex, slides.getSize()) * slideSpacing;
            slides[i].setPosition({268.f + offset, 150.f});
        }
    }

    for (int i = 0; i < slides.getSize(); i++)
        slides[i].update(mousePos, mousePressed);
}

void PosterSlider::draw(RenderWindow& window) {
    if (slides.getSize() == 0) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    View defaultView = window.getView();
    window.setView(sliderView);

    for (int i = 0; i < slides.getSize(); i++)
        slides[i].draw(window);

    window.setView(defaultView);

    leftButton.setTextColor(leftButton.isHovered() ? Color(255, 255, 255, 120) : Color(255, 255, 255, 180));
    rightButton.setTextColor(rightButton.isHovered() ? Color(255, 255, 255, 120) : Color(255, 255, 255, 180));

    leftButton.draw(window);
    rightButton.draw(window);

    for (int i = 0; i < dots.getSize(); ++i) {
        bool isHovered = dots[i].getGlobalBounds().contains(mousePos);
        bool isActive = (i == currentIndex);
        dots[i].setFillColor(isActive ? Color::White : isHovered ? Color(135, 139, 208, 255) : Color(120, 120, 120));
        dots[i].setOutlineColor(Color::White);
        dots[i].setOutlineThickness(isHovered ? 2.f : 0.f);
        window.draw(dots[i]);   
    }
}

void PosterSlider::handleEvent(const Vector2f& mousePos, bool mousePressed, AppState& state) {
    bool justClicked = mousePressed && !wasMousePressed;
    wasMousePressed = mousePressed;
    
    if (animator.isAnimating() || slides.getSize() == 0 || !justClicked) return;

    if (leftButton.isClicked(mousePos, true)) {
        dotNavActive = false;
        targetIndex = (currentIndex - 1 + slides.getSize()) % slides.getSize();
        animator.start(slides[currentIndex].getPosterSprite().getPosition().x, -1);  // Force left direction
    }
    else if (rightButton.isClicked(mousePos, true)) {
        dotNavActive = false;
        targetIndex = (currentIndex + 1) % slides.getSize();
        animator.start(slides[currentIndex].getPosterSprite().getPosition().x, 1);  // Force right direction
    }
    else {
        for (int i = 0; i < dots.getSize(); i++) {
            if (dots[i].getGlobalBounds().contains(mousePos)) {
                if (i == currentIndex) return;

                // Dot navigation: one continuous multi-slide animation (no pauses), forced direction.
                dotNavActive = false;
                dotNavFinalTarget = i;
                dotNavDirection = (dotNavFinalTarget > currentIndex) ? 1 : -1;

                targetIndex = dotNavFinalTarget;
                int steps = std::abs(dotNavFinalTarget - currentIndex);
                float totalDuration = calcDotTotalDurationSeconds(steps);
                animator.start(slides[currentIndex].getPosterSprite().getPosition().x, dotNavDirection, totalDuration, steps);
                return;
            }
        }
        
        for (int i = 0 ; i < slides.getSize(); i++) {
            if (slides[i].isDetailButtonClicked(mousePos, true)) {
                currentIndex = i;
                if (onDetailRequested) onDetailRequested(i);
                state = AppState::MOVIE_DETAILS;
                return;
            }
        }
    }
}

int PosterSlider::calcCircularOffset(int i1, int i2, int total) {
    int d = i1 - i2;
    if (d > total / 2) d -= total;
    if (d < -total / 2) d += total;
    return d;
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
        {clipPx.position.x / static_cast<float>(ws.x), clipPx.position.y / static_cast<float>(ws.y)},
        {clipPx.size.x / static_cast<float>(ws.x), clipPx.size.y / static_cast<float>(ws.y)}
    ));
}

void PosterSlider::onResize(const RenderWindow& window) {
    updateViewport(window);
}

void PosterSlider::setOnDetailRequested(function<void(int)> callback) { 
    onDetailRequested = callback; 
}

int PosterSlider::getCurrentIndex() const { 
    return currentIndex; 
}