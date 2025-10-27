#include "Movie.h"

vector<Movie> loadMoviesFromCSV(const string& filePath) {
    vector<Movie> movies;
    
    // Sử dụng CSVReader để đọc file
    vector<vector<string>> csvData = CSVReader::readCSV(filePath, true);
    
    if (csvData.empty()) {
        cerr << "❌ Không có dữ liệu phim trong file CSV!" << endl;
        return movies;
    }
    
    // Parse từng dòng thành Movie object
    for (const auto& row : csvData) {
        // Kiểm tra số lượng cột (phải có đủ 13 cột)
        if (row.size() < 13) {
            cerr << "⚠️ Dòng không đủ dữ liệu (cần 13 cột, có " << row.size() << " cột)" << endl;
            continue;
        }
        
        Movie m;
        // CSV format: film_id,title,age_rating,country,language,genres,duration_min,release_date,director,cast,synopsis,poster_path,status
        m.film_id = row[0];
        m.title = row[1];
        m.age_rating = row[2];
        m.country = row[3];
        m.language = row[4];
        m.genres = row[5];
        m.duration_min = row[6];
        m.release_date = row[7];
        m.director = row[8];
        m.cast = row[9];
        m.synopsis = row[10];
        m.poster_path = row[11];
        m.status = row[12];
        
        movies.push_back(m);
    }
    
    return movies;
}

// Hàm lấy danh sách poster paths từ CSV (theo đúng thứ tự trong file)
vector<string> getMoviePosterPaths(const string& filePath) {
    vector<string> posterPaths;
    
    vector<vector<string>> csvData = CSVReader::readCSV(filePath, true);
    
    for (const auto& row : csvData) {
        if (row.size() >= 12) {  // Cột poster_path là cột thứ 12 (index 11)
            posterPaths.push_back(row[11]);
        }
    }
    return posterPaths;
}