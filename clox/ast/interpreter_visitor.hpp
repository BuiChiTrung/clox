#pragma once
#include "clox/ast/expr.hpp"
#include "clox/environment.hpp"
#include "clox/token.hpp"
#include "stmt.hpp"
#include <memory>

class InterpreterVisitor : public IExprVisitor, public IStmtVisitor {
  public:
    std::shared_ptr<Environment> env = std::make_shared<Environment>();

    LiteralVariant interpret_single_expr(std::shared_ptr<Expr> expression);

    void interpret_program(std::vector<std::shared_ptr<Stmt>> stmts);

    LiteralVariant evaluate_expr(std::shared_ptr<Expr> expr);

    void visit_expr_stmt(const ExprStmt &e) override;

    void visit_assign_stmt(const AssignStmt &a) override;

    void visit_print_stmt(const PrintStmt &p) override;

    void visit_var_stmt(const VarStmt &v) override;

    void visit_block_stmt(const BlockStmt &b) override;

    void visit_if_stmt(const IfStmt &b) override;

    LiteralVariant visit_variable(const VariableExpr &v) override;

    LiteralVariant visit_literal(const LiteralExpr &l) override;

    LiteralVariant visit_grouping(const GroupExpr &g) override;

    LiteralVariant visit_unary(const UnaryExpr &u) override;

    LiteralVariant visit_binary(const BinaryExpr &b) override;

    bool cast_literal_to_bool(LiteralVariant val);

    bool is_equal(LiteralVariant left, LiteralVariant right);

    void checkNumberOperand(std::shared_ptr<Token> tok, LiteralVariant right);

    void checkNumberOperands(std::shared_ptr<Token> tok, LiteralVariant left,
                             LiteralVariant right);
};