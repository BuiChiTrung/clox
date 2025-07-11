#pragma once
#include <string>
#include <vector>

std::vector<std::string> split_string(const std::string &str, char delimiter);

// Function to trim leading and trailing spaces
std::string strip(const std::string &str);

std::string double_to_string(double num);

void smart_pointer_no_op_deleter(void *);

bool is_double_int(double num);