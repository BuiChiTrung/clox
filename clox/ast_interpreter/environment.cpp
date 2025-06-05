#include "clox/ast_interpreter/environment.hpp"
#include "clox/common/error_manager.hpp"
#include <memory>

// TODO(trung.bc): should use default constructor or init in hpp
Environment::Environment() {
    identifier_table = {};
    parent_scope_env = nullptr;
}

Environment::Environment(std::shared_ptr<Environment> parent_scope_env)
    : parent_scope_env(parent_scope_env) {}

void Environment::add_identifier(std::string name, const ExprVal &value) {
    identifier_table[name] = value;
}

void Environment::update_identifier(std::string name, const ExprVal &value,
                                    std::shared_ptr<Token> token) {
    try {
        identifier_table.at(name) = value;
    } catch (const std::out_of_range &e) {
        throw RuntimeException(token, "Cannot update undefined identifier '" +
                                          name + "'");
    }
}

ExprVal Environment::get_identifier(std::string name,
                                    std::shared_ptr<Token> token) {
    try {
        return identifier_table.at(name);
    } catch (const std::out_of_range &e) {
        throw RuntimeException(token, "Undefined indentifier '" + name + "'");
    }
}