// #include "UI/screens/DetailScreen.h"
// #include <locale>
// #include <codecvt>
// #include <sstream>

// wstring wrapText(const wstring& text, const Font& font, unsigned int fontSize, float maxWidth) {
//     if (text.empty()) return text;
    
//     Text testText(font, L"", fontSize);
//     wstring wrappedText = L"";
//     wstring currentLine = L"";
    
//     wstring remainingText = text;
//     size_t pos = 0;
    
//     while (pos < remainingText.length()) {
//         size_t newlinePos = remainingText.find(L'\n', pos);
//         if (newlinePos == wstring::npos) newlinePos = remainingText.length();
        
//         wstring segment = remainingText.substr(pos, newlinePos - pos);
        
//         // Process each word in the segment
//         wstringstream ss(segment);
//         wstring word;
        
//         while (ss >> word) {
//             wstring testLine = currentLine.empty() ? word : currentLine + L" " + word;
//             testText.setString(testLine);
            
//             if (testText.getLocalBounds().size.x <= maxWidth) {
//                 currentLine = testLine;
//             } else {
//                 if (!currentLine.empty()) {
//                     wrappedText += currentLine + L"\n";
//                 }
//                 currentLine = word;
//             }
//         }
        
//         if (newlinePos < remainingText.length()) {
//             // Add current line and newline
//             if (!currentLine.empty()) {
//                 wrappedText += currentLine + L"\n";
//                 currentLine = L"";
//             }
//         }
        
//         pos = newlinePos + 1;
//     }
    
//     // Add the last line
//     if (!currentLine.empty()) {
//         wrappedText += currentLine;
//     }
    
//     return wrappedText;
// }

// DetailScreen::DetailScreen(Font& font, int movieIndex, const string& username) : 
//     HomeScreen(font),
//     titleFont("../assets/BEBAS_NEUE_ZSMALL.ttf"),
//     detailFont("../assets/quicksand_medium.ttf"),
//     poster(posterTexture),
//     titleText(titleFont, L"", 62),
//     ageRatingText(titleFont, L"", 62),
//     genreText(detailFont, L"", 20),
//     durationText(detailFont, L"", 20),
//     countryText(detailFont, L"", 20),
//     languageText(detailFont, L"", 20),
//     directorText(detailFont, L"", 20),
//     castText(detailFont, L"", 20),
//     releaseDateText(detailFont, L"", 20),
//     statusText(detailFont, L"", 20),
//     descriptionText(detailFont, L"", 19),
//     detailHeader(titleFont, L"MÔ TẢ", 40),
//     synopsisHeader(titleFont, L"NỘI DUNG PHIM", 40),
//     icon1("../assets/elements/genres.png"),
//     genreIcon(icon1),
//     icon2("../assets/elements/duration_time.png"),
//     durationIcon(icon2),
//     icon3("../assets/elements/country.png"),
//     countryIcon(icon3),
//     icon4("../assets/elements/language.png"),
//     languageIcon(icon4),
//     icon5("../assets/elements/ageRating.png"),
//     ageRatingIcon(icon5),

//     bookingButton({200, 45}),
//     bookingText(titleFont, L"ĐẶT VÉ NGAY", 32)
// {
//     setLoggedUser(username);
//     loadMovieData(movieIndex);

//     titleText.setFillColor(Color::White);
//     titleText.setStyle(Text::Bold);
//     titleText.setPosition({804, 110});
    
//     ageRatingText.setFillColor(Color::White);
//     ageRatingText.setStyle(Text::Bold);

//     FloatRect titleBounds = titleText.getGlobalBounds();
//     float titleEndX = titleBounds.position.x + titleBounds.size.x;
//     float titleEndY = titleBounds.position.y + titleBounds.size.y;

//     ageRatingText.setPosition({titleEndX + 20.f, titleText.getPosition().y});
    
//     genreIcon.setPosition({804, titleEndY + 20});
//     durationIcon.setPosition({804, titleEndY + 50});
//     countryIcon.setPosition({804, titleEndY + 80});
//     languageIcon.setPosition({804, titleEndY + 110});
//     ageRatingIcon.setPosition({804, titleEndY + 140});

//     genreIcon.setScale({0.048f, 0.048f});
//     durationIcon.setScale({0.048f, 0.048f});
//     countryIcon.setScale({0.048f, 0.048f});
//     languageIcon.setScale({0.048f, 0.048f});
//     ageRatingIcon.setScale({0.048f, 0.048f});
    
