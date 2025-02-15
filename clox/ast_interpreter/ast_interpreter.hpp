#pragma once
#include "clox/ast_interpreter/environment.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include <memory>

class InterpreterVisitor : public IExprVisitor, public IStmtVisitor {
  public:
    const std::shared_ptr<Environment> global_env;
    std::shared_ptr<Environment> env;
    InterpreterVisitor();

    ExprVal interpret_single_expr(std::shared_ptr<Expr> expression);

    void interpret_program(std::vector<std::shared_ptr<Stmt>> stmts);

    ExprVal evaluate_expr(std::shared_ptr<Expr> expr);

    void visit_expr_stmt(const ExprStmt &e) override;

    void visit_assign_stmt(const AssignStmt &a) override;

    void visit_print_stmt(const PrintStmt &p) override;

    void visit_var_decl(const VarDecl &v) override;

    void
    visit_block_stmt(const BlockStmt &b,
                     std::shared_ptr<Environment> block_env = nullptr) override;

    void visit_if_stmt(const IfStmt &b) override;

    void visit_while_stmt(const WhileStmt &w) override;

    void visit_function_decl(const FunctionDecl &f) override;

    void visit_return_stmt(const ReturnStmt &r) override;

    ExprVal visit_variable(const VariableExpr &v) override;

    ExprVal visit_literal(const LiteralExpr &l) override;

    ExprVal visit_grouping(const GroupExpr &g) override;

    ExprVal visit_func_call(const FuncCallExpr &f) override;

    ExprVal visit_unary(const UnaryExpr &u) override;

    ExprVal visit_binary(const BinaryExpr &b) override;

    bool cast_literal_to_bool(ExprVal val);

    bool is_equal(ExprVal left, ExprVal right);

    void checkNumberOperand(std::shared_ptr<Token> tok, ExprVal right);

    void checkNumberOperands(std::shared_ptr<Token> tok, ExprVal left,
                             ExprVal right);
    void checkIntOperands(std::shared_ptr<Token> tok, ExprVal left,
                          ExprVal right);
};