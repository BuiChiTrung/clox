#include "../ast/expr.hpp"
#include "../ast/printer_visitor.hpp"
#include "../token.hpp"
#include <iostream>

int main() {
    PrinterVisitor printer_visitor = PrinterVisitor();
    // Token t = Token(MINUS, "-", "", 1);
    // Literal l = Literal(123);
    // Unary u = Unary(t, l);

    // Token t1 = Token(STAR, "*", "", 1);

    // Literal l1 = Literal(45.67);
    // Grouping g = Grouping(l1);

    // Binary b = Binary(u, t1, g);

    // // std::cout << b.accept(printer_visitor);

    // Expr e = Literal("12");
    // std::cout << std::get<std::string>(e.accept(printer_visitor));
    // std::cout << l1.accept(printer_visitor);

    std::shared_ptr<Literal> l = std::make_shared<Literal>(std::string("abc"));
    std::cout << std::get<std::string>(l->accept(printer_visitor));
}