#include "../token.hpp"

class Expr {};

class Binary : public Expr {
private:
  Expr left;
  Token op;
  Expr right;

public:
  Binary(Expr left, Token op, Expr right) {
    this->left = left;
    this->op = op;
    this->right = right;
  }
};