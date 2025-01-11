#pragma once
#include "../clox/token.hpp"
#include <memory>

class Unary;
class Binary;
class Grouping;
class Literal;

// Use visitor design pattern to pack all the logic of override function for all
// Exp subclass in a seperate Visitor class.
class IVisitor {
  public:
    virtual LiteralVariant visit_literal(const Literal &l) = 0;
    virtual LiteralVariant visit_grouping(const Grouping &g) = 0;
    virtual LiteralVariant visit_unary(const Unary &u) = 0;
    virtual LiteralVariant visit_binary(const Binary &b) = 0;
};

class Expr {
  public:
    // IVisitor param CANNOT be smart pointer as when we call
    // visitor.visit_<...>() in Expr subclass, we have to create a smart pointer
    // pointed to this, when this smart pointer run of scope, it'll destruct our
    // Visitor obj.
    virtual LiteralVariant accept(IVisitor &visitor) = 0;
};

class Binary : public Expr {
  public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> &left, std::shared_ptr<Token> &op,
           std::shared_ptr<Expr> &right)
        : left(left), op(op), right(right) {}

    LiteralVariant accept(IVisitor &visitor) override {
        return visitor.visit_binary(*this);
    }
};

class Grouping : public Expr {
  public:
    std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> &expression) : expression(expression) {}

    LiteralVariant accept(IVisitor &visitor) override {
        return visitor.visit_grouping(*this);
    }
};

class Literal : public Expr {
  public:
    LiteralVariant value;

    Literal(LiteralVariant value) : value(value) {}

    LiteralVariant accept(IVisitor &visitor) override {
        return visitor.visit_literal(*this);
    }
};

class Unary : public Expr {
  public:
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    Unary(std::shared_ptr<Token> &op, std::shared_ptr<Expr> &right)
        : op(op), right(right) {}

    LiteralVariant accept(IVisitor &visitor) override {
        return visitor.visit_unary(*this);
    }
};
