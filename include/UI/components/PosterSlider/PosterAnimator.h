#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
using namespace sf;
using namespace std;

class PosterAnimator {
private:
    bool animating = false;
    float elapsed = 0.f;
    static constexpr float defaultAnimTime = 0.4f;
    float animTime = defaultAnimTime;
    float startX = 0.f;
    int animDirection = 0;
    int forcedSteps = 1;

public:
    void start(float startXPos, int direction = 0, float durationSeconds = -1.f, int steps = 1);
    void update(float, int&, int, int, float, function<void(int, float)>);
    bool isAnimating() const;
    float easeInOutCubic(float);
};
