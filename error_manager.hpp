
#pragma once
#include <iostream>

class ErrorManager {
  public:
    static bool had_error;

    static void report_error(uint line, std::string where,
                             std::string message) {
        std::cout << "[line " << line << "] Error" + where + ": " + message
                  << std::endl;
        // had_error = true;
    }

    static void err(uint line, std::string message) {
        report_error(line, "", message);
    }
};