#include "ast/expr.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include <variant>

class InterpreterVisitor : IVisitor {
  public:
    LiteralVariant visit_literal(const Literal &l) { return l.value; }

    LiteralVariant visit_grouping(const Grouping &g) {
        return g.expression->accept(*this);
    }

    LiteralVariant visit_unary(const Unary &u) {
        LiteralVariant right = u.right->accept(*this);

        switch (u.op->type) {
        case TokenType::BANG:
            return !cast_literal_to_bool(right);
        case TokenType::MINUS:
            if (const auto double_ptr(std::get_if<double>(&right));
                double_ptr) {
                return -*double_ptr;
            }
        default:
            ErrorManager::err(u.op->line, "Invalid unary expression");
            return std::monostate();
        }
    }

    LiteralVariant visit_binary(const Binary &b) {
        LiteralVariant left = b.left->accept(*this);
        LiteralVariant right = b.right->accept(*this);

        auto left_double_ptr = std::get_if<double>(&left);
        auto right_double_ptr = std::get_if<double>(&right);
        auto left_string_ptr = std::get_if<std::string>(&left);
        auto right_string_ptr = std::get_if<std::string>(&right);
        auto left_bool_ptr = std::get_if<bool>(&left);
        auto right_bool_ptr = std::get_if<bool>(&right);

        switch (b.op->type) {
        // Special case: + op can be used to concate 2 strings
        case TokenType::PLUS:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr + *right_double_ptr;
            }
            if (left_string_ptr && right_string_ptr) {
                return *left_string_ptr + *right_string_ptr;
            }
        case TokenType::MINUS:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr - *right_double_ptr;
            }
        case TokenType::STAR:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr * *right_double_ptr;
            }
        case TokenType::SLASH:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr / *right_double_ptr;
            }
        case TokenType::GREATER:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr > *right_double_ptr;
            }
        case TokenType::LESS:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr < *right_double_ptr;
            }
        case TokenType::GREATER_EQUAL:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr >= *right_double_ptr;
            }
        case TokenType::LESS_EQUAL:
            if (left_double_ptr && right_double_ptr) {
                return *left_double_ptr <= *right_double_ptr;
            }
        // Special case: support compare mixed type another type with bool
        case TokenType::BANG_EQUAL:
            return !is_equal(left, right);
        case TokenType::EQUAL_EQUAL:
            return is_equal(left, right);
        case TokenType::AND:
            if (left_double_ptr && right_double_ptr) {
                return cast_literal_to_bool(*left_double_ptr) &&
                       cast_literal_to_bool(*right_double_ptr);
            }
            if (left_string_ptr && right_string_ptr) {
                return cast_literal_to_bool(*left_string_ptr) &&
                       cast_literal_to_bool(*right_string_ptr);
            }
        case TokenType::OR:
            if (left_double_ptr) {
                return cast_literal_to_bool(*left_double_ptr) ||
                       cast_literal_to_bool(*right_double_ptr);
            }
            if (left_string_ptr && right_string_ptr) {
                return cast_literal_to_bool(*left_string_ptr) ||
                       cast_literal_to_bool(*right_string_ptr);
            }
        default:
            ErrorManager::err(b.op->line, "Invalid binary expression");
            return std::monostate();
        }
    }

  private:
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

        ErrorManager::err(0, "Unsupported type to cast to bool");
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
};