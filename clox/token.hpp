#pragma once
#include <memory>
#include <sys/types.h>

#include "clox/utils/magic_enum.hpp"
#include <format>
#include <map>
#include <string>
#include <variant>

const uint MAX_ARGS_NUM = 255;

class LoxCallable;

// std::monostate to present nil in Lox
using ExprVal = std::variant<double, bool, std::string,
                             std::shared_ptr<LoxCallable>, std::monostate>;

inline std::string literal_to_string(const ExprVal &value) {
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            }
            else if constexpr (std::is_same_v<T, double>) {
                if (arg == static_cast<int>(arg)) {
                    return std::to_string(static_cast<int>(arg));
                }
                else {
                    return std::format("{:.2f}", arg);
                }
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, LoxCallable>) {
                return arg.to_string();
            }
            else {
                return "nil";
            }
        },
        value);
}

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    MOD,

    // Operations
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUNC,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    EOS, // end of source code
};

const std::map<std::string, TokenType> reserved_kws = {
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"for", TokenType::FOR},       {"fun", TokenType::FUNC},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE},
};

class Token {
  public:
    TokenType type = TokenType::EOS;
    std::string lexeme = "";
    ExprVal literal = "";
    uint line = 0;

    Token() {}
    Token(TokenType type, std::string lexeme, ExprVal literal, uint line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    std::string toString() {
        return std::format("Token: {:15} | Line: {:3} | Lexeme: {}",
                           magic_enum::enum_name(this->type), this->line,
                           this->lexeme);
    }
};
