#pragma once
#include "clox/common/token.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include <memory>

class Parser {
  private:
    std::vector<std::shared_ptr<Token>> tokens;
    uint32_t current_tok_pos = 0;

    std::shared_ptr<Stmt> parse_declaration();
    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<ReturnStmt> parse_return_stmt();
    std::shared_ptr<BlockStmt> parse_block_stmt();
    std::shared_ptr<FunctionDecl> parse_function_decl();
    std::shared_ptr<ClassDecl> parse_class_decl();
    std::shared_ptr<FunctionDecl> parse_function();
    // return BlockStmt or WhileStmt
    std::shared_ptr<Stmt> parse_for_stmt();
    std::shared_ptr<WhileStmt> parse_while_stmt();
    std::shared_ptr<IfStmt> parse_if_stmt();
    std::shared_ptr<VarDecl> parse_var_decl();
    std::shared_ptr<PrintStmt> parse_print_stmt();
    // return SetClassFieldStmt or AssignStmt
    std::shared_ptr<Stmt> parse_assign_stmt();
    std::shared_ptr<Expr> parse_expr();
    std::shared_ptr<Expr> parse_logic_or_expr();
    std::shared_ptr<Expr> parse_logic_and_expr();
    std::shared_ptr<Expr> parse_equality_expr();
    std::shared_ptr<Expr> parse_comparision_expr();
    std::shared_ptr<Expr> parse_term();
    std::shared_ptr<Expr> parse_factor();
    std::shared_ptr<Expr> parse_unary();
    std::shared_ptr<Expr> parse_call();
    std::vector<std::shared_ptr<IdentifierExpr>> parse_func_params();
    std::vector<std::shared_ptr<Expr>> parse_func_call_arguments();
    std::shared_ptr<Expr> parse_primary();

    bool consumed_all_tokens();
    bool validate_token_and_advance(const std::vector<TokenType> &tok_types);
    std::shared_ptr<Token> assert_tok_and_advance(TokenType type,
                                                  std::string msg);
    bool validate_token(TokenType tok_type);
    std::shared_ptr<Token> get_prev_tok();
    std::shared_ptr<Token> advance();
    std::shared_ptr<Token> get_cur_tok();

    void panic_mode_synchornize();

  public:
    Parser(const std::vector<std::shared_ptr<Token>> &tokens);

    std::vector<std::shared_ptr<Stmt>> parse_program();
    std::shared_ptr<Expr> parse_single_expr();
};