//     genreText.setPosition({834, titleEndY + 20});
//     durationText.setPosition({834, titleEndY + 50});
//     countryText.setPosition({834, titleEndY + 80});
//     languageText.setPosition({834, titleEndY + 110});
//     statusText.setPosition({834, titleEndY + 140});

//     // ✅ Detail texts màu trắng nhẹ
//     Color detailColor(230, 230, 230);
//     genreText.setFillColor(detailColor);
//     durationText.setFillColor(detailColor);
//     countryText.setFillColor(detailColor);
//     languageText.setFillColor(detailColor);
//     directorText.setFillColor(detailColor);
//     castText.setFillColor(detailColor);
//     releaseDateText.setFillColor(detailColor);
//     statusText.setFillColor(detailColor);
//     descriptionText.setFillColor(Color(200, 200, 200));
    
//     // ✅ Section headers
//     detailHeader.setFillColor(Color::White);
//     detailHeader.setStyle(Text::Bold);
//     detailHeader.setPosition({804, titleEndY + 184});

//     directorText.setPosition({804, detailHeader.getGlobalBounds().position.y + detailHeader.getGlobalBounds().size.y + 14});
//     castText.setPosition({804, directorText.getGlobalBounds().position.y + directorText.getGlobalBounds().size.y + 9});
//     releaseDateText.setPosition({804, castText.getGlobalBounds().position.y + castText.getGlobalBounds().size.y + 9});
    
//     synopsisHeader.setFillColor(Color::White);
//     synopsisHeader.setStyle(Text::Bold);
//     synopsisHeader.setPosition({804, releaseDateText.getGlobalBounds().position.y + releaseDateText.getGlobalBounds().size.y + 14});

//     descriptionText.setPosition({804, synopsisHeader.getGlobalBounds().position.y + synopsisHeader.getGlobalBounds().size.y + 14});

//     poster.setPosition({168, 126});
//     poster.setScale({0.36f, 0.36f});

//     bookingButton.setPosition({poster.getGlobalBounds().size.x + poster.getGlobalBounds().position.x + 60, poster.getGlobalBounds().size.y + poster.getGlobalBounds().position.y - 50});
//     bookingButton.setFillColor(Color(255, 255, 255, 220));
//     bookingButton.setOutlineThickness(2.f);
//     bookingButton.setOutlineColor(Color(200, 200, 200));

//     bookingText.setPosition({bookingButton.getGlobalBounds().position.x + 40, bookingButton.getGlobalBounds().position.y + 5});
//     bookingText.setFillColor(Color::Black);
// }

// void DetailScreen::loadMovieData(int movieIndex) {
//     allMovies = loadMoviesFromCSV("../data/movies.csv");

//     // Get the selected movie (validate index)
//     if (movieIndex >= 0 && movieIndex < allMovies.size())
//         currentMovie = allMovies[movieIndex];
//     else {
//         cout << "⚠️ Chỉ số phim không hợp lệ: " << movieIndex << endl;
//         if (!allMovies.empty()) {
//             currentMovie = allMovies[0]; // Fallback to first movie
//         } else {
//             return;
//         }
//     }

//     // Convert string to wstring for SFML Text
//     wstring_convert<codecvt_utf8<wchar_t>> converter;
    
//     // ✅ Title uppercase
//     wstring title = converter.from_bytes(currentMovie.title);
//     transform(title.begin(), title.end(), title.begin(), ::towupper);
//     titleText.setString(title);
    
//     // ✅ Age rating trong ngoặc, đặt cạnh title
//     wstring ageRating = L"(" + converter.from_bytes(currentMovie.age_rating) + L")";
//     ageRatingText.setString(ageRating);
    
//     // ✅ Detail info (without "Thể loại:", "Thời lượng:" labels, just values)
//     genreText.setString(converter.from_bytes(currentMovie.genres));
    
//     wstring duration = converter.from_bytes(currentMovie.duration_min) + L"'"; // 118'
//     durationText.setString(duration);
    
//     countryText.setString(converter.from_bytes(currentMovie.country));
//     languageText.setString(converter.from_bytes(currentMovie.language));
    
//     // ✅ Age rating description với background vàng (hiển thị mô tả đầy đủ)
//     wstring ageDesc = converter.from_bytes(currentMovie.age_rating) + 
//                      L": Phim dành cho khán giả từ đủ " + 
//                      converter.from_bytes(currentMovie.age_rating.substr(1)) + 
//                      L" tuổi trở lên (" + 
//                      converter.from_bytes(currentMovie.age_rating) + L"+)";
//     statusText.setString(ageDesc);
//     statusText.setFillColor(Color::Black); // Text màu đen
    
