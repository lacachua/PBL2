// #pragma once
// #include <SFML/Graphics.hpp>
// #include "Slide.h"
// #include "HomeScreen.h"
// #include "Movie.h"

// using namespace sf;
// using namespace std;

// class DetailScreen : public HomeScreen {
// private:
//     Font titleFont, detailFont;
//     Sprite poster;
//     Texture posterTexture;
    
//     Text titleText, genreText, durationText, countryText, castText, descriptionText;
//     Text ageRatingText, languageText, directorText, statusText, releaseDateText;
//     Movie currentMovie;
//     vector<Movie> allMovies;

//     Texture icon1, icon2, icon3, icon4, icon5;
//     Sprite genreIcon, durationIcon, countryIcon, languageIcon, ageRatingIcon;

//     Text detailHeader, synopsisHeader;

//     RectangleShape bookingButton;
//     Text bookingText;
// public:
//     DetailScreen(Font&, int movieIndex, const string& username);

//     void update(Vector2f, bool, AppState&);
//     void draw(RenderWindow&);
//     bool isButtonHovered(Vector2f) const;
//     bool isButtonClicked(Vector2f, bool) const;
//     void highlightButton(bool);
//     void loadMovieData(int movieIndex);
// };

#pragma once
#include <SFML/Graphics.hpp>
#include "Slide.h"
#include "HomeScreen.h"
#include "Movie.h"

using namespace sf;
using namespace std;

class DetailScreen : public HomeScreen {
private:
    Font titleFont, detailFont;
    Sprite poster;
    Texture posterTexture;

    Text titleText, genreText, durationText, countryText, castText, descriptionText;
    Text ageRatingText, languageText, directorText, statusText, releaseDateText;
    Movie currentMovie;
    vector<Movie> allMovies;

    Texture icon1, icon2, icon3, icon4, icon5;
    Sprite genreIcon, durationIcon, countryIcon, languageIcon, ageRatingIcon;

    Text detailHeader, synopsisHeader;

    RectangleShape bookingButton;
    Text bookingText;

public:
    DetailScreen(Font&, int movieIndex, const string& username);

    void update(Vector2f, bool, AppState&);
    void update(Vector2f, bool, AppState&, const Event* event);
    void draw(RenderWindow&);
    bool isButtonHovered(Vector2f) const;
    bool isButtonClicked(Vector2f, bool) const;
    void highlightButton(bool);
    void loadMovieData(int movieIndex);

    const Texture& getPosterTexture() const;
    const Sprite& getPosterSprite() const;
    int getCurrentMovieId() const { 
        try {
            return !currentMovie.film_id.empty() ? stoi(currentMovie.film_id) : 1;
        } catch (...) {
            return 1; // Default to movie 1
        }
    }
};
