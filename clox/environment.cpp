#include "environment.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include <format>
#include <memory>

void Environment::add_new_variable(std::shared_ptr<Token> tok_var,
                                   LiteralVariant value) {
    std::string name = tok_var->lexeme;
    if (variable_table.count(name)) {
        throw RuntimeException(
            nullptr,
            std::format("Error: Variable {} is already declared.", name));
    }

    variable_table[name] = value;
}

LiteralVariant Environment::get_variable(std::shared_ptr<Token> var) {
    if (!variable_table.count(var->lexeme)) {
        throw RuntimeException(var, "Error: reference to non-exist variable.");
    }

    return variable_table[var->lexeme];
}