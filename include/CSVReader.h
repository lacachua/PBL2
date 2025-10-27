#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

class CSVReader {
public:
    // Đọc một field từ CSV, xử lý cả trường hợp có quotes và dấu phẩy bên trong
    static string parseCSVField(stringstream& ss);
    
    // Chuẩn hóa string: loại bỏ khoảng trắng thừa ở đầu và cuối
    static string trim(const string& str);
    
    // Chuẩn hóa string: loại bỏ khoảng trắng thừa giữa các từ
    static string normalizeSpaces(const string& str);
    
    // Đọc một dòng CSV và trả về vector các field đã được chuẩn hóa
    static vector<string> parseLine(const string& line, bool normalize = true);
    
    // Kiểm tra file có tồn tại không
    static bool fileExists(const string& filepath);
    
    // Đọc toàn bộ file CSV và trả về vector các dòng (mỗi dòng là vector các field)
    static vector<vector<string>> readCSV(const string& filepath, bool skipHeader = true);
};
