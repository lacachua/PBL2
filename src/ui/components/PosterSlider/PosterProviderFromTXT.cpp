#include "UI/components/PosterSlider/PosterProviderFromTXT.h"
#include <sstream>
#include <filesystem>

using namespace std;
using namespace sf;

PosterProviderFromTXT::PosterProviderFromTXT(const string& path)
    : filePath(path) {}

DLL<String> PosterProviderFromTXT::getPosterPaths() {
    DLL<String> paths;
    ifstream file(filePath);
    if (!file.is_open()) return paths;

    string line;
    getline(file, line);

    // Determine poster_path column index from header for schema flexibility.
    int posterColIndex = -1;
    if (!line.empty()) {
        stringstream hs(line);
        string h;
        int idx = 0;
        while (getline(hs, h, '|')) {
            if (h == "poster_path") {
                posterColIndex = idx;
                break;
            }
            idx++;
        }
    }
    if (posterColIndex < 0) posterColIndex = 11; // legacy fallback

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        int col = 0;
        string posterPath;

        while (getline(ss, token, '|')) {
            if (col == posterColIndex) {
                posterPath = token;
                break;
            }
            col++;
        }

        if (!posterPath.empty()) {
            while (!posterPath.empty() && isspace((unsigned char)posterPath.back()))
                posterPath.pop_back();
            while (!posterPath.empty() && isspace((unsigned char)posterPath.front()))
                posterPath.erase(posterPath.begin());

            if (filesystem::exists(posterPath))
                paths.push_back(String(posterPath));
        }
    }

    file.close();
    return paths;
}
