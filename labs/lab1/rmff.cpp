#include <vector>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

using namespace std;

using matrix = std::vector<std::vector<int>>;

std::string trimWhitespace(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

matrix readMatrixFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("File dont open: " + filename);
    
    matrix result;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        std::string trimmed = trimWhitespace(line);
        
        std::vector<int> row;
        std::stringstream ss(trimmed);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            std::string cleaned = trimWhitespace(token);
            size_t pos;
            int value = std::stoi(cleaned, &pos);      
            row.push_back(value);
        }
        
        if (!row.empty()) result.push_back(row);
    }

    file.close();
    return result;
}
