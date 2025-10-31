#include "AdminMovieForm.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <locale>

AdminMovieForm::AdminMovieForm(const Font& upperFont, const Font& lowerFont)
        : titleFont(upperFont), labelFont(lowerFont), textFont(lowerFont),
            backButton(lowerFont, L"← Quay lại", 140.f, 45.f, 18),
      saveButton(lowerFont, L"Lưu", 130.f, 50.f, 20),
      cancelButton(lowerFont, L"Hủy", 130.f, 50.f, 20),
            pageTitle(upperFont, L"", 26), mode(MovieFormMode::CREATE), isEditing(false), currentMovie(nullptr)
{
    // Content background
    contentBg.setSize({1428.f, 922.f});
    contentBg.setPosition({300.f, 50.f});
    contentBg.setFillColor(Color(248, 249, 251)); // #F8F9FB

    // Form container
    formContainer.setSize({1340.f, 830.f});
    formContainer.setPosition({340.f, 90.f});
    formContainer.setFillColor(Color::White);
    formContainer.setOutlineColor(Color(209, 213, 219));
    formContainer.setOutlineThickness(1.f);

    // Page title
    pageTitle.setPosition({360.f, 110.f});
    pageTitle.setFillColor(Color(17, 24, 39));

    // Back button
    backButton.setPosition({1540.f, 110.f});
    backButton.setNormalColor(Color(107, 114, 128));
    backButton.setHoverColor(Color(75, 85, 99));
    backButton.setTextFillColor(Color::White);

    // Layout
    float leftColX = 380.f;
    float rightColX = 1020.f;
    float startY = 180.f;
    float fieldGap = 75.f;
    float leftWidth = 580.f;
    float rightWidth = 600.f;
    float inputHeight = 45.f;

    initializeField(titleField, L"Tên phim *", leftColX, startY, leftWidth, inputHeight);
    initializeField(genreField, L"Thể loại *", leftColX, startY + fieldGap, leftWidth, inputHeight);
    initializeField(durationField, L"Thời lượng (phút) *", leftColX, startY + fieldGap*2, leftWidth, inputHeight);
    initializeField(directorField, L"Đạo diễn", leftColX, startY + fieldGap*3, leftWidth, inputHeight);
    initializeField(castField, L"Diễn viên", leftColX, startY + fieldGap*4, leftWidth, inputHeight);
    initializeField(descriptionField, L"Nội dung phim", leftColX, startY + fieldGap*5, leftWidth, 90.f, true);
    initializeField(posterPathField, L"Đường dẫn poster", leftColX, startY + fieldGap*6.5f, leftWidth, inputHeight);

    initializeField(ageRatingField, L"Độ tuổi *", rightColX, startY, rightWidth, inputHeight);
    initializeField(countryField, L"Quốc gia *", rightColX, startY + fieldGap, rightWidth, inputHeight);
    initializeField(languageField, L"Ngôn ngữ *", rightColX, startY + fieldGap*2, rightWidth, inputHeight);
    initializeField(releaseDateField, L"Ngày công chiếu (dd/mm/yyyy) *", rightColX, startY + fieldGap*3, rightWidth, inputHeight);
    initializeField(statusField, L"Trạng thái *", rightColX, startY + fieldGap*4, rightWidth, inputHeight);

    // Buttons
    saveButton.setPosition({1500.f, 860.f});
    saveButton.setNormalColor(Color(22, 163, 74));
    saveButton.setHoverColor(Color(21, 128, 61));
    saveButton.setTextFillColor(Color::White);

    cancelButton.setPosition({1350.f, 860.f});
    cancelButton.setNormalColor(Color(220, 38, 38));
    cancelButton.setHoverColor(Color(185, 28, 28));
    cancelButton.setTextFillColor(Color::White);

    // Defaults
    clearFields();
}

AdminMovieForm::~AdminMovieForm() {}

void AdminMovieForm::initializeField(EditableField& field, const wstring& label, float x, float y, float width, float height, bool isMultiline) {
    field.label = label;
    field.isMultiline = isMultiline;
    field.value = L"";
    field.cursorPos = 0;
    field.isFocused = false;
    field.maxWidth = width - 24.f;

    field.box.setSize({width, height});
    field.box.setPosition({x, y + 30.f});
    field.box.setFillColor(Color::White);
    field.box.setOutlineColor(Color(209, 213, 219));
    field.box.setOutlineThickness(1.f);
}

