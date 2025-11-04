#pragma once
#include "core/IPosterProvider.h"
#include "utils/TextFileReader.h"

class MovieRepository : public IPosterProvider {
private:
    String filePath;
public:
    MovieRepository(const String& path) : filePath(path) {}

    DLL<String> getPosterPaths() override {
        TextFileReader reader;
        DLL<String> paths;
        if (reader.loadFile(filePath)) {
            for (int i = 0; i < reader.getRowCount(); i++)
                paths.push_back(reader.getValue(i, 11));
        }
        return paths;
    }
};
