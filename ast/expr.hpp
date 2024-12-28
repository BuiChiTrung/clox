#pragma once
#include "../clox/token.hpp"
#include <memory>
#include <variant>

class Unary;
class Binary;
class Grouping;
class Literal;

using Variant = std::variant<std::string, std::monostate>;

class IVisitor {
  public:
    virtual Variant visit_literal(const Literal &l) = 0;
    virtual Variant visit_grouping(const Grouping &g) = 0;
    virtual Variant visit_unary(const Unary &u) = 0;
    virtual Variant visit_binary(const Binary &b) = 0;
};

class Expr {
  public:
    virtual Variant accept(IVisitor &visitor) = 0;
};

class Binary : public Expr {
  public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> &left, std::shared_ptr<Token> &op,
           std::shared_ptr<Expr> &right)
        : left(left), op(op), right(right) {}

    Variant accept(IVisitor &visitor) override {
        return visitor.visit_binary(*this);
    }
};

class Grouping : public Expr {
  public:
    std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> &expression) : expression(expression) {}

    Variant accept(IVisitor &visitor) override {
        return visitor.visit_grouping(*this);
    }
};

class Literal : public Expr {
  public:
    std::any value;

    Literal(std::any value) { this->value = value; }

    Variant accept(IVisitor &visitor) override {
        return visitor.visit_literal(*this);
    }
};

class Unary : public Expr {
  public:
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;

    Unary(std::shared_ptr<Token> &op, std::shared_ptr<Expr> &right)
        : op(op), right(right) {}

    Variant accept(IVisitor &visitor) override {
        return visitor.visit_unary(*this);
    }
};
