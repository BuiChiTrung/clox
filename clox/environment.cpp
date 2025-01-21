#include "environment.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include <format>
#include <memory>

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
    if (!variable_table.count(var_name)) {
        throw RuntimeException(var_tok,
                               "Error: reference to non-exist variable.");
    }

    return variable_table[var_name];
}

void Environment::assign_new_value_to_variable(std::shared_ptr<Token> var_tok,
                                               LiteralVariant value) {
    std::string var_name = var_tok->lexeme;
    if (!variable_table.count(var_name)) {
        throw RuntimeException(var_tok,
                               "Error: assignment to non-exist variable.");
    }

    variable_table[var_name] = value;
}