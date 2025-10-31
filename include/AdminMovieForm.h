#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include "Button.h"
#include "Movie.h"

using namespace std;
using namespace sf;

enum class MovieFormMode {
    CREATE,
    EDIT
};

struct EditableField {
    wstring label;
    wstring value;
    RectangleShape box;
    bool isMultiline;
    bool isFocused;
    int cursorPos;
    float maxWidth;
    
    EditableField() : isMultiline(false), isFocused(false), cursorPos(0), maxWidth(0) {}
};

class AdminMovieForm {
private:
    Font titleFont;
    Font labelFont;
    Font textFont;
    
    
    // Container
    RectangleShape contentBg;
    RectangleShape formContainer;
    
    // Title
    Text pageTitle;
    Button backButton;
    
    // Form fields (12 total)
    // Left column (7 fields)
    EditableField titleField;          // 1. Tên phim
    EditableField genreField;          // 5. Thể loại
    EditableField durationField;       // 6. Thời lượng
    EditableField directorField;       // 8. Đạo diễn
    EditableField castField;           // 9. Diễn viên
    EditableField descriptionField;    // 10. Nội dung phim (multiline)
    EditableField posterPathField;     // 11. Poster Path
    
    // Right column (5 fields)
    EditableField ageRatingField;      // 2. Độ tuổi
    EditableField countryField;        // 3. Quốc gia
    EditableField languageField;       // 4. Ngôn ngữ
    EditableField releaseDateField;    // 7. Ngày công chiếu
    EditableField statusField;         // 12. Trạng thái
    
    // Bottom buttons
    Button saveButton;
    Button cancelButton;
    
    MovieFormMode mode;
    bool isEditing;
    Movie* currentMovie;
    wstring currentBreadcrumb;
    
    // Helper methods
    void initializeField(EditableField& field, const wstring& label, float x, float y, float width, float height, bool isMultiline = false);
    void drawField(RenderWindow& window, EditableField& field, const Font& font);
    wstring stringToWString(const string& str);
    string wstringToString(const wstring& wstr);
    wstring truncateText(const wstring& text, float maxWidth, const Font& font, unsigned int charSize);
    
    // Input handling
    EditableField* focusedField = nullptr;
    
public:
    AdminMovieForm(const Font& upperFont, const Font& lowerFont);
    ~AdminMovieForm();
    
    void setMode(MovieFormMode newMode);
    void loadMovieData(const Movie& movie);
    void clearFields();
    
    void draw(RenderWindow& window);
    void update(Vector2f mousePos, bool mousePressed);
    void handleTextInput(uint32_t unicode);
    void handleKeyPress(Keyboard::Key key);
    
    // Getters
    bool isBackClicked(Vector2f mousePos, bool mousePressed);
    bool isSaveClicked(Vector2f mousePos, bool mousePressed);
    bool isCancelClicked(Vector2f mousePos, bool mousePressed);
    
    // Data extraction
    Movie getMovieData();
    void saveToCSV(const string& filepath, const vector<Movie>& movies);
};
