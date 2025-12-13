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
    int animDirection = 0;

public:
    void start(float, int direction = 0);
    void update(float, int&, int, int, float, function<void(int, float)>);
    bool isAnimating() const;
    float easeInOutCubic(float);
};
