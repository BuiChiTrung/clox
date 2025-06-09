#include "helper.hpp"
#include <iomanip>
#include <sstream>

std::vector<std::string> split_string(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

// Function to trim leading and trailing spaces
std::string strip(const std::string &str) {
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

std::string double_to_string(double num) {
    if (num == static_cast<int>(num)) {
        return std::to_string(static_cast<int>(num));
    } else {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << num;
        return oss.str();
    }
}

void smart_pointer_no_op_deleter(void *) {}

bool is_double_int(double num) {
    return std::floor(num) == num && std::isfinite(num);
}