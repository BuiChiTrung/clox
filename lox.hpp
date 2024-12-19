#pragma once
#include "error_manager.hpp"
#include "scanner.hpp"
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

class CLox {
  public:
    // Process one line or a whole file
    static void run(std::string source) {
        Scanner *scanner = new Scanner(source);
        std::vector<Token> tokens = scanner->scan_tokens();

        for (Token token : tokens) {
            std::cout << token.toString() << std::endl;
        }
    }

    static void run_file(std::string path) {
        ErrorManager::had_error = false;
        std::ifstream file(path);
        std::string content;

        if (file.is_open()) {
            // Read entire file into a string
            content.assign((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

            run(content);
            file.close();
        }
        else {
            std::cerr << "Unable to open file" << std::endl;
        }
    }

    static void run_prompt() {
        ErrorManager::had_error = false;
        std::string line;

        std::cout << "Enter lines of text (Ctrl+D or Ctrl+Z to end):"
                  << std::endl;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            std::cout << "> ";
            run(line);
        }
    }
};
