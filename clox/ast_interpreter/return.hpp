#pragma once
#include "clox/scanner/token.hpp"
#include <exception>

class Return : std::exception {
  public:
    ExprVal return_val;
    Return(ExprVal return_val) : return_val(return_val) {}
};