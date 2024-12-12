#include "../token.hpp"

class Expr {};

class Binary : public Expr{
  private:
	Expr left;
	Token op;
	Expr right;
};

class Grouping : public Expr{
  private:
	Expr expression;
};

class Literal : public Expr{
  private:
	std::any value;
};

class Unary : public Expr{
  private:
	Token op;
	Expr right;
};

