#include "CSVReader.h"

// Hàm parse một field từ CSV, xử lý quotes và dấu phẩy
string CSVReader::parseCSVField(stringstream& ss) {
    string field;
    char ch;
    bool inQuotes = false;
    
    while (ss.get(ch)) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ',' && !inQuotes) {
            break;
        } else {
            field += ch;
        }
    }
    
    return field;
}

// Chuẩn hóa string: loại bỏ khoảng trắng thừa ở đầu và cuối
string CSVReader::trim(const string& str) {
    if (str.empty()) return str;
    
    size_t start = 0;
    size_t end = str.length() - 1;
    
    // Tìm vị trí ký tự đầu tiên không phải khoảng trắng
    while (start <= end && isspace(str[start])) {
        start++;
    }
    
    // Tìm vị trí ký tự cuối cùng không phải khoảng trắng
    while (end >= start && isspace(str[end])) {
        end--;
    }
    
    if (start > end) return "";
    
    return str.substr(start, end - start + 1);
}

// Chuẩn hóa string: loại bỏ khoảng trắng thừa giữa các từ
string CSVReader::normalizeSpaces(const string& str) {
    string result;
    bool lastWasSpace = false;
    
    for (char ch : str) {
        if (isspace(ch)) {
            if (!lastWasSpace && !result.empty()) {
                result += ' ';
                lastWasSpace = true;
            }
        } else {
            result += ch;
            lastWasSpace = false;
        }
    }
    
    return trim(result);
}

// Đọc một dòng CSV và trả về vector các field
vector<string> CSVReader::parseLine(const string& line, bool normalize) {
    vector<string> fields;
    stringstream ss(line);
    
    while (ss.peek() != EOF) {
        string field = parseCSVField(ss);
        
        // Nếu đây là field cuối cùng và không có dấu phẩy
        if (ss.peek() == EOF && !field.empty()) {
            // Không cần làm gì thêm
        }
        
        // Chuẩn hóa field nếu được yêu cầu
        if (normalize) {
            field = normalizeSpaces(field);
        } else {
            field = trim(field);
        }
        
        fields.push_back(field);
    }
    return fields;
}

bool CSVReader::fileExists(const string& filepath) {
    ifstream file(filepath);
    return file.good();
}

vector<vector<string>> CSVReader::readCSV(const string& filepath, bool skipHeader) {
    vector<vector<string>> data;
    if (!fileExists(filepath)) 
        return data;
    
    ifstream file(filepath);
    string line;
    
    // Bỏ qua header nếu được yêu cầu
    if (skipHeader && getline(file, line)) 
        cout << "📋 Header: " << line << endl;
    int lineNumber = skipHeader ? 2 : 1;
    
    // Đọc từng dòng
    while (getline(file, line)) {
        if (line.empty()) {
            lineNumber++;
            continue;
        }
        vector<string> fields = parseLine(line, true);
        if (!fields.empty()) 
            data.push_back(fields);
        lineNumber++;
    }
    file.close();
    return data;
}
