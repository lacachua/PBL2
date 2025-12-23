#pragma once
#include <string>
#include <SFML/System/String.hpp>

using namespace std;
using namespace sf;

/**
 * @brief Model đại diện cho một bộ phim trong hệ thống
 * 
 * Chứa toàn bộ thông tin chi tiết của phim.
 * Được sử dụng bởi MovieRepository, DetailScreen, BookingScreen.
 */
struct Movie {
    string id;              // ID phim (VD: "M001")
    string title;           // Tên phim
    string ageRating;       // Giới hạn độ tuổi (VD: "13+", "18+")
    string country;         // Quốc gia sản xuất
    string language;        // Ngôn ngữ
    string genres;          // Thể loại (VD: "Hành động, Phiêu lưu")
    int durationMin = 0;    // Thời lượng (phút)
    string releaseDate;     // Ngày khởi chiếu (dd/mm/yyyy)
    string director;        // Đạo diễn
    string cast;            // Diễn viên
    string synopsis;        // Mô tả nội dung
    string posterPath;      // Đường dẫn poster
    string status;          // Trạng thái ("Đang chiếu", "Sắp chiếu", "Ngừng chiếu")
    
    // ===== CONSTRUCTORS =====
    
    Movie() = default;
    
    Movie(const string& id, const string& title, int duration, const string& status)
        : id(id), title(title), durationMin(duration), status(status) {}
    
    // ===== HELPER METHODS =====
    
    /**
     * @brief Kiểm tra phim có đang chiếu không
     */
    bool isNowShowing() const {
        return status.find("Đang chiếu") != string::npos || 
               status.find("chiếu") != string::npos;
    }
    
    /**
     * @brief Kiểm tra phim sắp chiếu
     */
    bool isComingSoon() const {
        return status.find("Sắp chiếu") != string::npos;
    }
    
    /**
     * @brief Lấy thời lượng dạng chuỗi "XXX phút"
     */
    string getDurationString() const {
        return to_string(durationMin) + " phút";
    }
    
    /**
     * @brief Chuyển thành sf::String để hiển thị UTF-8
     */
    String getTitleSf() const {
        return String::fromUtf8(title.begin(), title.end());
    }
    
    String getGenresSf() const {
        return String::fromUtf8(genres.begin(), genres.end());
    }
    
    String getDirectorSf() const {
        return String::fromUtf8(director.begin(), director.end());
    }
    
    String getCastSf() const {
        return String::fromUtf8(cast.begin(), cast.end());
    }
    
    String getSynopsisSf() const {
        return String::fromUtf8(synopsis.begin(), synopsis.end());
    }
    
    String getStatusSf() const {
        return String::fromUtf8(status.begin(), status.end());
    }
};

/**
 * @brief Backwards compatible alias cho MovieDetail (dùng trong code cũ)
 * 
 * Code mới nên sử dụng Movie struct.
 * MovieDetail được giữ lại để tương thích với DetailScreen, HomeScreen, etc.
 */
struct MovieDetail {
    String movie_id;
    String title;
    String age_rating;
    String country;
    String language;
    String genres;
    String duration_min;
    String release_date;
    String director;
    String cast;
    String synopsis;
    String posterPath;
    String status;
    
    // Conversion constructor from Movie
    MovieDetail() = default;
    
    MovieDetail(const Movie& m) {
        movie_id = String(m.id);
        title = String::fromUtf8(m.title.begin(), m.title.end());
        age_rating = String::fromUtf8(m.ageRating.begin(), m.ageRating.end());
        country = String::fromUtf8(m.country.begin(), m.country.end());
        language = String::fromUtf8(m.language.begin(), m.language.end());
        genres = String::fromUtf8(m.genres.begin(), m.genres.end());
        duration_min = String(to_string(m.durationMin));
        release_date = String::fromUtf8(m.releaseDate.begin(), m.releaseDate.end());
        director = String::fromUtf8(m.director.begin(), m.director.end());
        cast = String::fromUtf8(m.cast.begin(), m.cast.end());
        synopsis = String::fromUtf8(m.synopsis.begin(), m.synopsis.end());
        posterPath = String::fromUtf8(m.posterPath.begin(), m.posterPath.end());
        status = String::fromUtf8(m.status.begin(), m.status.end());
    }
    
    // Conversion to Movie
    Movie toMovie() const {
        Movie m;
        m.id = movie_id.toAnsiString();
        m.title = title.toAnsiString();
        m.ageRating = age_rating.toAnsiString();
        m.country = country.toAnsiString();
        m.language = language.toAnsiString();
        m.genres = genres.toAnsiString();
        try {
            m.durationMin = stoi(duration_min.toAnsiString());
        } catch (...) {
            m.durationMin = 120;
        }
        m.releaseDate = release_date.toAnsiString();
        m.director = director.toAnsiString();
        m.cast = cast.toAnsiString();
        m.synopsis = synopsis.toAnsiString();
        m.posterPath = posterPath.toAnsiString();
        m.status = status.toAnsiString();
        return m;
    }
};
