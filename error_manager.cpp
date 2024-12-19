#include "error_manager.hpp"

bool ErrorManager::had_error = false;
// cd "/Users/straw/Projects/build-from-scratch/clox/" && g++ main.cpp
// scanner.cpp error_manager.cpp -std=c++17 -o main &&
// "/Users/straw/Projects/build-from-scratch/clox/"main && rm
// "/Users/straw/Projects/build-from-scratch/clox/"main