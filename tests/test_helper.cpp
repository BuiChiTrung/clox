#include "../utils/helper.hpp"
#include <iostream>

void test_split_string() {
    std::string a = "a,b,c,d";
    std::vector<std::string> toks = split_string(a, ',');

    for (auto tok : toks) {
        std::cout << tok << " ";
    }
    std::cout << std::endl;
    std::cout << a;
}

int main() { test_split_string(); }