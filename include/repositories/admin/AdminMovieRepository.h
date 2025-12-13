#ifndef ADMIN_MOVIE_REPOSITORY_H
#define ADMIN_MOVIE_REPOSITORY_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

/**
 * @brief Repository để xử lý file movies.txt cho Admin Panel
 * 
 * Note: Đây là tầng data-access (repositories). UI panel chỉ nên gọi service,
 * và service gọi repository này.
 */
class AdminMovieRepository {
private:
    string filePath;
    vector<vector<string>> data;

    vector<string> splitString(const string& str, char delimiter);
    string generateNewId();

public:
    AdminMovieRepository(const string& path);

    void loadFromFile();
    void saveToFile();

    void addRecord(const vector<string>& record);
    void updateRecord(int index, const vector<string>& record);
    void deleteRecord(int index);

    vector<vector<string>> getAllData() const;
    vector<string> getRecord(int index) const;
    int getRecordCount() const;
};

#endif
