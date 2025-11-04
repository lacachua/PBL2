#pragma once
#include "IPosterProvider.h"
#include <SFML/System/String.hpp>
#include <fstream>
#include <sstream>

class PosterProviderFromTXT : public IPosterProvider {
private:
    string filePath;

public:
    PosterProviderFromTXT(const string& path) : filePath(path) {}
    DLL<String> getPosterPaths() override;
};
