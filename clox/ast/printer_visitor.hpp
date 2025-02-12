#pragma once
#include "clox/token.hpp"
#include "expr.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <variant>

// This visitor return string
class PrinterVisitor : public IExprVisitor {
  public:
    void print(std::shared_ptr<Expr> epxr) {
        std::cout << std::get<std::string>(epxr->accept(*this));
    }

  private:
    std::string print_expr(std::string name,
                           std::vector<std::shared_ptr<Expr>> exprs) {
        std::ostringstream builder;

        builder << "(" << name;
        for (auto expr : exprs) {
            builder << " " << std::get<std::string>(expr->accept(*this));
        }
        builder << ")";

        return builder.str();
    }

    ExprVal visit_variable(const VariableExpr &v) {
        return this->print_expr(v.name->lexeme, {});
    }

    ExprVal visit_unary(const UnaryExpr &u) {
        return this->print_expr(u.op->lexeme, {u.right});
    }

    ExprVal visit_binary(const BinaryExpr &b) {
        return this->print_expr(b.op->lexeme, {b.left, b.right});
    }

    ExprVal visit_grouping(const GroupExpr &g) {
        return this->print_expr("group", {g.expression});
    }

    ExprVal visit_func_call(const FuncCallExpr &f) { return "callable"; }

    ExprVal visit_literal(const LiteralExpr &l) {
        if (std::holds_alternative<bool>(l.value)) {
            return std::get<bool>(l.value) ? "true" : "false";
        }
        else if (std::holds_alternative<double>(l.value)) {
            return std::to_string(std::get<double>(l.value));
        }
        else if (std::holds_alternative<std::string>(l.value)) {
            return l.value;
        }
        else {
            return "nil";
        }
    }
};