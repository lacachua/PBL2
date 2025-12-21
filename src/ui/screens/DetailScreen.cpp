#include "UI/screens/DetailScreen.h"
#include "repositories/MovieRepository.h"
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
    // Initialize global search bar with movie data
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

    // Các nút
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

String DetailScreen::getMovieId() const {
    return movie.movie_id;
}

