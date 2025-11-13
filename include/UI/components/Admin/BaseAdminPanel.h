#ifndef BASE_ADMIN_PANEL_H
#define BASE_ADMIN_PANEL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "UI/components/Admin/EditableTable.h"
#include "UI/components/Button.h"

using namespace sf;
using namespace std;

/**
 * @brief Base class cho tất cả Admin CRUD Panels
 * 
 * Provides common functionality:
 * - EditableTable
 * - Action buttons (Add, Save, Delete, Reload)
 * - Message display (success/error)
 * - Virtual methods for subclasses to implement
 */
class BaseAdminPanel {
protected:
    float width;
    float height;
    Vector2f position;
    
    // Title
    string title;
    Font font;
    Text titleText;
    
    // Table
    unique_ptr<EditableTable> table;
    vector<string> columnHeaders;
    
    // Buttons
    vector<unique_ptr<Button>> buttons;
    
    // Message
    Text messageText;
    Clock messageClock;
    bool showMessage;
    bool isMessageSuccess;
    
    // Colors
    Color bgColor;
    Color titleColor;
    Color successColor;
    Color errorColor;
    
    // Data file path
    string dataFilePath;
    
    // Virtual methods for subclasses
    virtual void loadDataFromFile() = 0;
    virtual void saveDataToFile() = 0;
    virtual vector<string> createEmptyRow() = 0;
    virtual bool validateRow(const vector<string>& row) = 0;
    
    void setupUI();
    void showSuccessMessage(const string& msg);
    void showErrorMessage(const string& msg);
    
public:
    BaseAdminPanel(float w, float h, const string& title, 
                   const vector<string>& headers, const string& filePath);
    virtual ~BaseAdminPanel() = default;
    
    void setPosition(Vector2f pos);
    
    virtual void handleEvent(const Event& event, const RenderWindow& window);
    virtual void update();
    virtual void render(RenderWindow& window);
    
    // Actions
    void onAddRow();
    void onSaveData();
    void onDeleteRow();
    void onReloadData();
};

#endif
