#include "error_manager.hpp"
#include <format>
#include <iostream>

inline bool ErrorManager::had_err = false;
inline bool ErrorManager::had_runtime_err = false;

void ErrorManager::report_error(uint line, std::string where, std::string msg) {
    std::cout << "[line " << line << "] Error" + where + ": " + msg
              << std::endl;
    had_err = true;
}

void ErrorManager::handle_err(uint line, std::string msg) {
    report_error(line, "", msg);
}

void ErrorManager::handle_err(std::shared_ptr<Token> tok, std::string msg) {
    report_error(tok->line, " at '" + tok->lexeme + "'", msg);
}

void ErrorManager::handle_runtime_err(const RuntimeException &err) {
    had_runtime_err = true;
    std::cout << err.what();
}

void ErrorManager::handle_parser_err(const ParserException &e) {
    handle_err(e.tok, e.message);
}

ParserException::ParserException(std::shared_ptr<Token> tok,
                                 const std::string &message)
    : message(message), tok(tok) {}
const char *ParserException::what() const noexcept {
    // Construct the error message dynamically
    formattedMessage = std::format("{} at line {}, near '{}'", message,
                                   tok->line, tok->lexeme);
    return formattedMessage.c_str();
}

RuntimeException::RuntimeException(std::shared_ptr<Token> tok,
                                   const std::string &message)
    : message(message), tok(tok) {}

const char *RuntimeException::what() const noexcept {
    // Construct the error message dynamically
    formattedMessage = std::format("{} at line {}, near '{}'", message,
                                   tok->line, tok->lexeme);
    return formattedMessage.c_str();
}