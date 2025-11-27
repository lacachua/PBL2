#pragma once
#include "IPosterProvider.h"
#include <SFML/System/String.hpp>
#include <fstream>
#include <sstream>
#include <string>

class PosterProviderFromTXT : public IPosterProvider {
private:
    std::string filePath;

public:
    PosterProviderFromTXT(const std::string& path) : filePath(path) {}
    DLL<String> getPosterPaths() override;
};
