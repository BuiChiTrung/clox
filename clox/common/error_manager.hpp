
#pragma once
#include "clox/common/token.hpp"
#include <exception>

class RuntimeException : public std::exception {
  private:
    mutable std::string
        formattedMessage; // Store the full formatted error message
    std::string message;
    std::shared_ptr<Token> tok;

  public:
    RuntimeException(std::shared_ptr<Token> tok, std::string message);
    const char *what() const noexcept override;
    friend class ErrorManager;
};

class ParserException : std::exception {
  private:
    std::string message;
    std::shared_ptr<Token> tok;

  public:
    ParserException(std::shared_ptr<Token> tok, std::string message);
    friend class ErrorManager;
};

class ErrorManager {
  private:
    static void report_error(uint line, std::string where, std::string msg);

  public:
    static bool had_static_err;
    static bool had_runtime_err;

    static void handle_err(uint line, std::string msg);
    static void handle_err(const Token &tok, std::string msg);
    static void handle_runtime_err(const RuntimeException &err);
    static void handle_parser_err(const ParserException &e);
};
