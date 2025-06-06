#pragma once
#include "clox/ast_interpreter/callable/callable.hpp"
#include "clox/ast_interpreter/callable/class.hpp"
#include "clox/common/error_manager.hpp"
#include "clox/common/expr_val.hpp"
#include <string>

inline std::string cast_expr_val_to_string(const ExprVal &value) {
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, double>) {
                return double_to_string(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T,
                                                std::shared_ptr<LoxCallable>> ||
                                 std::is_same_v<T,
                                                std::shared_ptr<LoxInstance>>) {
                return arg ? arg->to_string() : "nil";
            } else {
                return "nil";
            }
        },
        value);
}

inline bool cast_expr_val_to_bool(const ExprVal &val) {
    if (const auto boolPtr(std::get_if<bool>(&val)); boolPtr) {
        return *boolPtr;
    }
    if (const auto doublePtr(std::get_if<double>(&val)); doublePtr) {
        return *doublePtr != 0;
    }
    if (const auto strPtr(std::get_if<std::string>(&val)); strPtr) {
        if (*strPtr == "false") {
            return false;
        }
        return strPtr->length() != 0;
    }
    if (std::holds_alternative<std::monostate>(val)) {
        return false;
    }

    throw RuntimeException(nullptr, "Unsupported type to cast to bool");
}

inline double cast_expr_val_to_double(const ExprVal &val) {
    if (const auto doublePtr(std::get_if<double>(&val)); doublePtr) {
        return *doublePtr;
    }
    if (const auto boolPtr(std::get_if<bool>(&val)); boolPtr) {
        return *boolPtr ? 1.0 : 0.0;
    }
    if (const auto strPtr(std::get_if<std::string>(&val)); strPtr) {
        try {
            return std::stod(*strPtr);
        } catch (std::invalid_argument &) {
            throw RuntimeException(nullptr, "Cannot cast string to double");
        }
    }

    throw RuntimeException(nullptr, "Unsupported type to cast to double");
}

inline bool is_expr_vals_equal(const ExprVal &left, const ExprVal &right) {
    if (std::holds_alternative<bool>(left) ||
        std::holds_alternative<bool>(right)) {
        return cast_expr_val_to_bool(left) == cast_expr_val_to_bool(right);
    }

    // Compare variant: Type check then value check.
    return left == right;
}

inline void assert_expr_val_number(std::shared_ptr<Token> tok,
                                   const ExprVal &right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
}

inline void assert_expr_vals_number(std::shared_ptr<Token> tok,
                                    const ExprVal &left, const ExprVal &right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
    if (!std::holds_alternative<double>(left)) {
        throw RuntimeException(tok, "Left operand must be a number");
    }
}

inline void assert_expr_vals_int(std::shared_ptr<Token> tok,
                                 const ExprVal &left, const ExprVal &right) {
    assert_expr_vals_number(tok, left, right);

    double left_double = std::get<double>(left);
    if (static_cast<int>(left_double) != left_double) {
        throw RuntimeException(tok, "Left operand must be an int");
    }
    double right_double = std::get<double>(right);
    if (static_cast<int>(right_double) != right_double) {
        throw RuntimeException(tok, "Right operand must be an int");
    }
}
