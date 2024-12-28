#pragma once
#include "ast/expr.hpp"
#include "token.hpp"
#include <exception>
#include <memory>

class Parser {
  public:
    Parser(std::vector<std::shared_ptr<Token>> tokens);

    std::shared_ptr<Expr> parse();
    class Exception : std::exception {};

  private:
    std::vector<std::shared_ptr<Token>> tokens;
    uint32_t current_tok_pos = 0;
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparision();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();

    bool consumed_all_tokens();
    bool match(std::vector<TokenType> tok_types);
    bool check(TokenType tok_type);
    std::shared_ptr<Token> previous_tok();
    std::shared_ptr<Token> advance();
    std::shared_ptr<Token> peek();
    std::shared_ptr<Token> consume(TokenType type, std::string msg);
    std::shared_ptr<Exception> error(std::shared_ptr<Token> tok,
                                     std::string msg);
};
