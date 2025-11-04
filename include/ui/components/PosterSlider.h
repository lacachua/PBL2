#pragma once
#include <SFML/Graphics.hpp>
#include "Slide.h"
#include "core/AppState.h"
#include "data-structures/DLL.h"
#include "UI/components/Button.h"
#include "models/IPosterProvider.h"
#include <cmath>
using namespace sf;

class PosterSlider {
private:
    DLL<Texture> textures;
    DLL<Slide> slides;
    DLL<CircleShape> dots;
    Button leftButton, rightButton;
    Font& font;
    View sliderView;
    FloatRect clipPx;

    int currentIndex = 0;
    int targetIndex = 0;
    bool animating = false;
    bool clickedDot = false;
    float animTime = 0.35f;
    float elapsed = 0.f;
    float startX_anim = 0.f;

    static constexpr float easeInOutCubic(float x) noexcept {
        return x < 0.5f ? 4.f * x * x * x : 1.f - powf(-2.f * x + 2.f, 3.f) / 2.f;
    }
    void updateViewport(const RenderWindow&);
public:
    PosterSlider(Font&, RenderWindow&);
    void loadPosters(IPosterProvider&, const Font&);
    void update(float, RenderWindow&);
    void draw(RenderWindow&);
    void handleEvent(const Vector2f&, bool, AppState&);
    void setClip(float, float, float, float, const RenderWindow&);
    void onResize(const RenderWindow&);
};

