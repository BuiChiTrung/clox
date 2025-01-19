#pragma once
#include "expr.hpp"
#include <memory>

class ExprStmt;
class PrintStmt;
class VarStmt;

class IStmtVisitor {
  public:
    virtual void visit_expr_stmt(const ExprStmt &e) = 0;
    virtual void visit_print_stmt(const PrintStmt &p) = 0;
    virtual void visit_var_stmt(const VarStmt &p) = 0;
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
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> initializer;

    VarStmt(std::shared_ptr<Token> name, std::shared_ptr<Expr> initializer)
        : name(name), initializer(initializer) {};

    void accept(IStmtVisitor &v) override { return v.visit_var_stmt(*this); }
};