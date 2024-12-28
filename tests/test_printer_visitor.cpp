#include "../ast/expr.hpp"
#include "../ast/printer_visitor.hpp"
#include "../clox/token.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(PrinterVisitorTest, BasicTest) {
    PrinterVisitor printer_visitor = PrinterVisitor();
    auto t = std::make_shared<Token>(TokenType::MINUS, "-", "", 1);
    std::shared_ptr<Expr> l = std::make_shared<Literal>(std::string("123"));
    std::shared_ptr<Expr> u = std::make_shared<Unary>(t, l);

    auto t1 = std::make_shared<Token>(TokenType::STAR, "*", "", 1);
    std::shared_ptr<Expr> l1 = std::make_shared<Literal>(45.67);
    std::shared_ptr<Expr> g = std::make_shared<Grouping>(l1);

    std::shared_ptr<Expr> b = std::make_shared<Binary>(u, t1, g);

    std::string expected = "(* (- 123) (group 45.670000))";
    std::string actual = std::get<std::string>(b->accept(printer_visitor));

    EXPECT_EQ(actual, expected);
}
