#include "clox/common/error_manager.hpp"
#include <iostream>

inline bool ErrorManager::had_static_err = false;
inline bool ErrorManager::had_runtime_err = false;

StaticException::StaticException(std::shared_ptr<Token> tok,
                                 std::string message)
    : message(message), tok(tok) {}

RuntimeException::RuntimeException(std::shared_ptr<Token> tok,
                                   std::string message)
    : message(message), tok(tok) {}

std::string RuntimeException::get_message() const { return message; }

void ErrorManager::handle_scanner_err(uint line, std::string msg) {
    std::cout << "[line " << line << "] Error: " + msg << std::endl;
}

void ErrorManager::handle_runtime_err(const RuntimeException &err) {
    had_runtime_err = true;
    if (err.tok == nullptr) {
        std::cout << "Runtime error: " + err.message << std::endl;
        return;
    }
    report_err(*err.tok, err.message);
}

void ErrorManager::handle_static_err(const StaticException &err) {
    had_static_err = true;
    if (err.tok == nullptr) {
        std::cout << "Static error: " + err.message << std::endl;
        return;
    }
    report_err(*err.tok, err.message);
}

void ErrorManager::report_err(const Token &tok, std::string msg) {
    std::string where = " at '" + tok.lexeme + "'";
    std::cout << "[line " << tok.line << "] Error" + where + ": " + msg
              << std::endl;
}
