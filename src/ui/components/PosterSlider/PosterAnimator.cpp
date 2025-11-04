#include "UI/components/PosterSlider/PosterAnimator.h"
#include <cmath>
#include <algorithm>

void PosterAnimator::start(float startXPos) {
    animating = true;
    elapsed = 0.f;
    startX = startXPos;
}

float PosterAnimator::easeInOutCubic(float t) {
    return t < 0.5f ? 4.f * t * t * t : 1.f - pow(-2.f * t + 2.f, 3.f) / 2.f;
}

void PosterAnimator::update(float dt, int& currentIndex, int targetIndex, int numSlides, float totalItemWidth, function<void(int, float)> positionCallback) {
    if (!animating) return;

    elapsed += dt;
    float t = min(elapsed / animTime, 1.f);
    float p = easeInOutCubic(t);

    auto calcOffset = [&](int index1, int index2) {
        int delta = index1 - index2;
        if (delta > numSlides / 2) delta -= numSlides;
        if (delta < -numSlides / 2) delta += numSlides;
        return delta;
    };

    int delta = calcOffset(targetIndex, currentIndex);
    float totalDisplacement = -delta * totalItemWidth;
    float currentDisplacement = totalDisplacement * p;

    for (int i = 0; i < numSlides; ++i) {
        float initialOffset = calcOffset(i, currentIndex) * totalItemWidth;
        positionCallback(i, startX + initialOffset + currentDisplacement);
    }

    if (t >= 1.f) {
        animating = false;
        currentIndex = targetIndex;
    }
}
