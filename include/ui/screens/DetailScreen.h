// // #pragma once
// // #include <SFML/Graphics.hpp>
// // #include "Slide.h"
// // #include "HomeScreen.h"
// // #include "Movie.h"

// // using namespace sf;
// // using namespace std;

// // class DetailScreen : public HomeScreen {
// // private:
// //     Font titleFont, detailFont;
// //     Sprite poster;
// //     Texture posterTexture;
    
// //     Text titleText, genreText, durationText, countryText, castText, descriptionText;
// //     Text ageRatingText, languageText, directorText, statusText, releaseDateText;
// //     Movie currentMovie;
// //     vector<Movie> allMovies;

// //     Texture icon1, icon2, icon3, icon4, icon5;
// //     Sprite genreIcon, durationIcon, countryIcon, languageIcon, ageRatingIcon;

// //     Text detailHeader, synopsisHeader;

// //     RectangleShape bookingButton;
// //     Text bookingText;
// // public:
// //     DetailScreen(Font&, int movieIndex, const string& username);

// //     void update(Vector2f, bool, AppState&);
// //     void draw(RenderWindow&);
// //     bool isButtonHovered(Vector2f) const;
// //     bool isButtonClicked(Vector2f, bool) const;
// //     void highlightButton(bool);
// //     void loadMovieData(int movieIndex);
// // };

// #pragma once
// #include <SFML/Graphics.hpp>
// #include "UI/components/Slide.h"
// #include "HomeScreen.h"
// #include "models/Movie.h"

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
//     void update(Vector2f, bool, AppState&, const Event* event);
//     void draw(RenderWindow&);
//     bool isButtonHovered(Vector2f) const;
//     bool isButtonClicked(Vector2f, bool) const;
//     void highlightButton(bool);
//     void loadMovieData(int movieIndex);

//     const Texture& getPosterTexture() const;
//     const Sprite& getPosterSprite() const;
//     int getCurrentMovieId() const { 
//         try {
//             return !currentMovie.film_id.empty() ? stoi(currentMovie.film_id) : 1;
//         } catch (...) {
//             return 1; // Default to movie 1
//         }
//     }
// };

#pragma once
#include "BaseScreen.h"
#include "UI/components/Button.h"
#include "UI/components/TextButton.h"
#include "core/AppState.h"
#include "services/MovieSearchManager.h"

// MovieDetail is now defined in MovieSearchManager.h

class DetailScreen : public BaseScreen {
    private:
        const Font& button_font;    // Font cho nút quay lại (TextButton) - được truyền vào
        const Font& title_font;     // Font cho tiêu đề phim và nút đặt vé - được truyền vào
        const Font& detail_font;    // Font cho nội dung chi tiết - được truyền vào
        
    Texture poster_tex;
    Sprite poster;
    Text title_text;
    Text genre_text;
    Text duration_text;
    Text country_text;
    Text language_text;
    Text rating_text;
    Text description_header;
    Text director_text;
    Text cast_text;
    Text synopsis_header;
    Text synopsis_text;
        TextButton backBtn;
        Button bookBtn;
        MovieDetail movie;
        Texture icon1, icon2, icon3, icon4, icon5;
        Sprite genreIcon, durationIcon, countryIcon, languageIcon, ageRatingIcon;
    public:
        // Constructor nhận 4 tham số: headerFont, buttonFont, titleFont, detailFont
        DetailScreen(Font& headerFont, Font& btnFont, Font& titleFnt, Font& detailFnt, const MovieDetail&);
        void update(Vector2f, bool, AppState&) override;
        void draw(RenderWindow&) override;
        
        String getMovieId() const;
};