#pragma once
#include "clox/scanner/token.hpp"
#include <map>
#include <memory>

class Environment {
  public:
    std::map<std::string, ExprVal> identifier_table;
    std::shared_ptr<Environment> parent_scope_env;
    Environment();
    Environment(std::shared_ptr<Environment> parent_scope_env);
    void add_new_variable(std::string name, ExprVal value);
};