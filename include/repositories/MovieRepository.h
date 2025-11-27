#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "data-structures/DLL.h"
#include "utils/StringUtils.h"
#include "utils/FileUtils.h"

/**
 * @brief Model Movie thống nhất cho toàn hệ thống
 * Format: movie_id|title|age_rating|country|language|genres|duration_min|release_date|director|cast|synopsis|poster_path|status
 */
struct Movie {
    std::string id;             // movie_id (VD: "F0001")
    std::string title;          // Tên phim
    std::string ageRating;      // Phân loại tuổi (T13, T16, PG, PG-13, ...)
    std::string country;        // Quốc gia
    std::string language;       // Ngôn ngữ
    std::string genres;         // Thể loại
    std::string duration;       // Thời lượng (phút)
    std::string releaseDate;    // Ngày phát hành
    std::string director;       // Đạo diễn
    std::string cast;           // Diễn viên
    std::string synopsis;       // Mô tả/Synopsis
    std::string posterPath;     // Đường dẫn poster
    std::string status;         // Trạng thái (Đang chiếu, Sắp chiếu, ...)
    
    Movie() = default;
    
    bool isActive() const {
        return status == "Đang chiếu" || status == "active" || status.empty();
    }
    
    int getDurationMinutes() const {
        try {
            return std::stoi(duration);
        } catch (...) {
            return 0;
        }
    }
    
    std::string getDurationFormatted() const {
        int mins = getDurationMinutes();
        if (mins <= 0) return duration;
        int hours = mins / 60;
        int remaining = mins % 60;
        if (hours > 0) {
            return std::to_string(hours) + "h " + std::to_string(remaining) + "m";
        }
        return std::to_string(mins) + " phút";
    }
};

/**
 * @brief Repository thống nhất quản lý Movie
 * 
 * Note: Đây là repository cho Admin CRUD operations.
 * Để hiển thị UI (PosterSlider, DetailScreen), sử dụng models/MovieRepository.h
 * 
 * REFACTORED: Sử dụng DLL thay vì std::vector cho internal storage
 */
class MovieRepository {
private:
    std::string filePath;
    DLL<Movie> movies;  // Changed from std::vector<Movie> to DLL<Movie>
    
    /**
     * @brief Tạo ID movie mới
     */
    std::string generateNewId() const {
        int maxId = 0;
        for (const auto& m : movies) {  // DLL now supports range-based for loop
            if (m.id.length() > 1 && m.id[0] == 'F') {
                try {
                    int id = std::stoi(m.id.substr(1));
                    if (id > maxId) maxId = id;
                } catch (...) {}
            }
        }
        maxId++;
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "F%04d", maxId);
        return std::string(buffer);
    }

