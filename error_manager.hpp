
#pragma once
#include "token.hpp"
#include <iostream>

class ErrorManager {
  private:
    static void report_error(uint line, std::string where, std::string msg) {
        std::cout << "[line " << line << "] Error" + where + ": " + msg
                  << std::endl;
        had_error = true;
    }

  public:
    static bool had_error;

    static void err(uint line, std::string msg) { report_error(line, "", msg); }

    static void err(std::shared_ptr<Token> tok, std::string msg) {
        report_error(tok->line, " at '" + tok->lexeme + "'", msg);
    }
};

inline bool ErrorManager::had_error = false;