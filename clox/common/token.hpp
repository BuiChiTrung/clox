#pragma once

#include <sys/types.h>

#include "clox/common/expr_val.hpp"
#include "clox/utils/magic_enum.hpp"
#include <iomanip>
#include <sstream>
#include <string>

const uint MAX_ARGS_NUM = 255;

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
    ELIF,
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

const std::unordered_map<std::string, TokenType> reserved_kws = {
    {"and", TokenType::AND},     {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},   {"elif", TokenType::ELIF},
    {"false", TokenType::FALSE}, {"for", TokenType::FOR},
    {"fun", TokenType::FUNC},    {"if", TokenType::IF},
    {"nil", TokenType::NIL},     {"or", TokenType::OR},
    {"print", TokenType::PRINT}, {"return", TokenType::RETURN},
    {"super", TokenType::SUPER}, {"this", TokenType::THIS},
    {"true", TokenType::TRUE},   {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
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
        std::ostringstream oss;
        oss << "Token: " << std::setw(15) << std::left
            << magic_enum::enum_name(this->type) << " | Line: " << std::setw(3)
            << this->line << " | Lexeme: " << this->lexeme;
        return oss.str();
    }
};
