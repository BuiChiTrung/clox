#pragma once
#include "expr.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class PrinterVisitor : public IVisitor {
  public:
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

    Variant visit_unary(std::shared_ptr<Unary> u) {
        return this->print_expr(u->op->lexeme, {u->right});
    }

    Variant visit_binary(std::shared_ptr<Binary> b) {
        return this->print_expr(b->op->lexeme, {b->left, b->right});
    }

    Variant visit_grouping(std::shared_ptr<Grouping> g) {
        return this->print_expr("Group", {g->expression});
    }

    Variant visit_literal(Literal *l) {
        if (l->value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(l->value));
        }
        else if (l->value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(l->value);
        }
        else {
            std::cout << "Invalid literal value" << "\n";
            exit(1);
        }
    }
};