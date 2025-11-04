#include "UI/components/PosterSlider.h"
#include "UI/components/Slide.h"
#include <fstream>
#include <cmath>
using namespace std;
using namespace sf;

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
    textures.clear();
    slides.clear();
    dots.clear();

    DLL<String> paths = provider.getPosterPaths();

    for (int i = 0; i < paths.getSize(); i++) {
        const String& path = paths[i];
        ifstream file(path.toAnsiString(), ios::binary);
        if (!file.is_open()) continue;

        string buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        Texture tex;
        if (!tex.loadFromMemory(buffer.data(), buffer.size())) 
            continue;

        textures.push_back(tex);
        slides.push_back(Slide(textures[textures.getSize() - 1], font));
    }

    float dotRadius = 7.f;
    float spacing = 25.f;
    int numDots = slides.getSize();
    float totalWidth = (numDots - 1) * spacing;
    float startX = (1700.f - totalWidth) / 2.f;
    float yPos = 900.f;

    for (int i = 0; i < numDots; ++i) {
        CircleShape dot(dotRadius);
        dot.setOutlineThickness(0);
        dot.setOutlineColor(Color::White);
        dot.setFillColor(Color(120, 120, 120));
        dot.setPosition({startX + i * spacing, yPos});
        dots.push_back(dot);
    }

    if (slides.getSize() > 0)
        slides[0].setPosition({268.f, 150.f});
}

void PosterSlider::update(float dt, RenderWindow& window) {
    if (slides.getSize() == 0) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    bool mousePressed = Mouse::isButtonPressed(Mouse::Button::Left);

    leftButton.update(mousePos, mousePressed);
    rightButton.update(mousePos, mousePressed);

    for (int i = 0; i < slides.getSize(); i++)
        slides[i].update(mousePos, mousePressed);

    int numSlides = slides.getSize();
    float slideWidth = 512.f;
    float gap = 168.f;
    float totalItemWidth = slideWidth + gap;
    float centerX = 850.f;
    float centerY = 150.f;
    float mainSlotX = centerX - slideWidth / 2.f;

    auto calculateCircularOffset = [&](int index1, int index2) {
        int delta = index1 - index2;
        if (delta > numSlides / 2) delta -= numSlides;
        if (delta < -numSlides / 2) delta += numSlides;
        return delta;
    };

    if (animating) {
        elapsed += dt;
        float t = std::min(elapsed / animTime, 1.f);
        float p = easeInOutCubic(t);

        int delta = calculateCircularOffset(targetIndex, currentIndex);
        float totalDisplacement = -delta * totalItemWidth;
        float currentDisplacement = totalDisplacement * p;

        for (int i = 0; i < numSlides; ++i) {
            float initialOffset = calculateCircularOffset(i, currentIndex) * totalItemWidth;
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
            slides[i].setPosition({268.f + offset, centerY});
        }
    }
}

void PosterSlider::draw(RenderWindow& window) {
    if (slides.getSize() == 0) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    View defaultView = window.getView();
    window.setView(sliderView);

    for (int i = 0; i < slides.getSize(); i++)
        slides[i].draw(window);

    window.setView(defaultView);

    leftButton.setFillColor(leftButton.isHovered() ? Color(0, 0, 0, 180) : Color(0, 0, 0, 60));
    leftButton.setTextColor(leftButton.isHovered() ? Color(255, 255, 255, 120) : Color(255, 255, 255, 180));
    rightButton.setFillColor(rightButton.isHovered() ? Color(0, 0, 0, 180) : Color(0, 0, 0, 60));
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
    if (animating || slides.getSize() == 0) return;
    if (!mousePressed) return;

    auto startAnimation = [this]() {
        animating = true;
        elapsed = 0.f;
        startX_anim = slides[currentIndex].getPosterSprite().getPosition().x;
    };

    if (leftButton.isClicked(mousePos, mousePressed)) {
        targetIndex = (currentIndex - 1 + slides.getSize()) % slides.getSize();
        startAnimation();
        return;
    }

    if (rightButton.isClicked(mousePos, mousePressed)) {
        targetIndex = (currentIndex + 1) % slides.getSize();
        startAnimation();
        return;
    }

    for (int i = 0; i < dots.getSize(); ++i) {
        if (dots[i].getGlobalBounds().contains(mousePos)) {
            if (i == currentIndex) return;
            targetIndex = i;
            clickedDot = true;  
            startAnimation();
            return;
        }
    }
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
