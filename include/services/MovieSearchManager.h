#pragma once
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include "data-structures/DLL.h"
#include "../include/data-structures/Trie.h"

using namespace std;
using namespace sf;

// Forward declaration to avoid circular dependency
struct MovieDetail {
    String movie_id;
    wstring title, age_rating, country, language, genres, duration_min, release_date, director, cast, synopsis, posterPath, status;
};

// Structure to hold search results with movie index
struct SearchResult {
    string title;
    int movieIndex;
};

class MovieSearchManager {
private:
    Trie trie;
    DLL<MovieDetail> movies;
    map<string, vector<int>> titleToIndexMap;
    
    string normalizeString(const string& str);
    
public:
    MovieSearchManager();
    void loadMovies(const DLL<MovieDetail>& movieList);  // ✅ Thay đổi parameter
    vector<SearchResult> searchMovies(const string& query, int limit = 5);
    int getMovieCount() const { return movies.getSize(); }  // ✅ Đổi từ size() sang getSize()
};
