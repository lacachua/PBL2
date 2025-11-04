#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
using namespace sf;
using namespace std;

class PosterAnimator {
private:
    bool animating = false;
    float elapsed = 0.f;
    float animTime = 0.4f;
    float startX = 0.f;

public:
    void start(float startXPos);
    void update(float dt, int& currentIndex, int targetIndex, int numSlides, float totalItemWidth, function<void(int, float)> positionCallback);
    bool isAnimating() const { return animating; }
    float easeInOutCubic(float t);
};
