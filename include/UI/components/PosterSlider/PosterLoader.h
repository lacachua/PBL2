#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class PosterLoader {
public:
    Texture loadTexture(const String&);
};
