#pragma once
#include "expr.hpp"
#include <memory>

class ExprStmt;
class PrintStmt;
class VarStmt;
class AssignStmt;
class BlockStmt;

class IStmtVisitor {
  public:
    virtual void visit_expr_stmt(const ExprStmt &e) = 0;
    virtual void visit_print_stmt(const PrintStmt &p) = 0;
    virtual void visit_var_stmt(const VarStmt &p) = 0;
    virtual void visit_assign_stmt(const AssignStmt &a) = 0;
    virtual void visit_block_stmt(const BlockStmt &b) = 0;
};

class Stmt {
  public:
    virtual void accept(IStmtVisitor &v) = 0;
};

class ExprStmt : public Stmt {
  public:
    std::shared_ptr<Expr> expr;

    ExprStmt(std::shared_ptr<Expr> expr) : expr(expr) {};

    void accept(IStmtVisitor &v) override { return v.visit_expr_stmt(*this); }
};

class PrintStmt : public Stmt {
  public:
    std::shared_ptr<Expr> expr;

    PrintStmt(std::shared_ptr<Expr> expr) : expr(expr) {};

    void accept(IStmtVisitor &v) override { return v.visit_print_stmt(*this); }
};

class VarStmt : public Stmt {
  public:
    std::shared_ptr<Token> var_name;
    std::shared_ptr<Expr> initializer;

    VarStmt(std::shared_ptr<Token> var_name, std::shared_ptr<Expr> initializer)
        : var_name(var_name), initializer(initializer) {};

    void accept(IStmtVisitor &v) override { return v.visit_var_stmt(*this); }
};

class AssignStmt : public Stmt {
  public:
    std::shared_ptr<VariableExpr> var;
    std::shared_ptr<Expr> value;

    AssignStmt(std::shared_ptr<VariableExpr> var, std::shared_ptr<Expr> value)
        : var(var), value(value) {}

    void accept(IStmtVisitor &v) override { return v.visit_assign_stmt(*this); }
};

class BlockStmt : public Stmt {
  public:
    std::vector<std::shared_ptr<Stmt>> stmts;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(stmts) {}

    void accept(IStmtVisitor &v) override { return v.visit_block_stmt(*this); }
};