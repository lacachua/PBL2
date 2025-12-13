#pragma once

#include "UI/components/PosterSlider/IPosterProvider.h"
#include "data-structures/DLL.h"
#include "services/MovieSearchManager.h" // MovieDetail

#include <SFML/System/String.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class MovieRepository : public IPosterProvider {
private:
    sf::String filePath;
    DLL<MovieDetail> movies;
    int selectedIndex = -1;

    void loadMovies() {
        std::ifstream file(filePath.toAnsiString());
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> cols;
            while (std::getline(ss, token, '|')) {
                cols.push_back(token);
            }

            if (cols.size() < 13) continue;

            MovieDetail m;
            m.movie_id      = sf::String(cols[0]);
            m.title         = sf::String::fromUtf8(cols[1].begin(), cols[1].end());
            m.age_rating    = sf::String::fromUtf8(cols[2].begin(), cols[2].end());
            m.country       = sf::String::fromUtf8(cols[3].begin(), cols[3].end());
            m.language      = sf::String::fromUtf8(cols[4].begin(), cols[4].end());
            m.genres        = sf::String::fromUtf8(cols[5].begin(), cols[5].end());
            m.duration_min  = sf::String::fromUtf8(cols[6].begin(), cols[6].end());
            m.release_date  = sf::String::fromUtf8(cols[7].begin(), cols[7].end());
            m.director      = sf::String::fromUtf8(cols[8].begin(), cols[8].end());
            m.cast          = sf::String::fromUtf8(cols[9].begin(), cols[9].end());
            m.synopsis      = sf::String::fromUtf8(cols[10].begin(), cols[10].end());
            m.posterPath    = sf::String::fromUtf8(cols[11].begin(), cols[11].end());
            m.status        = sf::String::fromUtf8(cols[12].begin(), cols[12].end());

            movies.push_back(m);
        }
        file.close();
    }

public:
    MovieRepository(const sf::String& path) : filePath(path) {
        loadMovies();
    }

    // Returns movieId if not found
    sf::String getMovieTitleById(const sf::String& movieId) const {
        for (int i = 0; i < movies.getSize(); ++i) {
            if (movies[i].movie_id == movieId) {
                return movies[i].title;
            }
        }
        return movieId;
    }

    DLL<sf::String> getPosterPaths() override {
        DLL<sf::String> paths;
        for (int i = 0; i < movies.getSize(); i++)
            paths.push_back(movies[i].posterPath);
        return paths;
    }

    void setSelectedIndex(int index) {
        selectedIndex = index;
    }
    int getSelectedIndex() const {
        return selectedIndex;
    }

    MovieDetail getMovieDetailbyIndex(int index) const {
        if (index >= 0 && index < (int)movies.getSize())
            return movies[index];
        return MovieDetail();
    }

    const DLL<MovieDetail>& getAllMovies() const {
        return movies;
    }
};
