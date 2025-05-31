#include "clox/common/token.hpp"
#include "clox/scanner/scanner.hpp"
#include <gtest/gtest.h>

// Test: scan_tokens should correctly parse an arithmetic expression
TEST(ScannerTest, ArithmeticExpression) {
    Scanner scanner("3 + 4 * 2");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1]->type, TokenType::PLUS);
    EXPECT_EQ(tokens[2]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[3]->type, TokenType::STAR);
    EXPECT_EQ(tokens[4]->type, TokenType::NUMBER);
}

// Test: scan_tokens should correctly parse a logic expression
TEST(ScannerTest, LogicExpression) {
    Scanner scanner("(3 + 4) > 2 and (4 * 2) < 10 or !(true and false)");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_EQ(tokens.size(), 22);
    EXPECT_EQ(tokens[0]->type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[2]->type, TokenType::PLUS);
    EXPECT_EQ(tokens[3]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4]->type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[5]->type, TokenType::GREATER);
    EXPECT_EQ(tokens[6]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[7]->type, TokenType::AND);
    EXPECT_EQ(tokens[8]->type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[9]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[10]->type, TokenType::STAR);
    EXPECT_EQ(tokens[11]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[12]->type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[13]->type, TokenType::LESS);
    EXPECT_EQ(tokens[14]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[15]->type, TokenType::OR);
    EXPECT_EQ(tokens[16]->type, TokenType::BANG);
    EXPECT_EQ(tokens[17]->type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[18]->type, TokenType::TRUE);
    EXPECT_EQ(tokens[19]->type, TokenType::AND);
    EXPECT_EQ(tokens[20]->type, TokenType::FALSE);
    EXPECT_EQ(tokens[21]->type, TokenType::RIGHT_PAREN);
}

// Test: scan_tokens should ignore comments
TEST(ScannerTest, SingleLineComment) {
    Scanner scanner("// this is a comment\n3 + 4");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1]->type, TokenType::PLUS);
    EXPECT_EQ(tokens[2]->type, TokenType::NUMBER);
}

// Test: parse_str should correctly parse a string
TEST(ScannerTest, ParseString) {
    Scanner scanner("\"hello world\"");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0]->type, TokenType::STRING);
}

// Test: scan_tokens should return an empty vector for an empty source string
TEST(ScannerTest, EmptySource) {
    Scanner scanner("");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_TRUE(tokens.empty());
}

// Test: scan_tokens should correctly identify a single token
TEST(ScannerTest, SingleToken) {
    Scanner scanner("identifier");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0]->lexeme, "identifier");
}

// Test: scan_tokens should correctly handle multiple tokens
TEST(ScannerTest, MultipleTokens) {
    Scanner scanner("var x = 42;");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0]->type, TokenType::VAR);
    EXPECT_EQ(tokens[1]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2]->type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4]->type, TokenType::SEMICOLON);
}

// Test: scan_tokens should handle whitespace correctly
TEST(ScannerTest, Whitespace) {
    Scanner scanner(" \t\nvar\n\t x = 42; \t");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0]->type, TokenType::VAR);
    EXPECT_EQ(tokens[1]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2]->type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3]->type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4]->type, TokenType::SEMICOLON);
}

TEST(ScannerTest, OnlyWhitespace) {
    Scanner scanner(" \t\n ");
    std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
    EXPECT_TRUE(tokens.empty());
}
