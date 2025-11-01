#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "AppState.h"
#include "Slide.h"

using namespace std;
using namespace sf;

class PosterSlider {
private:
    vector<Texture> textures;
    vector<Slide> slides;
    vector<CircleShape> dots;

    Text leftArrow, rightArrow;
    Font font;
    RectangleShape rightButton, leftButton;

    int currentIndex = 0;
    int targetIndex = 0;
    bool animating = false;
    bool clickedDot = false;
    float animTime = 0.35;
    float elapsed = 0;
    float startX_anim = 0;
    float easeInOutCubic(float x) const;
    View sliderView;
    FloatRect clipPx;
    void updateViewport(const RenderWindow&);
public:
    PosterSlider(Font&, RenderWindow&);
    void loadPosters(const vector<string>&, const Font&);
    void update(float, RenderWindow&);
    void draw(RenderWindow&);
    void handleEvent(const Vector2f&, bool, AppState&);
    Sprite& getSlidePosterSprite(int);
    void setClip(float, float, float, float, const RenderWindow&);
    void onResize(const RenderWindow&);

    int selectedIndex = -1;
    const Slide& getSelectedSlide() const { return slides[selectedIndex]; }
    int getSelectedIndex() const { return selectedIndex; }
};
