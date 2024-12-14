#include "../ast/expr.hpp"
#include "../ast/printer_visitor.hpp"
#include "../token.hpp"
#include <iostream>
#include <memory>

int main() {
    PrinterVisitor printer_visitor = PrinterVisitor();
    auto t = std::make_shared<Token>(MINUS, "-", "", 1);
    std::shared_ptr<Expr> l = std::make_shared<Literal>(std::string("123"));
    std::shared_ptr<Expr> u = std::make_shared<Unary>(t, l);

    auto t1 = std::make_shared<Token>(STAR, "*", "", 1);
    std::shared_ptr<Expr> l1 = std::make_shared<Literal>(45.67);
    std::shared_ptr<Expr> g = std::make_shared<Grouping>(l1);

    std::shared_ptr<Expr> b = std::make_shared<Binary>(u, t1, g);

    std::cout << std::get<std::string>(b->accept(printer_visitor));
}