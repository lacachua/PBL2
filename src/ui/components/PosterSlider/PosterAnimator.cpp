#include "UI/components/PosterSlider/PosterAnimator.h"
#include <cmath>
#include <algorithm>

void PosterAnimator::start(float startXPos, int direction, float durationSeconds, int steps) {
    animating = true;
    elapsed = 0.f;
    startX = startXPos;
    animDirection = direction;
    forcedSteps = std::max(1, steps);

    if (durationSeconds > 0.f) {
        animTime = durationSeconds;
    } else {
        animTime = defaultAnimTime;
    }
}

bool PosterAnimator::isAnimating() const {
    return animating;
}

float PosterAnimator::easeInOutCubic(float t) {
    return t < 0.5f ? 4.f * t * t * t : 1.f - pow(-2.f * t + 2.f, 3.f) / 2.f;
}

void PosterAnimator::update(float dt, int& currentIndex, int targetIndex, int numSlides, float totalItemWidth, function<void(int, float)> positionCallback) {
    if (!animating) return;

    elapsed += dt;
    float t = min(elapsed / animTime, 1.f);
    float p = easeInOutCubic(t);

    auto circularOffset = [&](int index, int center) {
        int d = index - center;
        if (d > numSlides / 2) d -= numSlides;
        if (d < -numSlides / 2) d += numSlides;
        return d;
    };

    // Forced multi-step travel (used by dot jumps): move continuously without pausing per slide.
    if (animDirection != 0 && forcedSteps > 1) {
        float virtualCenter = static_cast<float>(currentIndex) + static_cast<float>(animDirection) * (static_cast<float>(forcedSteps) * p);
        float half = static_cast<float>(numSlides) / 2.f;

        for (int i = 0; i < numSlides; ++i) {
            float d = static_cast<float>(i) - virtualCenter;
            if (d > half) d -= static_cast<float>(numSlides);
            if (d < -half) d += static_cast<float>(numSlides);
            positionCallback(i, startX + d * totalItemWidth);
        }

        if (t >= 1.f) {
            animating = false;
            currentIndex = targetIndex;
        }
        return;
    }

    // Delta = how many steps the carousel moves.
    // - Arrow buttons are always a single-step move.
    // - Dot clicks take the shortest path.
    int delta = 0;
    if (animDirection != 0) {
        delta = animDirection; // forced single-step
    } else {
        delta = circularOffset(targetIndex, currentIndex);
    }

    float totalDisplacement = -static_cast<float>(delta) * totalItemWidth;
    float currentDisplacement = totalDisplacement * p;

    for (int i = 0; i < numSlides; ++i) {
        int initialOffset = circularOffset(i, currentIndex);
        positionCallback(i, startX + initialOffset * totalItemWidth + currentDisplacement);
    }

    if (t >= 1.f) {
        animating = false;
        currentIndex = targetIndex;
    }
}