public:
    explicit MovieRepository(const std::string& path = "../data/movies.txt")
        : filePath(path) {
        loadFromFile();
    }
    
    // ===== LOAD/SAVE =====
    
    void loadFromFile() {
        movies.clear();
        auto lines = FileUtils::readLines(filePath);
        
        bool isFirst = true;
        for (const auto& line : lines) {
            if (line.empty()) continue;
            
            auto cols = StringUtils::split(line, '|');
            
            if (isFirst && StringUtils::isHeaderRow(cols, "movie_id")) {
                isFirst = false;
                continue;
            }
            isFirst = false;
            
            if (cols.size() >= 13) {
                Movie m;
                m.id = cols[0];
                m.title = cols[1];
                m.ageRating = cols[2];
                m.country = cols[3];
                m.language = cols[4];
                m.genres = cols[5];
                m.duration = cols[6];
                m.releaseDate = cols[7];
                m.director = cols[8];
                m.cast = cols[9];
                m.synopsis = cols[10];
                m.posterPath = cols[11];
                m.status = cols[12];
                movies.push_back(m);
            }
        }
    }
    
    void saveToFile() const {
        std::ofstream file(filePath, std::ios::trunc | std::ios::binary);
        if (!file.is_open()) return;
        
        // UTF-8 BOM
        const char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF };
        file.write(bom, 3);
        
        file << "movie_id|title|age_rating|country|language|genres|duration_min|release_date|director|cast|synopsis|poster_path|status\n";
        
        for (const auto& m : movies) {  // DLL iterator
            file << m.id << "|" << m.title << "|" << m.ageRating << "|"
                 << m.country << "|" << m.language << "|" << m.genres << "|"
                 << m.duration << "|" << m.releaseDate << "|" << m.director << "|"
                 << m.cast << "|" << m.synopsis << "|" << m.posterPath << "|" << m.status << "\n";
        }
        file.close();
    }
    
    void reload() { loadFromFile(); }
    
    // ===== READ OPERATIONS =====
    
    /**
     * @brief Return DLL reference for iteration
     * NOTE: Changed from std::vector to DLL
     */
    const DLL<Movie>& getAll() const { return movies; }
    DLL<Movie>& getAll() { return movies; }

    /**
     * @brief Legacy support: Convert to vector when needed (e.g., for UI components)
     */
    std::vector<Movie> getAllAsVector() const {
        std::vector<Movie> result;
        result.reserve(movies.size());
        for (const auto& m : movies) {
            result.push_back(m);
        }
        return result;
    }
    
    DLL<Movie> getAllAsDLL() const {
        return movies;  // Copy constructor
    }
    
    /**
     * @brief Lấy danh sách phim active
     */
    DLL<Movie> getActiveMovies() const {
        DLL<Movie> result;
        for (const auto& m : movies) {
            if (m.isActive()) {
                result.push_back(m);
            }
        }
        return result;
    }
    
    /**
     * @brief Lấy phim theo thể loại
     */
    DLL<Movie> getByGenre(const std::string& genre) const {
        DLL<Movie> result;
        for (const auto& m : movies) {
            if (m.genres.find(genre) != std::string::npos) {
                result.push_back(m);
            }
        }
        return result;
    }
    
    const Movie* findById(const std::string& id) const {
        for (const auto& m : movies) {
            if (m.id == id) return &m;
        }
        return nullptr;
    }
    
    const Movie* findByTitle(const std::string& title) const {
        for (const auto& m : movies) {
            if (m.title == title) return &m;
        }
        return nullptr;
    }
    
    int count() const { return movies.size(); }
    
    int countActive() const {
        int cnt = 0;
        for (const auto& m : movies) {
            if (m.isActive()) cnt++;
        }
        return cnt;
    }
    
    /**
     * @brief Lấy dữ liệu dạng table cho EditableTable (13 cột)
     * NOTE: Returns std::vector for UI compatibility
     */
    std::vector<std::vector<std::string>> getAllAsTable() const {
        std::vector<std::vector<std::string>> result;
        for (const auto& m : movies) {
            result.push_back({m.id, m.title, m.ageRating, m.country, m.language,
                             m.genres, m.duration, m.releaseDate, m.director,
                             m.cast, m.synopsis, m.posterPath, m.status});
        }
        return result;
    }
    
    std::vector<std::string> getAllTitles() const {
        std::vector<std::string> result;
        for (const auto& m : movies) {
            result.push_back(m.title);
        }
        return result;
    }
    
    // ===== WRITE OPERATIONS =====
    
    /**
     * @brief Thêm phim mới
     */
    bool add(const Movie& movie) {
        Movie m = movie;
        if (m.id.empty()) {
            m.id = generateNewId();
        }
        movies.push_back(m);
        saveToFile();
        return true;
    }
    
    /**
     * @brief Thêm phim từ danh sách cột (13 cột)
     */
    bool addFromRow(const std::vector<std::string>& row) {
        if (row.size() < 13) return false;
        Movie m;
        m.id = row[0].empty() ? generateNewId() : row[0];
        m.title = row[1];
        m.ageRating = row[2];
        m.country = row[3];
        m.language = row[4];
        m.genres = row[5];
        m.duration = row[6];
        m.releaseDate = row[7];
        m.director = row[8];
        m.cast = row[9];
        m.synopsis = row[10];
        m.posterPath = row[11];
        m.status = row[12];
        return add(m);
    }
    
    /**
     * @brief Cập nhật phim theo index
     */
    bool update(int index, const Movie& movie) {
        if (index < 0 || index >= movies.size()) return false;
        movies[index] = movie;
        saveToFile();
        return true;
    }
    
    bool updateFromRow(int index, const std::vector<std::string>& row) {
        if (index < 0 || index >= movies.size()) return false;
        if (row.size() < 13) return false;
        
        Movie& m = movies[index];
        m.id = row[0];
        m.title = row[1];
        m.ageRating = row[2];
        m.country = row[3];
        m.language = row[4];
        m.genres = row[5];
        m.duration = row[6];
        m.releaseDate = row[7];
        m.director = row[8];
        m.cast = row[9];
        m.synopsis = row[10];
        m.posterPath = row[11];
        m.status = row[12];
        
        saveToFile();
        return true;
    }
    
    /**
     * @brief Xóa phim theo index
     */
    bool remove(int index) {
        if (index < 0 || index >= movies.size()) return false;
        movies.removeAt(index);  // DLL method
        saveToFile();
        return true;
    }
    
    bool deleteById(const std::string& id) {
        int index = 0;
        for (const auto& m : movies) {
            if (m.id == id) {
                return remove(index);
            }
            index++;
        }
        return false;
    }
    
    void deleteRecord(int index) { remove(index); }
    
    /**
     * @brief Toggle trạng thái phim
     */
    bool toggleStatus(int index) {
        if (index < 0 || index >= movies.size()) return false;
        Movie& m = movies[index];
        m.status = m.isActive() ? "Ngừng chiếu" : "Đang chiếu";
        saveToFile();
        return true;
    }
};
