#include "error_manager.hpp"
#include "lox.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cout << "Usage: lox [script]" << std::endl;
        exit(1);
    }
    else if (argc == 2) {
        CLox::run_file(argv[1]);
        if (ErrorManager::had_error) {
            exit(1);
        }
    }
    else {
        CLox::run_prompt();
    }
}