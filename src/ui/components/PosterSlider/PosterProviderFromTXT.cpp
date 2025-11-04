#include "UI/components/PosterSlider/PosterProviderFromTXT.h"
#include <sstream>
#include <filesystem>

using namespace std;
using namespace sf;

DLL<String> PosterProviderFromTXT::getPosterPaths() {
    DLL<String> paths;
    ifstream file(filePath);
    if (!file.is_open()) return paths;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        int col = 0;
        string posterPath;

        while (getline(ss, token, '|')) {
            if (col == 11) {
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
