
#pragma once
#include "clox/scanner/token.hpp"
#include <exception>

class RuntimeException : public std::exception {
  private:
    mutable std::string
        formattedMessage; // Store the full formatted error message

  public:
    std::string message;
    std::shared_ptr<Token> tok;
    RuntimeException(std::shared_ptr<Token> tok, const std::string &message);
    const char *what() const noexcept override;
};

class ParserException : std::exception {
  public:
    std::string message;
    std::shared_ptr<Token> tok;
    ParserException(std::shared_ptr<Token> tok, const std::string &message);
};

class ErrorManager {
  private:
    static void report_error(uint line, std::string where, std::string msg);

  public:
    static bool had_err;
    static bool had_runtime_err;

    static void handle_err(uint line, std::string msg);
    static void handle_err(std::shared_ptr<Token> tok, std::string msg);
    static void handle_runtime_err(const RuntimeException &err);
    static void handle_parser_err(const ParserException &e);
};
