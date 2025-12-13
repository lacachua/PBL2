#ifndef EDITABLE_TABLE_H
#define EDITABLE_TABLE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <memory>

using namespace sf;
using namespace std;

/**
 * @brief Inline-editable table component (SFML 3 compatible)
 * 
 * Features:
 * - Click cell to edit
 * - Scroll support
 * - Row selection
 * - Blue theme matching reference UI
 * - Text created dynamically in render (no Text storage)
 */
class EditableTable {
public:
    struct Cell {
        string value;
        bool isEditable;
        
        Cell(const string& val = "", bool editable = true);
    };
    
private:
    Font& font;  // Reference to avoid loading multiple times
    vector<string> headers;
    vector<int> columnWidths;
    vector<vector<Cell>> rows;
    
    float x, y, width, height;
    float rowHeight = 48.f;
    float headerHeight = 52.f;
    float scrollOffset = 0.f;
    float maxScroll = 0.f;
    float padding = 24.f;
    
    int selectedRow = -1;
    int selectedCol = -1;
    bool isEditing = false;
    string editBuffer;
    Clock cursorBlinkClock;
    int hoveredRow = -1;
    
    // CineXine Theme Colors
    Color bgColor = Color(17, 28, 42);           // #111C2A - darker than sidebar
    Color headerColor = Color(20, 118, 172);     // #1476AC - CineXine blue
    Color rowColor = Color(24, 44, 64);          // #182C40 - row background
    Color borderColor = Color(34, 59, 84);       // #223B54 - subtle border
    Color hoverColor = Color(30, 58, 92);        // #1E3A5C - hover state
    Color selectedColor = Color(40, 80, 120);    // Lighter blue for selection
    Color editingColor = Color(50, 90, 130);     // Even lighter for editing
    Color textColor = Color(240, 240, 240);      // #F0F0F0 - light text
    Color headerTextColor = Color(255, 255, 255); // White for header
    
    RectangleShape background;
    RectangleShape scrollBar;
    RectangleShape scrollThumb;
    bool isDraggingScrollBar = false;
    float scrollBarWidth = 10.f;
    
    // Callbacks
    function<bool(int row, int col, const string& newValue)> onCellChange;
    function<void(int row)> onRowSelect;
    
    void calculateColumnWidths();
    void updateScrollBar();
    FloatRect getCellBounds(int row, int col) const;
    pair<int, int> getCellAtPosition(Vector2f mousePos) const;
    
public:
    EditableTable(Font& font, float x, float y, float width, float height);
    
    void setHeaders(const vector<string>& headers);
    void addRow(const vector<string>& rowData, bool editable = true);
    void clearRows();
    void setColumnWidths(const vector<int>& widths);
    
    void handleEvent(const Event& event, const RenderWindow& window);
    void update(Vector2f mousePos, bool mousePressed);
    void render(RenderWindow& window);
    
    // Callbacks
    void setOnCellChange(function<bool(int, int, const string&)> callback);
    void setOnRowSelect(function<void(int)> callback);
    
    int getSelectedRow() const;
    vector<string> getRowData(int row) const;
    int getRowCount() const;
    
    // Update/delete operations
    void updateRow(int row, const vector<string>& newData);
    void deleteRow(int row);
};

#endif
