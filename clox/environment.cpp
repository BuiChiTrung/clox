#include "environment.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include <format>
#include <memory>

Environment::Environment() {
    variable_table = {};
    parent_scope_env = nullptr;
}
Environment::Environment(std::shared_ptr<Environment> parent_scope_env)
    : parent_scope_env(parent_scope_env) {}

void Environment::add_new_variable(std::shared_ptr<Token> var_tok,
                                   LiteralVariant value) {
    std::string var_name = var_tok->lexeme;
    if (variable_table.count(var_name)) {
        throw RuntimeException(
            nullptr,
            std::format("Error: Variable {} is already declared.", var_name));
    }

    variable_table[var_name] = value;
}

LiteralVariant Environment::get_variable(std::shared_ptr<Token> var_tok) {
    std::string var_name = var_tok->lexeme;
    if (variable_table.count(var_name)) {
        return variable_table[var_name];
    }

    if (parent_scope_env != nullptr) {
        return parent_scope_env->get_variable(var_tok);
    }

    throw RuntimeException(var_tok, "Error: reference to non-exist variable.");
}

void Environment::assign_new_value_to_variable(std::shared_ptr<Token> var_tok,
                                               LiteralVariant value) {
    std::string var_name = var_tok->lexeme;
    if (variable_table.count(var_name)) {
        variable_table[var_name] = value;
        return;
    }

    if (parent_scope_env != nullptr) {
        parent_scope_env->assign_new_value_to_variable(var_tok, value);
        return;
    }

    throw RuntimeException(var_tok, "Error: assignment to non-exist variable.");
}