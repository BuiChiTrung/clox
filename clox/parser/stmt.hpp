#pragma once
#include "clox/ast_interpreter/environment.hpp"
#include "clox/common/token.hpp"
#include "expr.hpp"
#include <memory>
#include <vector>

class ExprStmt;
class PrintStmt;
class VarDecl;
class AssignStmt;
class BlockStmt;
class IfStmt;
class WhileStmt;
class BreakStmt;
class ContinueStmt;
class FunctionDecl;
class ReturnStmt;
class ClassDecl;
class SetClassFieldStmt;

class IStmtVisitor {
  public:
    virtual void visit_var_decl(const VarDecl &) = 0;
    virtual void visit_function_decl(FunctionDecl &) = 0;
    virtual void visit_expr_stmt(const ExprStmt &) = 0;
    virtual void visit_print_stmt(const PrintStmt &) = 0;
    virtual void visit_assign_stmt(const AssignStmt &) = 0;
    virtual void
    visit_block_stmt(const BlockStmt &b,
                     std::shared_ptr<Environment> block_env = nullptr) = 0;
    virtual void visit_if_stmt(const IfStmt &) = 0;
    virtual void visit_while_stmt(const WhileStmt &) = 0;
    virtual void visit_break_stmt(const BreakStmt &) = 0;
    virtual void visit_continue_stmt(const ContinueStmt &) = 0;
    virtual void visit_return_stmt(const ReturnStmt &) = 0;
    virtual void visit_class_decl(const ClassDecl &) = 0;
    virtual void visit_set_class_field(const SetClassFieldStmt &) = 0;
};

// We use the same class Stmt for both statment and declaration for simplicity
// as both class has only 1 same method: accept
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

class VarDecl : public Stmt {
  public:
    std::shared_ptr<Token> var_name;
    std::shared_ptr<Expr> initializer;

    VarDecl(std::shared_ptr<Token> var_name, std::shared_ptr<Expr> initializer)
        : var_name(var_name), initializer(initializer) {};

    void accept(IStmtVisitor &v) override { return v.visit_var_decl(*this); }
};

class AssignStmt : public Stmt {
  public:
    std::shared_ptr<IdentifierExpr> var;
    std::shared_ptr<Expr> value;

    AssignStmt(std::shared_ptr<IdentifierExpr> var, std::shared_ptr<Expr> value)
        : var(var), value(value) {}

    void accept(IStmtVisitor &v) override { return v.visit_assign_stmt(*this); }
};

class BlockStmt : public Stmt {
  public:
    std::vector<std::shared_ptr<Stmt>> stmts;
    // This is used when we exec continue statement in a for loop, run this
    // statement to increment the loop variable before jumping out from the
    // block scope
    std::shared_ptr<Stmt> for_loop_increment = nullptr;

    BlockStmt(const std::vector<std::shared_ptr<Stmt>> &stmts) : stmts(stmts) {}

    void accept(IStmtVisitor &v) override { return v.visit_block_stmt(*this); }
};

class IfStmt : public Stmt {
  public:
    std::vector<std::shared_ptr<Expr>> conditions;
    std::vector<std::shared_ptr<Stmt>> if_blocks;
    std::shared_ptr<Stmt> else_block;

    IfStmt(std::vector<std::shared_ptr<Expr>> &conditions,
           std::vector<std::shared_ptr<Stmt>> &if_blocks,
           std::shared_ptr<Stmt> else_block)
        : conditions(conditions), if_blocks(if_blocks), else_block(else_block) {
    }

    void accept(IStmtVisitor &v) override { return v.visit_if_stmt(*this); }
};

class WhileStmt : public Stmt {
  public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> body)
        : condition(condition), body(body) {}

    void accept(IStmtVisitor &v) override { return v.visit_while_stmt(*this); }
};

class BreakStmt : public Stmt {
  public:
    std::shared_ptr<Token> break_kw;

    BreakStmt(std::shared_ptr<Token> break_kw) : break_kw(break_kw) {}
    void accept(IStmtVisitor &v) override { return v.visit_break_stmt(*this); }
};

class BreakKwException : public std::exception {};
class ContinueStmt : public Stmt {
  public:
    std::shared_ptr<Token> continue_kw;

    ContinueStmt(std::shared_ptr<Token> continue_kw)
        : continue_kw(continue_kw) {}
    void accept(IStmtVisitor &v) override {
        return v.visit_continue_stmt(*this);
    }
};

class ContinueKwException : public std::exception {};

class FunctionDecl : public Stmt {
  public:
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<IdentifierExpr>> params;
    std::shared_ptr<BlockStmt> body;

    FunctionDecl(std::shared_ptr<Token> name,
                 std::vector<std::shared_ptr<IdentifierExpr>> &params,
                 std::shared_ptr<BlockStmt> body)
        : name(name), params(params), body(body) {}

    void accept(IStmtVisitor &v) override {
        return v.visit_function_decl(*this);
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

class ReturnKwException : std::exception {
  public:
    ExprVal return_val;
    ReturnKwException(ExprVal return_val) : return_val(return_val) {}
};

class ClassDecl : public Stmt {
  public:
    std::shared_ptr<Token> name;
    std::shared_ptr<IdentifierExpr> superclass;
    std::vector<std::shared_ptr<FunctionDecl>> methods;

    ClassDecl(std::shared_ptr<Token> name,
              std::shared_ptr<IdentifierExpr> superclass,
              std::vector<std::shared_ptr<FunctionDecl>> &methods)
        : name(name), superclass(superclass), methods(methods) {}

    void accept(IStmtVisitor &v) override { return v.visit_class_decl(*this); }
};

class SetClassFieldStmt : public Stmt {
  public:
    std::shared_ptr<Expr> lox_instance;
    std::shared_ptr<Token> field_token;
    std::shared_ptr<Expr> value;

    SetClassFieldStmt(std::shared_ptr<Expr> lox_instance,
                      std::shared_ptr<Token> field_token,
                      std::shared_ptr<Expr> value)
        : lox_instance(lox_instance), field_token(field_token), value(value) {}

    void accept(IStmtVisitor &v) override {
        return v.visit_set_class_field(*this);
    }
};
