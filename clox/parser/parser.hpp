#pragma once
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include <memory>

class Parser {
  public:
    Parser(std::vector<std::shared_ptr<Token>> tokens);

    std::vector<std::shared_ptr<Stmt>> parse_program();
    std::shared_ptr<Expr> parse_single_expr();

  private:
    std::vector<std::shared_ptr<Token>> tokens;
    uint32_t current_tok_pos = 0;

    std::shared_ptr<Stmt> parse_declaration();
    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<Stmt> parse_return_stmt();
    std::shared_ptr<Stmt> parse_block();
    std::shared_ptr<Stmt> parse_function_decl();
    std::shared_ptr<Stmt> parse_for_stmt();
    std::shared_ptr<Stmt> parse_while_stmt();
    std::shared_ptr<Stmt> parse_if_stmt();
    std::shared_ptr<Stmt> parse_var_decl();
    std::shared_ptr<Stmt> parse_print_stmt();
    std::shared_ptr<Stmt> parse_assign_stmt();
    std::shared_ptr<Stmt> parse_expr_stmt();
    std::shared_ptr<Expr> parse_expr();
    std::shared_ptr<Expr> parse_logic_or_expr();
    std::shared_ptr<Expr> parse_logic_and_expr();
    std::shared_ptr<Expr> parse_equality_expr();
    std::shared_ptr<Expr> parse_comparision_expr();
    std::shared_ptr<Expr> parse_term();
    std::shared_ptr<Expr> parse_factor();
    std::shared_ptr<Expr> parse_unary();
    std::shared_ptr<Expr> parse_call();
    std::vector<std::shared_ptr<VariableExpr>> parse_func_params();
    std::vector<std::shared_ptr<Expr>> parse_func_call_arguments();
    std::shared_ptr<Expr> parse_primary();

    bool consumed_all_tokens();
    bool validate_token_and_advance(std::vector<TokenType> tok_types);
    std::shared_ptr<Token> assert_tok_and_advance(TokenType type,
                                                  std::string msg);
    bool validate_token(TokenType tok_type);
    std::shared_ptr<Token> get_prev_tok();
    std::shared_ptr<Token> advance();
    std::shared_ptr<Token> get_cur_tok();

    void panic_mode_synchornize();
};
