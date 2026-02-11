#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

using matrix = std::vector<std::vector<int>>;

/**
 * Удаляет пробельные символы из строки
 */
std::string trimWhitespace(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * Читает матрицу из файла
 * Формат: числа разделены запятыми, допустимы пробелы и запятые в конце строк
 * Пример:
 *  0, 16,  0, 27,  0,  0,  0,
 * 16,  0,  0,  0,  0,  0,  0,
 *  ...
 */
matrix readMatrixFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("File dont open: " + filename);
    }
    
    matrix result;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        std::string trimmed = trimWhitespace(line);
        
        // // Пропускаем пустые строки
        // if (trimmed.empty()) {
        //     continue;
        // }
        
        std::vector<int> row;
        std::stringstream ss(trimmed);
        std::string token;
        
        // Разбиваем строку по запятым
        while (std::getline(ss, token, ',')) {
            std::string cleaned = trimWhitespace(token);
            
            // Пропускаем пустые токены (возникают из-за запятых в конце строки)
            if (cleaned.empty()) {
                continue;
            }
            
            // Преобразуем в число
            try {
                size_t pos;
                int value = std::stoi(cleaned, &pos);
                
                // Проверяем, что всё содержимое токена было распознано как число
                if (pos != cleaned.length()) {
                    throw std::invalid_argument("лишние символы после числа");
                }
                
                row.push_back(value);
            } catch (const std::exception& e) {
                throw std::runtime_error(
                    "Ошибка парсинга числа в строке " + std::to_string(lineNumber) + 
                    ": '" + cleaned + "' (" + e.what() + ")"
                );
            }
        }
        
        if (!row.empty()) {
            result.push_back(row);
        }
    }
    
    file.close();
    
    return result;
}
