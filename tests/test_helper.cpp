#include "../utils/helper.hpp"
#include <iostream>

void test_split_string() {
    std::string input = "a,b,c,d";
    char delimiter = ',';
    std::vector<std::string> expected = {"a", "b", "c", "d"};
    std::vector<std::string> result = split_string(input, delimiter);

    assert(result == expected);
    std::cout << "All test_split_string passed" << std::endl;
}

void test_strip() {
    std::string a = "   a   ";
    std::string b = "\ta\n";
    std::string c = "   \t";

    assert(strip(a) == "a");
    assert(strip(b) == "a");
    assert(strip(c) == "");
    std::cout << "All test_strip passed" << std::endl;
}

int main() {
    test_split_string();
    test_strip();
}