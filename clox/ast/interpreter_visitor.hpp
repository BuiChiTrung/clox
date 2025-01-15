#pragma once
#include "clox/ast/expr.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include "stmt.hpp"
#include <iostream>
#include <memory>
#include <variant>

class InterpreterVisitor : public IExprVisitor, public IStmtVisitor {
  public:
    LiteralVariant interpret_single_expr(std::shared_ptr<Expr> expression) {
        try {
            LiteralVariant result = evaluate_expr(expression);
            return result;
        }
        catch (RuntimeException &err) {
            ErrorManager::handle_runtime_err(err);
            return std::monostate();
        }
    }

    void interpret_program(std::vector<std::shared_ptr<Stmt>> stmts) {
        try {
            for (const auto &stmt : stmts) {
                // exec stmt
                stmt->accept(*this);
            }
            return;
        }
        catch (RuntimeException &err) {
            ErrorManager::handle_runtime_err(err);
            return;
        }
    }

    LiteralVariant evaluate_expr(std::shared_ptr<Expr> expr) {
        return expr->accept(*this);
    }

    void visit_expr_stmt(const ExprStmt &e) override {
        evaluate_expr(e.expr);
        return;
    }

    void visit_print_stmt(const PrintStmt &p) override {
        LiteralVariant val = evaluate_expr(p.expr);
        std::cout << literal_to_string(val) << std::endl;
        return;
    }

    LiteralVariant visit_literal(const Literal &l) override { return l.value; }

    LiteralVariant visit_grouping(const Grouping &g) override {
        return evaluate_expr(g.expression);
    }

    LiteralVariant visit_unary(const Unary &u) override {
        LiteralVariant right = evaluate_expr(u.right);

        switch (u.op->type) {
        case TokenType::BANG:
            return !cast_literal_to_bool(right);
        case TokenType::MINUS:
            checkNumberOperand(u.op, right);
            return -std::get<double>(right);
        default:
            ErrorManager::handle_err(u.op->line, "Invalid unary expression");
            return std::monostate();
        }
    }

    LiteralVariant visit_binary(const Binary &b) override {
        LiteralVariant left = evaluate_expr(b.left);
        LiteralVariant right = evaluate_expr(b.right);

        auto left_double_ptr = std::get_if<double>(&left);
        auto right_double_ptr = std::get_if<double>(&right);
        auto left_string_ptr = std::get_if<std::string>(&left);
        auto right_string_ptr = std::get_if<std::string>(&right);

        switch (b.op->type) {
        // Special case: + op can be used to concate 2 strings
        case TokenType::PLUS:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr + *right_double_ptr;
            }
            if (left_string_ptr && right_string_ptr) {
                return *left_string_ptr + *right_string_ptr;
            }
            throw RuntimeException(
                b.op, "Operands must be two numbers or two strings");
        case TokenType::MINUS:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr - *right_double_ptr;
        case TokenType::STAR:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr * *right_double_ptr;
        case TokenType::SLASH:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr / *right_double_ptr;
        case TokenType::GREATER:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr > *right_double_ptr;
        case TokenType::LESS:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr < *right_double_ptr;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr >= *right_double_ptr;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(b.op, left, right);
            return *left_double_ptr <= *right_double_ptr;
        // Special case: support compare mixed type another type with bool
        case TokenType::BANG_EQUAL:
            return !is_equal(left, right);
        case TokenType::EQUAL_EQUAL:
            return is_equal(left, right);
        case TokenType::AND:
            return cast_literal_to_bool(left) && cast_literal_to_bool(right);
        case TokenType::OR:
            return cast_literal_to_bool(left) || cast_literal_to_bool(right);
        default:
            ErrorManager::handle_err(b.op->line, "Invalid binary expression");
            return std::monostate();
        }
    }

    bool cast_literal_to_bool(LiteralVariant val) {
        if (const auto boolPtr(std::get_if<bool>(&val)); boolPtr) {
            return *boolPtr;
        }
        if (const auto doublePtr(std::get_if<double>(&val)); doublePtr) {
            return *doublePtr != 0;
        }
        if (const auto strPtr(std::get_if<std::string>(&val)); strPtr) {
            return strPtr->length() != 0;
        }
        if (std::holds_alternative<std::monostate>(val)) {
            return false;
        }

        ErrorManager::handle_err(0, "Unsupported type to cast to bool");
        exit(1);
    }

    bool is_equal(LiteralVariant left, LiteralVariant right) {
        if (std::holds_alternative<bool>(left) ||
            std::holds_alternative<bool>(right)) {
            return cast_literal_to_bool(left) == cast_literal_to_bool(right);
        }

        // Compare variant: Type check then value check.
        return left == right;
    }

    void checkNumberOperand(std::shared_ptr<Token> tok, LiteralVariant right) {
        if (!std::holds_alternative<double>(right)) {
            throw RuntimeException(tok, "Right operand must be a number");
        }
        return;
    }

    void checkNumberOperands(std::shared_ptr<Token> tok, LiteralVariant left,
                             LiteralVariant right) {
        if (!std::holds_alternative<double>(right)) {
            throw RuntimeException(tok, "Right operand must be a number");
        }
        if (!std::holds_alternative<double>(left)) {
            throw RuntimeException(tok, "Left operand must be a number");
        }
        return;
    }
};