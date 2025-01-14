#pragma once
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "token.hpp"
#include <memory>

class Parser {
  public:
    Parser(std::vector<std::shared_ptr<Token>> tokens);

    std::vector<std::shared_ptr<Stmt>> parse_program();
    std::shared_ptr<Expr> parse_single_expr();

  private:
    std::vector<std::shared_ptr<Token>> tokens;
    uint32_t current_tok_pos = 0;

    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<Stmt> parse_print_stmt();
    std::shared_ptr<Stmt> parse_expr_stmt();
    std::shared_ptr<Expr> parse_expr();
    std::shared_ptr<Expr> parse_logic_expr();
    std::shared_ptr<Expr> parse_equality_expr();
    std::shared_ptr<Expr> parse_comparision_expr();
    std::shared_ptr<Expr> parse_term();
    std::shared_ptr<Expr> parse_factor();
    std::shared_ptr<Expr> parse_unary();
    std::shared_ptr<Expr> parse_primary();

    bool consumed_all_tokens();
    bool validate_token_and_advance(std::vector<TokenType> tok_types);
    std::shared_ptr<Token> get_prev_tok();
    std::shared_ptr<Token> advance();
    std::shared_ptr<Token> get_cur_tok();
    std::shared_ptr<Token> validate_and_throw_err(TokenType type,
                                                  std::string msg);
};
