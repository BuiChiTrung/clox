#pragma once
#include "clox/token.hpp"
#include <map>
#include <memory>

class Environment {
  private:
    std::map<std::string, LiteralVariant> variable_table = {};

  public:
    void add_new_variable(std::shared_ptr<Token> name, LiteralVariant value);
    LiteralVariant get_variable(std::shared_ptr<Token> var);
};