void AdminMovieForm::drawField(RenderWindow& window, EditableField& field, const Font& font) {
    float labelY = field.box.getPosition().y - 27.f;
    float labelX = field.box.getPosition().x;
    Text labelText(font, field.label, 16);
    labelText.setPosition({labelX, labelY});
    labelText.setFillColor(Color(17, 24, 39));
    labelText.setStyle(Text::Bold);
    window.draw(labelText);

    if (field.isFocused) {
        field.box.setOutlineColor(Color(20, 118, 172));
        field.box.setOutlineThickness(2.f);
    } else {
        field.box.setOutlineColor(Color(209, 213, 219));
        field.box.setOutlineThickness(1.f);
    }
    window.draw(field.box);

    if (field.isMultiline) {
        wstring displayText = truncateText(field.value, field.maxWidth, font, 15);
        Text valueText(font, displayText, 15);
        valueText.setPosition({field.box.getPosition().x + 12.f, field.box.getPosition().y + 12.f});
        valueText.setFillColor(field.value.empty() ? Color(156, 163, 175) : Color(31, 41, 55));
        window.draw(valueText);
    } else {
        wstring displayText = field.value;
        if (displayText.length() > 50) displayText = displayText.substr(0, 50) + L"...";
        Text valueText(font, displayText.empty() ? L"" : displayText, 15);
        float textY = field.box.getPosition().y + (field.box.getSize().y - 20.f) / 2.f;
        valueText.setPosition({field.box.getPosition().x + 12.f, textY});
        valueText.setFillColor(displayText.empty() ? Color(156, 163, 175) : Color(31, 41, 55));
        window.draw(valueText);

        if (field.isFocused) {
            float cursorX = field.box.getPosition().x + 12.f;
            if (!displayText.empty()) {
                Text tempText(font, displayText, 15);
                cursorX += tempText.getGlobalBounds().size.x;
            }
            RectangleShape cursor({2.f, 22.f});
            cursor.setPosition({cursorX, textY + 1.f});
            cursor.setFillColor(Color::Black);
            window.draw(cursor);
        }
    }
}

wstring AdminMovieForm::stringToWString(const string& str) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    try { return converter.from_bytes(str); } catch (...) { return L""; }
}

string AdminMovieForm::wstringToString(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    try { return converter.to_bytes(wstr); } catch (...) { return ""; }
}

wstring AdminMovieForm::truncateText(const wstring& text, float maxWidth, const Font& font, unsigned int charSize) {
    Text temp(font, text, charSize);
    if (temp.getGlobalBounds().size.x <= maxWidth) return text;
    wstring result = text;
    while (!result.empty()) {
        result.pop_back();
        temp.setString(result + L"...");
        if (temp.getGlobalBounds().size.x <= maxWidth) return result + L"...";
    }
    return L"";
}

void AdminMovieForm::setMode(MovieFormMode newMode) {
    mode = newMode;
    isEditing = (newMode == MovieFormMode::EDIT);
    if (mode == MovieFormMode::CREATE) {
        pageTitle.setString(L"TẠO PHIM MỚI");
        clearFields();
    } else {
        pageTitle.setString(L"CHỈNH SỬA PHIM");
    }
}

void AdminMovieForm::loadMovieData(const Movie& movie) {
    currentMovie = const_cast<Movie*>(&movie);
    titleField.value = stringToWString(movie.title);
    ageRatingField.value = stringToWString(movie.age_rating);
    countryField.value = stringToWString(movie.country);
    languageField.value = stringToWString(movie.language);
    genreField.value = stringToWString(movie.genres);
    durationField.value = stringToWString(movie.duration_min);
    releaseDateField.value = stringToWString(movie.release_date);
    directorField.value = stringToWString(movie.director);
    castField.value = stringToWString(movie.cast);
    descriptionField.value = stringToWString(movie.synopsis);
    posterPathField.value = stringToWString(movie.poster_path);
    statusField.value = stringToWString(movie.status);
}

void AdminMovieForm::clearFields() {
    titleField.value = L"";
    ageRatingField.value = L"";
    countryField.value = L"";
    languageField.value = L"";
    genreField.value = L"";
    durationField.value = L"";
    releaseDateField.value = L"";
    directorField.value = L"";
    castField.value = L"";
    descriptionField.value = L"";
    posterPathField.value = L"../assets/posters/";
    statusField.value = L"Sắp chiếu";
    focusedField = nullptr;
}

