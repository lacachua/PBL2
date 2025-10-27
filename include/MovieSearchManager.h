#pragma once
#include <string>
#include <vector>
#include <map>
#include "Movie.h"
#include "Trie.h"

using namespace std;

// Structure to hold search results with movie index
struct SearchResult {
    string title;
    int movieIndex;
};

class MovieSearchManager {
private:
    Trie trie;
    vector<Movie> movies;
    map<string, vector<int>> titleToIndexMap; // Normalized title to movie indices
    
    string normalizeString(const string& str);
    
public:
    MovieSearchManager();
    void loadMovies(const vector<Movie>& movieList);
    vector<SearchResult> searchMovies(const string& query, int limit = 5);
    int getMovieCount() const { return movies.size(); }
};
