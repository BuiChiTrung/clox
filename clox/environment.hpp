#pragma once
#include "clox/token.hpp"
#include <map>
#include <memory>

class Environment {
  private:
    std::map<std::string, LiteralVariant> variable_table;
    std::shared_ptr<Environment> parent_scope_env;

  public:
    Environment();
    Environment(std::shared_ptr<Environment> parent_scope_env);
    void add_new_variable(std::string name, LiteralVariant value);
    LiteralVariant get_variable(std::shared_ptr<Token> var_tok);
    void assign_new_value_to_variable(std::shared_ptr<Token> var_tok,
                                      LiteralVariant value);
};