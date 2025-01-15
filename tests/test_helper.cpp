#include "clox/utils/helper.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(TestHelper, SplitString) {
    std::string input = "a,b,c,d";
    char delimiter = ',';
    std::vector<std::string> expected = {"a", "b", "c", "d"};
    std::vector<std::string> result = split_string(input, delimiter);

    EXPECT_EQ(result, expected);
}

TEST(TestHelper, Strip) {
    std::string a = "   a   ";
    std::string b = "\ta\n";
    std::string c = "   \t";

    EXPECT_EQ(strip(a), "a");
    EXPECT_EQ(strip(b), "a");
    EXPECT_EQ(strip(c), "");
}
