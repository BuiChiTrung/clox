#pragma once
#include "clox/common/token.hpp"
#include <memory>

class UnaryExpr;
class BinaryExpr;
class GroupExpr;
class LiteralExpr;
class IdentifierExpr;
class FuncCallExpr;
class GetClassFieldExpr;
class ThisExpr;

// Use visitor design pattern to pack all the logic of override function for all
// Exp subclass in a seperate Visitor class.
class IExprVisitor {
  public:
    virtual ExprVal visit_literal(const LiteralExpr &) = 0;
    virtual ExprVal visit_grouping(const GroupExpr &) = 0;
    virtual ExprVal visit_unary(const UnaryExpr &) = 0;
    virtual ExprVal visit_binary(const BinaryExpr &) = 0;
    virtual ExprVal visit_identifier(const IdentifierExpr &) = 0;
    virtual ExprVal visit_func_call(const FuncCallExpr &) = 0;
    virtual ExprVal visit_get_class_field(const GetClassFieldExpr &) = 0;
    virtual ExprVal visit_this(const ThisExpr &) = 0;
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
    std::shared_ptr<Expr> left_operand;
    std::shared_ptr<Token> operation;
    std::shared_ptr<Expr> right_operand;

    BinaryExpr(std::shared_ptr<Expr> &left_operand, std::shared_ptr<Token> &op,
               std::shared_ptr<Expr> &right_operand)
        : left_operand(left_operand), operation(op),
          right_operand(right_operand) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_binary(*this);
    }
};

class GroupExpr : public Expr {
  public:
    std::shared_ptr<Expr> expr;

    GroupExpr(std::shared_ptr<Expr> &expr) : expr(expr) {}

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
    std::shared_ptr<Token> operation;
    std::shared_ptr<Expr> operand;

    UnaryExpr(std::shared_ptr<Token> &op, std::shared_ptr<Expr> &operand)
        : operation(op), operand(operand) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_unary(*this);
    }
};

class IdentifierExpr : public Expr {
  public:
    std::shared_ptr<Token> name;

    IdentifierExpr(std::shared_ptr<Token> name) : name(name) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_identifier(*this);
    }
};

class ThisExpr : public IdentifierExpr {
  public:
    using IdentifierExpr::IdentifierExpr;
    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_this(*this);
    }
};

class FuncCallExpr : public Expr {
  public:
    std::shared_ptr<Expr> callee;
    std::shared_ptr<Token> close_parenthesis;
    std::vector<std::shared_ptr<Expr>> args;

    FuncCallExpr(std::shared_ptr<Expr> callee,
                 std::shared_ptr<Token> close_parenthesis,
                 std::vector<std::shared_ptr<Expr>> &args)
        : callee(callee), close_parenthesis(close_parenthesis), args(args) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_func_call(*this);
    }
};

class GetClassFieldExpr : public Expr {
  public:
    std::shared_ptr<Expr> lox_instance;
    std::shared_ptr<Token> field_token;

    GetClassFieldExpr(std::shared_ptr<Expr> lox_instance,
                      std::shared_ptr<Token> field_token)
        : lox_instance(lox_instance), field_token(field_token) {}

    ExprVal accept(IExprVisitor &visitor) override {
        return visitor.visit_get_class_field(*this);
    }
};
