#pragma once
#include <string>
#include <map>

class MoviesRepository {
public:
    /**
     * Get movie title by movie_id from movies.csv
     * Returns cached result if already loaded
     * 
     * @param path Path to movies.csv
     * @param movie_id Movie ID to search
     * @return Movie title or "Unknown Movie" if not found
     */
    static std::string getTitleById(const std::string& path, int movie_id);
    
    /**
     * Clear cache (useful for testing)
     */
    static void clearCache();

private:
    // Cache: movie_id -> movie_title
    static std::map<int, std::string> titleCache;
    static bool cacheLoaded;
    
    // Load all movies into cache
    static void loadCache(const std::string& path);
};
