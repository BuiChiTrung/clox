#pragma once
#include "clox/token.hpp"
#include <map>
#include <memory>

class Environment {
  private:
    std::map<std::string, LiteralVariant> variable_table = {};

  public:
    void add_new_variable(std::shared_ptr<Token> name, LiteralVariant value);
    LiteralVariant get_variable(std::shared_ptr<Token> var_tok);
    void assign_new_value_to_variable(std::shared_ptr<Token> var_tok,
                                      LiteralVariant value);
};