#include "clox/error_manager/error_manager.hpp"
#include <iostream>

inline bool ErrorManager::had_static_err = false;
inline bool ErrorManager::had_runtime_err = false;

void ErrorManager::report_error(uint line, std::string where, std::string msg) {
    std::cout << "[line " << line << "] Error" + where + ": " + msg
              << std::endl;
    had_static_err = true;
}

void ErrorManager::handle_err(uint line, std::string msg) {
    report_error(line, "", msg);
}

void ErrorManager::handle_err(std::shared_ptr<Token> tok, std::string msg) {
    report_error(tok->line, " at '" + tok->lexeme + "'", msg);
}

void ErrorManager::handle_runtime_err(const RuntimeException &err) {
    had_runtime_err = true;
    handle_err(err.tok, err.message);
}

void ErrorManager::handle_parser_err(const ParserException &err) {
    handle_err(err.tok, err.message);
}

ParserException::ParserException(std::shared_ptr<Token> tok,
                                 const std::string &message)
    : message(message), tok(tok) {}

RuntimeException::RuntimeException(std::shared_ptr<Token> tok,
                                   const std::string &message)
    : message(message), tok(tok) {}

const char *RuntimeException::what() const noexcept {
    // Construct the error message dynamically
    formattedMessage = message + " at line " + std::to_string(tok->line) +
                       ", near '" + tok->lexeme + "'";
    return formattedMessage.c_str();
}