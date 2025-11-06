#pragma once
#include "UI/components/PosterSlider/IPosterProvider.h"
#include "utils/TextFileReader.h"
#include "UI/screens/DetailScreen.h"

class MovieRepository : public IPosterProvider {
private:
    String filePath;
    DLL<MovieDetail> movies;
    int selectedIndex = -1;

    void loadMovies() {
        ifstream file(filePath.toAnsiString());
        if (!file.is_open()) return;

        string line;
        getline(file, line);

        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string token;
            vector<string> cols;
            while (getline(ss, token, '|')) {
                cols.push_back(token);
            }

            if (cols.size() < 13) continue;

            MovieDetail m;
            m.movie_id      = String(cols[0]);
            m.title         = String::fromUtf8(cols[1].begin(), cols[1].end());
            m.age_rating    = String::fromUtf8(cols[2].begin(), cols[2].end());
            m.country       = String::fromUtf8(cols[3].begin(), cols[3].end());
            m.language      = String::fromUtf8(cols[4].begin(), cols[4].end());
            m.genres        = String::fromUtf8(cols[5].begin(), cols[5].end());
            m.duration_min  = String::fromUtf8(cols[6].begin(), cols[6].end());
            m.release_date  = String::fromUtf8(cols[7].begin(), cols[7].end());
            m.director      = String::fromUtf8(cols[8].begin(), cols[8].end());
            m.cast          = String::fromUtf8(cols[9].begin(), cols[9].end());
            m.synopsis      = String::fromUtf8(cols[10].begin(), cols[10].end());
            m.posterPath    = String::fromUtf8(cols[11].begin(), cols[11].end());
            m.status        = String::fromUtf8(cols[12].begin(), cols[12].end());

            movies.push_back(m);
        }
        file.close();
    }
public:
    MovieRepository(const String& path) : filePath(path) 
    {
        loadMovies();
    }

    DLL<String> getPosterPaths() override {
        DLL<String> paths;
        for (int i = 0; i < movies.getSize(); i++)
            paths.push_back(movies[i].posterPath);
        return paths;
    }

    void setSelectedIndex(int index) { 
        selectedIndex = index; 
    }
    int getSelectedIndex() const { 
        return selectedIndex; 
    }

    MovieDetail getMovieDetailbyIndex(int index) const {
        if (index >= 0 && index < (int)movies.getSize())
            return movies[index];
        return MovieDetail();
    }
}; 
