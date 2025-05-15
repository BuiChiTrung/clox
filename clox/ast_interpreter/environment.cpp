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
