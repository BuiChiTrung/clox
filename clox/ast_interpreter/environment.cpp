#include "clox/ast_interpreter/environment.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/scanner/token.hpp"
#include <memory>

Environment::Environment() {
    identifier_table = {};
    parent_scope_env = nullptr;
}
Environment::Environment(std::shared_ptr<Environment> parent_scope_env)
    : parent_scope_env(parent_scope_env) {}

void Environment::add_new_variable(std::string name, ExprVal value) {
    identifier_table[name] = value;
}

ExprVal Environment::get_variable(std::shared_ptr<Token> var_tok) {
    std::string var_name = var_tok->lexeme;
    if (identifier_table.count(var_name)) {
        return identifier_table[var_name];
    }

    if (parent_scope_env != nullptr) {
        return parent_scope_env->get_variable(var_tok);
    }

    throw RuntimeException(var_tok, "Reference to non-exist variable.");
}

void Environment::assign_new_value_to_variable(std::shared_ptr<Token> var_tok,
                                               ExprVal value) {
    std::string var_name = var_tok->lexeme;
    if (identifier_table.count(var_name)) {
        identifier_table[var_name] = value;
        return;
    }

    if (parent_scope_env != nullptr) {
        parent_scope_env->assign_new_value_to_variable(var_tok, value);
        return;
    }

    throw RuntimeException(var_tok, "Assignment to non-exist variable.");
}