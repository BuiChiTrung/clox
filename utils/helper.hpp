#pragma once
#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> split_string(const std::string &str,
                                             char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

// Function to trim leading and trailing spaces
inline std::string strip(const std::string &str) {
    size_t start = 0;
    size_t end = str.size();

    while (start < end &&
           std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }

    while (end > start &&
           std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }

    return str.substr(start, end - start);
}
