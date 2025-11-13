#ifndef ADMIN_MOVIE_REPOSITORY_H
#define ADMIN_MOVIE_REPOSITORY_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

/**
 * @brief Repository để xử lý file movies.txt cho Admin Panel
 */
class AdminMovieRepository {
private:
    string filePath;
    vector<vector<string>> data;
    
    vector<string> splitString(const string& str, char delimiter);
    string generateNewId();
    
public:
    AdminMovieRepository(const string& path);
    
    // Load/Save
    void loadFromFile();
    void saveToFile();
    
    // CRUD operations
    void addRecord(const vector<string>& record);
    void updateRecord(int index, const vector<string>& record);
    void deleteRecord(int index);
    
    // Getters
    vector<vector<string>> getAllData() const { return data; }
    vector<string> getRecord(int index) const;
    int getRecordCount() const { return data.size(); }
};

#endif
