#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/middleware/identifier_resolver.hpp"
#include "clox/parser/parser.hpp"
#include "clox/scanner/scanner.hpp"
#include <gtest/gtest.h>
#include <memory>

class AstInterpreterTest : public testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

void evaluateExpression(const std::string &source, const ExprVal &expected) {
    Scanner scanner{source};
    auto tokens = scanner.scan_tokens();

    Parser parser{tokens};
    auto expression = parser.parse_single_expr();

    auto interpreter = std::make_shared<AstInterpreter>(false);
    auto actual = interpreter->interpret_single_expr(*expression);

    ASSERT_EQ(actual, expected);
}

TEST_F(AstInterpreterTest, EvaluatesLiteral) {
    evaluateExpression("\"Cafe treebee\"", "Cafe treebee");
    evaluateExpression("522001", 522001.0);
    evaluateExpression("true", true);
}

TEST_F(AstInterpreterTest, EvaluatesUnaryExpression) {
    evaluateExpression("-5 * 3", -15.0f);
    evaluateExpression("!true", false);
    evaluateExpression("!false", true);
    evaluateExpression("-(-3)", 3.0f);
}

TEST_F(AstInterpreterTest, EvaluatesGroupingExpression) {
    evaluateExpression("(5 + 3) * 2", 16.0f);
    evaluateExpression("!(false or true)", false);
    evaluateExpression("!(true and false)", true);
}

TEST_F(AstInterpreterTest, EvaluatesBinaryExpression) {
    evaluateExpression("(1 + 2) % 3", 0.0);
    evaluateExpression("5 + (3 * 2) - 4 / 2", 9.0f);
    evaluateExpression("true and false", false);
    evaluateExpression("true or false", true);
    evaluateExpression("\"Hello, \" + \"world!\"",
                       std::string("Hello, world!"));
    evaluateExpression("true == false", false);
}

TEST_F(AstInterpreterTest, EvaluatesComparisonExpression) {
    evaluateExpression("5 < 10", true);
    evaluateExpression("10 > 5", true);
    evaluateExpression("5 <= 5", true);
    evaluateExpression("5 >= 5", true);
    evaluateExpression("5 == 5", true);
    evaluateExpression("5 != 6", true);
    evaluateExpression("\"abc\" < \"def\"", true);
    evaluateExpression("\"abc\" > \"def\"", false);
}