void AdminMovieForm::draw(RenderWindow& window) {
    // Draw title
    window.draw(pageTitle);
    backButton.draw(window);
    window.draw(formContainer);

    // Draw fields
    drawField(window, titleField, textFont);
    drawField(window, genreField, textFont);
    drawField(window, durationField, textFont);
    drawField(window, directorField, textFont);
    drawField(window, castField, textFont);
    drawField(window, descriptionField, textFont);
    drawField(window, posterPathField, textFont);

    drawField(window, ageRatingField, textFont);
    drawField(window, countryField, textFont);
    drawField(window, languageField, textFont);
    drawField(window, releaseDateField, textFont);
    drawField(window, statusField, textFont);

    saveButton.draw(window);
    cancelButton.draw(window);
}

void AdminMovieForm::update(Vector2f mousePos, bool mousePressed) {
    backButton.update(mousePos);
    saveButton.update(mousePos);
    cancelButton.update(mousePos);
    if (mousePressed) {
        focusedField = nullptr;
        vector<EditableField*> allFields = {&titleField,&genreField,&durationField,&directorField,&castField,&descriptionField,&posterPathField,&ageRatingField,&countryField,&languageField,&releaseDateField,&statusField};
        for (auto* f : allFields) {
            f->isFocused = f->box.getGlobalBounds().contains(mousePos);
            if (f->isFocused) focusedField = f;
        }
    }
}

void AdminMovieForm::handleTextInput(uint32_t unicode) {
    if (!focusedField) return;
    if (unicode == 8) { // backspace
        if (!focusedField->value.empty()) focusedField->value.pop_back();
        return;
    }
    if (unicode >= 32) {
        focusedField->value += static_cast<wchar_t>(unicode);
    }
}

void AdminMovieForm::handleKeyPress(Keyboard::Key key) {
    if (!focusedField) return;
    if (key == Keyboard::Key::Backspace) {
        if (!focusedField->value.empty()) focusedField->value.pop_back();
    } else if (key == Keyboard::Key::Enter && focusedField->isMultiline) {
        focusedField->value += L"\n";
    }
}

bool AdminMovieForm::isBackClicked(Vector2f mousePos, bool mousePressed) { return backButton.isClicked(mousePos, mousePressed); }
bool AdminMovieForm::isSaveClicked(Vector2f mousePos, bool mousePressed) { return saveButton.isClicked(mousePos, mousePressed); }
bool AdminMovieForm::isCancelClicked(Vector2f mousePos, bool mousePressed) { return cancelButton.isClicked(mousePos, mousePressed); }

Movie AdminMovieForm::getMovieData() {
    Movie movie;
    if (isEditing && currentMovie) movie.film_id = currentMovie->film_id; else {
        time_t now = time(0);
        movie.film_id = "F" + to_string(now % 100000);
    }
    movie.title = wstringToString(titleField.value);
    movie.age_rating = wstringToString(ageRatingField.value);
    movie.country = wstringToString(countryField.value);
    movie.language = wstringToString(languageField.value);
    movie.genres = wstringToString(genreField.value);
    movie.duration_min = wstringToString(durationField.value);
    movie.release_date = wstringToString(releaseDateField.value);
    movie.director = wstringToString(directorField.value);
    movie.cast = wstringToString(castField.value);
    movie.synopsis = wstringToString(descriptionField.value);
    movie.poster_path = wstringToString(posterPathField.value);
    movie.status = wstringToString(statusField.value);
    return movie;
}

void AdminMovieForm::saveToCSV(const string& filepath, const vector<Movie>& movies) {
    // Use wofstream with UTF-8 locale
    std::wofstream wof(filepath);
    if (!wof.is_open()) return;
    try {
        wof.imbue(std::locale(wof.getloc(), new std::codecvt_utf8<wchar_t>()));
    } catch (...) {}

    // Header
    wof << L"film_id,title,age_rating,country,language,genres,duration_min,release_date,director,cast,synopsis,poster_path,status\n";
    for (const auto& m : movies) {
        wof << stringToWString(m.film_id) << L"," << stringToWString(m.title) << L"," << stringToWString(m.age_rating) << L"," << stringToWString(m.country)
            << L"," << stringToWString(m.language) << L"," << stringToWString(m.genres) << L"," << stringToWString(m.duration_min) << L"," << stringToWString(m.release_date)
            << L"," << stringToWString(m.director) << L"," << stringToWString(m.cast) << L"," << stringToWString(m.synopsis) << L"," << stringToWString(m.poster_path)
            << L"," << stringToWString(m.status) << L"\n";
    }
    wof.close();
}