//     // Director
//     wstring director = L"Đạo diễn: " + converter.from_bytes(currentMovie.director);
//     directorText.setString(director);
    
//     // ✅ Cast with wrapping
//     wstring cast = L"Diễn viên: " + converter.from_bytes(currentMovie.cast);
//     wstring wrappedCast = wrapText(cast, detailFont, 20, 620.0f);
//     castText.setString(wrappedCast);
    
//     // Release date
//     wstring releaseDate = L"Khởi chiếu: " + converter.from_bytes(currentMovie.release_date);
//     releaseDateText.setString(releaseDate);
    
//     // ✅ Synopsis with wrapping
//     wstring synopsis = converter.from_bytes(currentMovie.synopsis);
//     wstring wrappedSynopsis = wrapText(synopsis, detailFont, 19, 620.0f);
//     descriptionText.setString(wrappedSynopsis);
    
//     // Load poster image
//     if (posterTexture.loadFromFile(currentMovie.poster_path)) {
//         poster.setTexture(posterTexture, true); // ✅ true để reset texture rect
//     } else {
//         cout << "⚠️ Không thể tải poster: " << currentMovie.poster_path << endl;
//     }

//     // FloatRect descBounds = descriptionText.getGlobalBounds();
//     // float bookingX = bookingButton.getPosition().x;
//     // float bookingY = descriptionText.getPosition().y + descBounds.size.y + 22.f;
//     // bookingButton.setPosition({bookingX, bookingY});
// }

// void DetailScreen::update(Vector2f mouse, bool mousePressed, AppState& state) {
//     HomeScreen::update(mouse, mousePressed, state);
// // <<<<<<< HEAD
// // }

// // void DetailScreen::update(Vector2f mouse, bool mousePressed, AppState& state, const Event* event) {
// //     HomeScreen::update(mouse, mousePressed, state, event);
// // =======
//     bool hovered = isButtonHovered(mouse);
//     highlightButton(hovered);
//     if (isButtonClicked(mouse, mousePressed)) 
//         state = AppState::BOOKING;
// // >>>>>>> feature-datvengay
// } 

// void DetailScreen::draw(RenderWindow& window) {
//     HomeScreen::draw(window);

//     // ✅ Draw icons first
//     window.draw(genreIcon);
//     window.draw(durationIcon);
//     window.draw(countryIcon);
//     window.draw(languageIcon);
//     window.draw(ageRatingIcon);
    
//     // ✅ Draw title and age rating (same line)
//     window.draw(titleText);
//     window.draw(ageRatingText);
    
//     // ✅ Draw section headers
//     window.draw(detailHeader);
//     window.draw(synopsisHeader);
    
//     // ✅ Draw detail texts
//     window.draw(genreText);
//     window.draw(durationText);
//     window.draw(countryText);
//     window.draw(languageText);
//     window.draw(statusText);
//     window.draw(directorText);
//     window.draw(castText);
//     window.draw(releaseDateText);
//     window.draw(descriptionText);
    
//     window.draw(poster);
//     window.draw(bookingButton);
//     window.draw(bookingText);
// }

// bool DetailScreen::isButtonHovered(Vector2f mousePos) const {
//     return bookingButton.getGlobalBounds().contains(mousePos);
// }

// bool DetailScreen::isButtonClicked(Vector2f mousePos, bool mousePressed) const {
//     return mousePressed && isButtonHovered(mousePos);
// }

// void DetailScreen::highlightButton(bool hovered) {
//     if (hovered) {
//         bookingButton.setFillColor(Color(52, 62, 209, 255));
//         bookingText.setFillColor(Color::White);
//     } else {
//         bookingButton.setFillColor(Color(255, 255, 255, 235));
//         bookingText.setFillColor(Color::Black);
//     }
// }

// const Texture& DetailScreen::getPosterTexture() const {
//     return posterTexture;
// }

// const Sprite& DetailScreen::getPosterSprite() const {
//     return poster;
// }

#include "UI/screens/DetailScreen.h"
#include "models/MovieRepository.h"
#include <sstream>

using namespace sf;
using namespace std;

