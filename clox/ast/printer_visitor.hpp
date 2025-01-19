#pragma once
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

    LiteralVariant visit_variable(const Variable &v) {
        return this->print_expr(v.name->lexeme, {});
    }

    LiteralVariant visit_unary(const Unary &u) {
        return this->print_expr(u.op->lexeme, {u.right});
    }

    LiteralVariant visit_binary(const Binary &b) {
        return this->print_expr(b.op->lexeme, {b.left, b.right});
    }

    LiteralVariant visit_grouping(const Grouping &g) {
        return this->print_expr("group", {g.expression});
    }

    LiteralVariant visit_literal(const Literal &l) {
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