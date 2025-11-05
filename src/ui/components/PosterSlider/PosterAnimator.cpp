#include "UI/components/PosterSlider/PosterAnimator.h"
#include <cmath>
#include <algorithm>

void PosterAnimator::start(float startXPos, int direction) {
    animating = true;
    elapsed = 0.f;
    startX = startXPos;
    animDirection = direction;
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
        if (abs(delta) > numSlides / 2) {
            if (index1 < index2) 
                delta -= numSlides;
            else 
                delta += numSlides;
        }
        return delta;
    };

    // Calculate delta based on animation direction
    int delta;
    if (animDirection == 0) {
        // Use shortest path (for dot clicks)
        delta = calcOffset(targetIndex, currentIndex);
    } else {
        // Use forced direction (for arrow buttons)
        delta = targetIndex - currentIndex;
        
        // Wrap around if needed, but respect the direction
        if (animDirection > 0) {  // Right arrow
            if (delta < 0) delta += numSlides;
        } else {  // Left arrow
            if (delta > 0) delta -= numSlides;
        }
    }

    float totalDisplacement = -delta * totalItemWidth;
    float currentDisplacement = totalDisplacement * p;

    for (int i = 0; i < numSlides; ++i) {
        // Use the same offset calculation method as delta to maintain consistency
        int initialOffset;
        if (animDirection == 0) {
            // For dot clicks (shortest path), use circular offset
            initialOffset = calcOffset(i, currentIndex);
        } else {
            // For arrow buttons (forced direction), use linear offset respecting wrap-around
            initialOffset = i - currentIndex;
            
            // Adjust offset based on animation direction to maintain continuity
            if (animDirection > 0) {  // Going right
                if (initialOffset < 0) initialOffset += numSlides;
            } else {  // Going left
                if (initialOffset > 0) initialOffset -= numSlides;
            }
        }
        
        positionCallback(i, startX + initialOffset * totalItemWidth + currentDisplacement);
    }

    if (t >= 1.f) {
        animating = false;
        currentIndex = targetIndex;
    }
}