static String wrapText(const String& text, const Font& font, unsigned int fontSize, float maxWidth) {
    if (text.isEmpty()) return text;

    Text measure(font, String(), fontSize);
    std::wstring source = text.toWideString();
    std::wstring wrapped;
    std::wstring currentLine;
    size_t pos = 0;

    auto appendLine = [&](const std::wstring& line) {
        if (line.empty()) return;
        if (!wrapped.empty() && wrapped.back() != L'\n') wrapped += L'\n';
        wrapped += line;
    };

    while (pos <= source.length()) {
        size_t newlinePos = source.find(L'\n', pos);
        if (newlinePos == std::wstring::npos) newlinePos = source.length();

        std::wstring segment = source.substr(pos, newlinePos - pos);
        std::wstringstream ss(segment);
        std::wstring word;

        while (ss >> word) {
            std::wstring candidate = currentLine.empty() ? word : currentLine + L" " + word;
            measure.setString(String(candidate));

            if (measure.getLocalBounds().size.x <= maxWidth) {
                currentLine = candidate;
            } else {
                appendLine(currentLine);
                currentLine = word;
            }
        }

        appendLine(currentLine);
        currentLine.clear();

        if (newlinePos < source.length()) {
            if (!wrapped.empty()) wrapped += L'\n';
        }

        pos = newlinePos + 1;
        if (newlinePos == source.length()) break;
    }

    return String(wrapped);
}

DetailScreen::DetailScreen(Font& headerFont, Font& btnFont, Font& titleFnt, Font& detailFnt, const MovieDetail& detail)
        : BaseScreen(headerFont),           // Font cho header (logo, search, login buttons)
        button_font(btnFont),               // Font cho nút quay lại (TextButton)
        title_font(titleFnt),               // Font cho tiêu đề phim và nút đặt vé
        detail_font(detailFnt),             // Font cho nội dung chi tiết
        movie(detail),
        backBtn(button_font, L"← Quay lại", 18, {160.f, 40.f}),   // Dùng button_font
        bookBtn(title_font, L"ĐẶT VÉ NGAY", 180.f, 40.f, 30),     // Dùng title_font
        poster_tex(detail.posterPath),
        poster(poster_tex),
        title_text(title_font, detail.title, 66),                  // Dùng title_font
        genre_text(detail_font, String(), 20),
        duration_text(detail_font, String(), 20),
        country_text(detail_font, String(), 20),
        language_text(detail_font, String(), 20),
        rating_text(detail_font, String(), 20),
        description_header(title_font, L"MÔ TẢ", 36),
        director_text(detail_font, String(), 20),
        cast_text(detail_font, String(), 20),
        synopsis_header(title_font, L"NỘI DUNG PHIM", 36),
        synopsis_text(detail_font, String(), 20),
        icon1("../assets/elements/genres.png"),
        genreIcon(icon1),
        icon2("../assets/elements/duration_time.png"),
        durationIcon(icon2),
        icon3("../assets/elements/country.png"),
        countryIcon(icon3),
        icon4("../assets/elements/language.png"),
        languageIcon(icon4),
        icon5("../assets/elements/ageRating.png"),
        ageRatingIcon(icon5)
{
    // ✅ Initialize global search bar with movie data
    MovieRepository repo("../data/movies.txt");
    initializeGlobalSearch(repo.getAllMovies());
    poster.setScale({0.32f, 0.32f});
    poster.setScale({0.32f, 0.32f});
    poster.setPosition({150.f, 160.f});

    title_text.setFillColor(Color::White);
    title_text.setPosition({700.f, 150.f});
    title_text.setStyle(Text::Bold);
    title_text.setOutlineThickness(2.f);
    title_text.setOutlineColor(Color(20, 118, 172));

    const Color infoColor(220, 220, 220);
    genre_text.setFillColor(infoColor);
    duration_text.setFillColor(infoColor);
    country_text.setFillColor(infoColor);
    language_text.setFillColor(infoColor);
    rating_text.setFillColor(infoColor);

    genre_text.setLetterSpacing(1.02f);
    duration_text.setLetterSpacing(1.02f);
    country_text.setLetterSpacing(1.02f);
    language_text.setLetterSpacing(1.02f);
    rating_text.setLetterSpacing(1.02f);

    const Vector2f iconScale{0.048f, 0.048f};
    genreIcon.setScale(iconScale);
    durationIcon.setScale(iconScale);
    countryIcon.setScale(iconScale);
    languageIcon.setScale(iconScale);
    ageRatingIcon.setScale(iconScale);

    float iconX = 700.f;
    float textX = iconX + 40.f;
    FloatRect titleBounds = title_text.getGlobalBounds();
    float infoY = titleBounds.position.y + titleBounds.size.y + 15.f;
    const float lineSpacing = 38.f;

    auto placeInfoRow = [&](Sprite& icon, Text& text, const String& value) {
        text.setString(value);
        icon.setPosition({iconX, infoY});
        text.setPosition({textX, infoY - 2.f});
        infoY += lineSpacing;
    };

    String genresStr(movie.genres);
    String durationStr(movie.duration_min);
    String countryStr(movie.country);
    String languageStr(movie.language);
    String ratingStr(movie.age_rating);

    placeInfoRow(genreIcon, genre_text, String(L"Thể loại: ") + genresStr);
    placeInfoRow(durationIcon, duration_text, String(L"Thời lượng: ") + durationStr + String(L" phút"));
    placeInfoRow(countryIcon, country_text, String(L"Quốc gia: ") + countryStr);
    placeInfoRow(languageIcon, language_text, String(L"Ngôn ngữ: ") + languageStr);
    placeInfoRow(ageRatingIcon, rating_text, String(L"Phân loại: ") + ratingStr);

    description_header.setFillColor(Color::White);
    description_header.setStyle(Text::Bold);
    description_header.setPosition({iconX, infoY + 18.f});

    director_text.setFillColor(infoColor);
    director_text.setString(String(L"Đạo diễn: ") + String(movie.director));
    director_text.setPosition({iconX, description_header.getGlobalBounds().position.y + description_header.getGlobalBounds().size.y + 12.f});

    cast_text.setFillColor(infoColor);
    cast_text.setString(wrapText(String(L"Diễn viên: ") + String(movie.cast), detail_font, 20, 620.f));
    cast_text.setPosition({iconX, director_text.getGlobalBounds().position.y + director_text.getGlobalBounds().size.y + 10.f});

    synopsis_header.setFillColor(Color::White);
    synopsis_header.setStyle(Text::Bold);
    synopsis_header.setPosition({iconX, cast_text.getGlobalBounds().position.y + cast_text.getGlobalBounds().size.y + 24.f});

    synopsis_text.setFillColor(Color(230, 230, 230));
    synopsis_text.setString(wrapText(String(movie.synopsis), detail_font, 20, 620.f));
    synopsis_text.setPosition({iconX, synopsis_header.getGlobalBounds().position.y + synopsis_header.getGlobalBounds().size.y + 16.f});

    // ✅ Các nút
    backBtn.setPosition({150.f, 130.f});
    bookBtn.setPosition({iconX, 758.f});
    bookBtn.setFillColor(Color(255, 255, 255, 235));
    bookBtn.setOutlineThickness(0.f);
    bookBtn.setTextColor(Color::Black);
    // durationText.setPosition({834, title_text.getPosition().y + title_text.getGlobalBounds().size.y + 50});
    // countryText.setPosition({834, title_text.getPosition().y + title_text.getGlobalBounds().size.y + 80});
    // languageText.setPosition({834, title_text.getPosition().y + title_text.getGlobalBounds().size.y + 110});
    // statusText.setPosition({834, title_text.getPosition().y + title_text.getGlobalBounds().size.y + 140});
}

