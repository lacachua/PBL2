#pragma once
#include <string>
#include <vector>

namespace models {

/**
 * @brief Movie entity - Represents a film
 * POCO - Pure data structure
 */
class Movie {
private:
    std::string filmId;
    std::string title;
    std::string ageRating;
    std::string country;
    std::string language;
    std::string genres;
    int durationMin;
    std::string releaseDate;
    std::string director;
    std::string cast;
    std::string synopsis;
    std::string posterPath;
    std::string status;  // "Now Showing", "Coming Soon", "Ended"
    
public:
    Movie() : durationMin(0) {}
    
    Movie(const std::string& id, const std::string& title)
        : filmId(id), title(title), durationMin(0) {}
    
    // Getters
    std::string getFilmId() const { return filmId; }
    std::string getTitle() const { return title; }
    std::string getAgeRating() const { return ageRating; }
    std::string getCountry() const { return country; }
    std::string getLanguage() const { return language; }
    std::string getGenres() const { return genres; }
    int getDurationMin() const { return durationMin; }
    std::string getReleaseDate() const { return releaseDate; }
    std::string getDirector() const { return director; }
    std::string getCast() const { return cast; }
    std::string getSynopsis() const { return synopsis; }
    std::string getPosterPath() const { return posterPath; }
    std::string getStatus() const { return status; }
    
    // Setters
    void setFilmId(const std::string& id) { filmId = id; }
    void setTitle(const std::string& t) { title = t; }
    void setAgeRating(const std::string& rating) { ageRating = rating; }
    void setCountry(const std::string& c) { country = c; }
    void setLanguage(const std::string& lang) { language = lang; }
    void setGenres(const std::string& g) { genres = g; }
    void setDurationMin(int duration) { durationMin = duration; }
    void setReleaseDate(const std::string& date) { releaseDate = date; }
    void setDirector(const std::string& dir) { director = dir; }
    void setCast(const std::string& c) { cast = c; }
    void setSynopsis(const std::string& syn) { synopsis = syn; }
    void setPosterPath(const std::string& path) { posterPath = path; }
    void setStatus(const std::string& s) { status = s; }
    
    // Business logic
    bool isNowShowing() const {
        return status == "Now Showing" || status == "Đang chiếu";
    }
    
    bool isComingSoon() const {
        return status == "Coming Soon" || status == "Sắp chiếu";
    }
    
    // Validation
    bool isValid() const {
        return !filmId.empty() && 
               !title.empty() &&
               durationMin > 0;
    }
    
    // Get numeric movie ID from film_id (e.g., "F0001" → 1)
    int getNumericId() const {
        if (filmId.length() > 1 && filmId[0] == 'F') {
            try {
                return std::stoi(filmId.substr(1));
            } catch (...) {
                return 0;
            }
        }
        return 0;
    }
};

} // namespace models
