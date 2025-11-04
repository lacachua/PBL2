#include "UI/components/PosterSlider/PosterLoader.h"
#include <fstream>

Texture PosterLoader::loadTexture(const String& path) {
    ifstream file(path.toAnsiString(), ios::binary);
    if (!file.is_open()) return {};

    string buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Texture tex;
    if (!tex.loadFromMemory(buffer.data(), buffer.size()))
        return {};
    return tex;
}
