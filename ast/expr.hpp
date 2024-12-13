#pragma once
#include "../token.hpp"

class Unary;
class Binary;
class Grouping;
class Literal;

template <typename T> class Visitor {
  public:
    virtual T visit_binary(Binary &b) = 0;
    virtual T visit_grouping(Grouping &g) = 0;
    virtual T visit_literal(Literal &l) = 0;
    virtual T visit_unary(Unary &u) = 0;
};

class Expr {
  public:
    template <typename T> T accept(Visitor<T> &visitor);
};

class Binary : public Expr {
  public:
    Expr left;
    Token op;
    Expr right;

    Binary(Expr &left, Token &op, Expr &right) {
        this->left = left;
        this->op = op;
        this->right = right;
    }

    template <typename T> T accept(Visitor<T> &visitor) {
        return visitor.visit_binary(*this);
    }
};

class Grouping : public Expr {
  public:
    Expr expression;

    Grouping(Expr &expr) { this->expression = expr; }

    template <typename T> T accept(Visitor<T> &visitor) {
        return visitor.visit_grouping(*this);
    }
};

class Literal : public Expr {
  public:
    std::any value;

    Literal(std::any value) { this->value = value; }

    template <typename T> T accept(Visitor<T> &visitor) {
        return visitor.visit_literal(*this);
    }
};

class Unary : public Expr {
  public:
    Token op;
    Expr right;

    Unary(Token &op, Expr &right) {
        this->op = op;
        this->right = right;
    }

    template <typename T> T accept(Visitor<T> &visitor) {
        return visitor.visit_unary(*this);
    }
};
