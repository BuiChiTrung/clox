#pragma once
#include "clox/environment.hpp"
#include "clox/token.hpp"
#include "expr.hpp"
#include <memory>

class ExprStmt;
class PrintStmt;
class VarStmt;
class AssignStmt;
class BlockStmt;
class IfStmt;
class WhileStmt;
class FunctionStmt;
class ReturnStmt;

class IStmtVisitor {
  public:
    virtual void visit_expr_stmt(const ExprStmt &e) = 0;
    virtual void visit_print_stmt(const PrintStmt &p) = 0;
    virtual void visit_var_stmt(const VarStmt &p) = 0;
    virtual void visit_assign_stmt(const AssignStmt &a) = 0;
    virtual void
    visit_block_stmt(const BlockStmt &b,
                     std::shared_ptr<Environment> block_env = nullptr) = 0;
    virtual void visit_if_stmt(const IfStmt &b) = 0;
    virtual void visit_while_stmt(const WhileStmt &w) = 0;
    virtual void visit_function_stmt(const FunctionStmt &f) = 0;
    virtual void visit_return_stmt(const ReturnStmt &r) = 0;
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

class IfStmt : public Stmt {
  public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> if_block;
    std::shared_ptr<Stmt> else_block;

    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> if_block,
           std::shared_ptr<Stmt> else_block)
        : condition(condition), if_block(if_block), else_block(else_block) {}

    void accept(IStmtVisitor &v) override { return v.visit_if_stmt(*this); }
};

class WhileStmt : public Stmt {
  public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}

    void accept(IStmtVisitor &v) override { return v.visit_while_stmt(*this); }
};

class FunctionStmt : public Stmt {
  public:
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<VariableExpr>> params;
    std::shared_ptr<Stmt> body;

    FunctionStmt(std::shared_ptr<Token> name,
                 std::vector<std::shared_ptr<VariableExpr>> params,
                 std::shared_ptr<Stmt> body)
        : name(name), params(params), body(body) {}

    void accept(IStmtVisitor &v) override {
        return v.visit_function_stmt(*this);
    }
};

class ReturnStmt : public Stmt {
  public:
    std::shared_ptr<Token> return_kw; // used for err reporting
    std::shared_ptr<Expr> expr;

    ReturnStmt(std::shared_ptr<Token> return_kw, std::shared_ptr<Expr> expr)
        : return_kw(return_kw), expr(expr) {}

    void accept(IStmtVisitor &v) override { return v.visit_return_stmt(*this); }
};