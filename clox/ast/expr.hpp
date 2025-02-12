#pragma once
#include "clox/token.hpp"
#include <memory>

class UnaryExpr;
class BinaryExpr;
class GroupExpr;
class LiteralExpr;
class VariableExpr;
class FuncCallExpr;

// Use visitor design pattern to pack all the logic of override function for all
// Exp subclass in a seperate Visitor class.
class IExprVisitor {
  public:
    virtual ExprVal visit_literal(const LiteralExpr &l) = 0;
    virtual ExprVal visit_grouping(const GroupExpr &g) = 0;
    virtual ExprVal visit_unary(const UnaryExpr &u) = 0;
    virtual ExprVal visit_binary(const BinaryExpr &b) = 0;
    virtual ExprVal visit_variable(const VariableExpr &v) = 0;
    virtual ExprVal visit_func_call(const FuncCallExpr &f) = 0;
};

class Expr {
  public:
    // IVisitor param CANNOT be smart pointer as when we call
    // visitor.visit_<...>() in Expr subclass, we have to create a smart pointer
    // pointed to this, when this smart pointer run of scope, it'll destruct our
    // Visitor obj.
    virtual ExprVal accept(IExprVisitor &visitor) = 0;
};

class BinaryExpr : public Expr {
  public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> &left, std::shared_ptr<Token> &op,
               std::shared_ptr<Expr> &right)
        : left(left), op(op), right(right) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_binary(*this);
    }
};

class GroupExpr : public Expr {
  public:
    std::shared_ptr<Expr> expression;

    GroupExpr(std::shared_ptr<Expr> &expression) : expression(expression) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_grouping(*this);
    }
};

class LiteralExpr : public Expr {
  public:
    ExprVal value;

    LiteralExpr(ExprVal value) : value(value) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_literal(*this);
    }
};

class UnaryExpr : public Expr {
  public:
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    UnaryExpr(std::shared_ptr<Token> &op, std::shared_ptr<Expr> &right)
        : op(op), right(right) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_unary(*this);
    }
};

class VariableExpr : public Expr {
  public:
    std::shared_ptr<Token> name;

    VariableExpr(std::shared_ptr<Token> name) : name(name) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_variable(*this);
    }
};

class FuncCallExpr : public Expr {
  public:
    std::shared_ptr<Expr> callee;
    std::shared_ptr<Token> close_parenthesis;
    std::vector<std::shared_ptr<Expr>> args;

    FuncCallExpr(std::shared_ptr<Expr> callee,
                 std::shared_ptr<Token> close_parenthesis,
                 std::vector<std::shared_ptr<Expr>> args)
        : callee(callee), close_parenthesis(close_parenthesis), args(args) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_func_call(*this);
    }
};