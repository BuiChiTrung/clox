#pragma once
#include "clox/ast_interpreter/callable.hpp"
#include "clox/ast_interpreter/class.hpp"
#include "clox/common/expr_val.hpp"
#include <string>

inline std::string exprval_to_string(const ExprVal &value) {
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