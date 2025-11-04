#pragma once
#include <SFML/Graphics.hpp>
#include "Slide.h"
#include "core/AppState.h"
#include "data-structures/DLL.h"
#include "UI/components/Button.h"
#include "IPosterProvider.h"
#include "PosterLoader.h"
#include "PosterAnimator.h"

class PosterSlider {
private:
    Font& font;
    DLL<Texture> textures;
    DLL<Slide> slides;
    DLL<CircleShape> dots;
    Button leftButton, rightButton;
    
    View sliderView;
    FloatRect clipPx;

    int currentIndex = 0;
    int targetIndex = 0;
    bool clickedDot = false;

    PosterAnimator animator;
    PosterLoader loader;

    function<void(int)> onDetailRequested;

    int calcCircularOffset(int, int, int);
    void updateViewport(const RenderWindow&);
public:
    PosterSlider(Font&, RenderWindow&);

    void loadPosters(IPosterProvider&, const Font&);
    void update(float, RenderWindow&);
    void draw(RenderWindow&);
    void handleEvent(const Vector2f&, bool, AppState&);

    void setClip(float, float, float, float, const RenderWindow&);
    void onResize(const RenderWindow&);

    void setOnDetailRequested(function<void(int)>);
    int getCurrentIndex() const;
};

