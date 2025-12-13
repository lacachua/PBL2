#pragma once

#include "UI/components/PosterSlider/IPosterProvider.h"
#include "data-structures/DLL.h"
#include "services/MovieSearchManager.h" // MovieDetail

#include <SFML/System/String.hpp>

#include <string>

class MovieRepository : public IPosterProvider {
private:
    sf::String filePath;
    DLL<MovieDetail> movies;
    int selectedIndex = -1;

    void loadMovies();

public:
    MovieRepository(const sf::String& path);

    // Returns movieId if not found
    sf::String getMovieTitleById(const sf::String& movieId) const;

    DLL<sf::String> getPosterPaths() override;

    void setSelectedIndex(int index);
    int getSelectedIndex() const;

    MovieDetail getMovieDetailbyIndex(int index) const;

    const DLL<MovieDetail>& getAllMovies() const;
};
