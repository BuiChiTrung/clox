#pragma once
#include "clox/token.hpp"
#include <memory>

class UnaryExpr;
class BinaryExpr;
class GroupExpr;
class LiteralExpr;
class VariableExpr;

// Use visitor design pattern to pack all the logic of override function for all
// Exp subclass in a seperate Visitor class.
class IExprVisitor {
  public:
    virtual LiteralVariant visit_literal(const LiteralExpr &l) = 0;
    virtual LiteralVariant visit_grouping(const GroupExpr &g) = 0;
    virtual LiteralVariant visit_unary(const UnaryExpr &u) = 0;
    virtual LiteralVariant visit_binary(const BinaryExpr &b) = 0;
    virtual LiteralVariant visit_variable(const VariableExpr &v) = 0;
};

class Expr {
  public:
    // IVisitor param CANNOT be smart pointer as when we call
    // visitor.visit_<...>() in Expr subclass, we have to create a smart pointer
    // pointed to this, when this smart pointer run of scope, it'll destruct our
    // Visitor obj.
    virtual LiteralVariant accept(IExprVisitor &visitor) = 0;
};

class BinaryExpr : public Expr {
  public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> &left, std::shared_ptr<Token> &op,
               std::shared_ptr<Expr> &right)
        : left(left), op(op), right(right) {}

    LiteralVariant accept(IExprVisitor &visitor) override {
        return visitor.visit_binary(*this);
    }
};

class GroupExpr : public Expr {
  public:
    std::shared_ptr<Expr> expression;

    GroupExpr(std::shared_ptr<Expr> &expression) : expression(expression) {}

    LiteralVariant accept(IExprVisitor &visitor) override {
        return visitor.visit_grouping(*this);
    }
};

class LiteralExpr : public Expr {
  public:
    LiteralVariant value;

    LiteralExpr(LiteralVariant value) : value(value) {}

    LiteralVariant accept(IExprVisitor &visitor) override {
        return visitor.visit_literal(*this);
    }
};

class UnaryExpr : public Expr {
  public:
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    UnaryExpr(std::shared_ptr<Token> &op, std::shared_ptr<Expr> &right)
        : op(op), right(right) {}

    LiteralVariant accept(IExprVisitor &visitor) override {
        return visitor.visit_unary(*this);
    }
};

class VariableExpr : public Expr {
  public:
    std::shared_ptr<Token> name;

    VariableExpr(std::shared_ptr<Token> name) : name(name) {}

    LiteralVariant accept(IExprVisitor &visitor) override {
        return visitor.visit_variable(*this);
    }
};