void DetailScreen::update(Vector2f mousePos, bool mousePressed, AppState& state) {
    // Update BaseScreen buttons (header buttons and global search)
    BaseScreen::update(mousePos, mousePressed, state);
    
    // Don't process detail screen buttons if search is active
    if (globalSearchBar && globalSearchBar->isInputActive()) return;
    
    // Update detail screen specific buttons
    backBtn.update(mousePos);
    bookBtn.update(mousePos, mousePressed, Color(30, 41, 202), Color(255, 255, 255, 235));
    bookBtn.setTextColor(bookBtn.isHovered() ? Color::White : Color::Black);

    // Check button clicks
    if (backBtn.isClicked(mousePos, mousePressed)) {
        state = AppState::HOME;
    }
    else if (bookBtn.isClicked(mousePos, mousePressed)) {
        state = AppState::BOOKING;
    }
}

void DetailScreen::draw(RenderWindow& window) {
    BaseScreen::draw(window);
    window.draw(poster);
    window.draw(title_text);
    window.draw(genreIcon);
    window.draw(genre_text);
    window.draw(durationIcon);
    window.draw(duration_text);
    window.draw(countryIcon);
    window.draw(country_text);
    window.draw(languageIcon);
    window.draw(language_text);
    window.draw(ageRatingIcon);
    window.draw(rating_text);
    window.draw(description_header);
    window.draw(director_text);
    window.draw(cast_text);
    window.draw(synopsis_header);
    window.draw(synopsis_text);
    backBtn.draw(window);
    bookBtn.draw(window);
}

