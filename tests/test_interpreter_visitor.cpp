#include "../ast/interpreter_visitor.hpp"
#include "../clox/parser.hpp"
#include "../clox/scanner.hpp"
#include <gtest/gtest.h>

class InterpreterVisitorTest : public testing::Test {
  protected:
    void SetUp() override {
        // Setup code if needed
    }
    void TearDown() override {
        // Cleanup code if needed
    }
};

void evaluateExpression(const std::string &source,
                        const LiteralVariant &expected) {
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