#pragma once
#include "clox/common/expr_val.hpp"
#include "clox/common/token.hpp"
#include <memory>
#include <unordered_map>

class Environment {
  private:
    std::unordered_map<std::string, ExprVal> identifier_table = {};

  public:
    std::shared_ptr<Environment> parent_scope_env = nullptr;

    Environment();
    Environment(std::shared_ptr<Environment> parent_scope_env);

    void add_identifier(std::string name, const ExprVal &value);
    void update_identifier(std::string name, const ExprVal &value,
                           std::shared_ptr<Token> token);
    ExprVal get_identifier(std::string name, std::shared_ptr<Token> token);

    friend class IdentifierResolver;
};