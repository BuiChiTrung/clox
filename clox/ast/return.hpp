#pragma once
#include "clox/token.hpp"
#include <exception>

class Return : std::exception {
  public:
    ExprVal return_val;
    Return(ExprVal return_val) : return_val(return_val) {}
};