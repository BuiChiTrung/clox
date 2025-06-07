
#pragma once
#include "clox/common/token.hpp"
#include <exception>

class RuntimeException : public std::exception {
  private:
    std::string message;
    std::shared_ptr<Token> tok;

  public:
    RuntimeException(std::shared_ptr<Token> tok, std::string message);
    std::string get_message() const;
    friend class ErrorManager;
};

class StaticException : std::exception {
  private:
    std::string message;
    std::shared_ptr<Token> tok;

  public:
    StaticException(std::shared_ptr<Token> tok, std::string message);
    friend class ErrorManager;
};

class ErrorManager {
  private:
    static void report_err(const Token &tok, std::string msg);

  public:
    static bool had_static_err;
    static bool had_runtime_err;

    static void handle_scanner_err(uint line, std::string msg);
    static void handle_runtime_err(const RuntimeException &err);
    static void handle_static_err(const StaticException &e);
};
