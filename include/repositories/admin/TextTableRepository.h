#pragma once

#include <string>
#include <vector>

class TextTableRepository {
public:
    // Reads all non-empty lines in file. If file can't be opened, returns empty.
    std::vector<std::string> readAllLines(const std::string& path) const;

    // Reads data lines (skips the first line/header). If file has < 2 lines, returns empty.
    std::vector<std::string> readDataLinesSkipHeader(const std::string& path) const;

    // Truncates file and writes optional header + data lines.
    void writeDataLines(const std::string& path,
                        const std::string& headerLine,
                        const std::vector<std::string>& dataLines) const;

    // Appends data lines. If file doesn't exist or is empty and headerLine != "", writes header first.
    void appendDataLines(const std::string& path,
                         const std::string& headerLine,
                         const std::vector<std::string>& dataLines) const;
};
