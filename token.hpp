#pragma once
#include <map>
#include <sys/types.h>

enum TokenType {
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
  FUN,
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
};

const std::map<std::string, TokenType> reserved_kws = {
    {"and", AND},   {"class", CLASS}, {"else", ELSE},     {"false", FALSE},
    {"for", FOR},   {"fun", FUN},     {"if", IF},         {"nil", NIL},
    {"or", OR},     {"print", PRINT}, {"return", RETURN}, {"super", SUPER},
    {"this", THIS}, {"true", TRUE},   {"var", VAR},       {"while", WHILE},
};

class Token {
private:
  TokenType type;
  std::string lexeme;
  std::string literal;
  uint line;

public:
  Token(TokenType type, std::string lexeme, std::string literal, uint line) {
    this->type = type;
    this->lexeme = lexeme;
    this->literal = literal;
    this->line = line;
  }

  std::string toString() {
    std::string s = "";
    s += this->type;
    s += " ";
    s += this->lexeme;
    s += " ";
    s += this->literal;
    s += " ";
    s += std::to_string(this->line);
    return s;
  }
};
