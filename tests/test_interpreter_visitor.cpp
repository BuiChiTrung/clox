#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/parser/parser.hpp"
#include "clox/scanner/scanner.hpp"
#include <gtest/gtest.h>

class InterpreterVisitorTest : public testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

void evaluateExpression(const std::string &source, const ExprVal &expected) {
    Scanner scanner{source};
    auto tokens = scanner.scan_tokens();

    Parser parser{tokens};
    auto expression = parser.parse_single_expr();

    InterpreterVisitor interpreter;
    auto actual = interpreter.interpret_single_expr(expression);

    ASSERT_EQ(actual, expected);
}

TEST_F(InterpreterVisitorTest, EvaluatesLiteral) {
    evaluateExpression("\"Cafe treebee\"", "Cafe treebee");
    evaluateExpression("522001", 522001.0);
    evaluateExpression("true", true);
}

TEST_F(InterpreterVisitorTest, EvaluatesUnaryExpression) {
    evaluateExpression("-5 * 3", -15.0f);
    evaluateExpression("!true", false);
    evaluateExpression("!false", true);
    evaluateExpression("-(-3)", 3.0f);
}

TEST_F(InterpreterVisitorTest, EvaluatesGroupingExpression) {
    evaluateExpression("(5 + 3) * 2", 16.0f);
    evaluateExpression("!(false or true)", false);
    evaluateExpression("!(true and false)", true);
}

TEST_F(InterpreterVisitorTest, EvaluatesBinaryExpression) {
    evaluateExpression("(1 + 2) * 3", 9.0f);
    evaluateExpression("5 + (3 * 2) - 4 / 2", 9.0f);
    evaluateExpression("true and false", false);
    evaluateExpression("true or false", true);
    evaluateExpression("\"Hello, \" + \"world!\"",
                       std::string("Hello, world!"));
    evaluateExpression("true == false", false);
}