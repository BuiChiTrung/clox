
#include "parser.hpp"
#include "../ast/stmt.hpp"
#include "error_manager.hpp"
#include "token.hpp"
#include <memory>

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens) : tokens(tokens) {}

std::shared_ptr<Expr> Parser::parse_single_expr() {
    try {
        return parse_expr();
    }
    catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        return nullptr;
    }
}

// program → statement*;
std::vector<std::shared_ptr<Stmt>> Parser::parse_program() {
    std::vector<std::shared_ptr<Stmt>> stmts{};
    try {
        while (!consumed_all_tokens()) {
            stmts.push_back(parse_stmt());
        }
        return stmts;
    }
    catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        return stmts;
    }
}

// statement → exprStmt | printStmt ;
std::shared_ptr<Stmt> Parser::parse_stmt() {
    if (validate_token_and_advance({TokenType::PRINT})) {
        return parse_print_stmt();
    }
    return parse_expr_stmt();
}

// printStmt  → "print" expression ";" ;
std::shared_ptr<Stmt> Parser::parse_print_stmt() {
    auto stmt = std::make_shared<PrintStmt>(parse_expr());
    validate_and_throw_err(TokenType::SEMICOLON,
                           "Expected ; at the end of print statement");
    return stmt;
}

// exprStmt → expression ";" ;
std::shared_ptr<Stmt> Parser::parse_expr_stmt() {
    auto stmt = std::make_shared<ExprStmt>(parse_expr());
    validate_and_throw_err(TokenType::SEMICOLON,
                           "Expected ; at the end of expresssion statement");
    return stmt;
}

// expression → logic ;
std::shared_ptr<Expr> Parser::parse_expr() { return parse_logic_expr(); }

// logic → equality ( ( "and" | "or" ) equality )* ;
std::shared_ptr<Expr> Parser::parse_logic_expr() {
    auto left = parse_equality_expr();

    while (validate_token_and_advance({TokenType::AND, TokenType::OR})) {
        auto op = get_prev_tok();
        auto right = parse_equality_expr();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// equality → comparison ( ( "!=" | "==" ) comparison )* ;
std::shared_ptr<Expr> Parser::parse_equality_expr() {
    auto left = parse_comparision_expr();

    while (validate_token_and_advance(
        {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = get_prev_tok();
        auto right = parse_comparision_expr();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
std::shared_ptr<Expr> Parser::parse_comparision_expr() {
    auto left = parse_term();

    while (validate_token_and_advance({TokenType::GREATER, TokenType::LESS,
                                       TokenType::GREATER_EQUAL,
                                       TokenType::LESS_EQUAL})) {
        auto op = get_prev_tok();
        auto right = parse_term();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// term → factor ( ( "-" | "+" ) factor )* ;
std::shared_ptr<Expr> Parser::parse_term() {
    auto left = parse_factor();

    while (validate_token_and_advance({TokenType::MINUS, TokenType::PLUS})) {
        auto op = get_prev_tok();
        auto right = parse_factor();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// factor → unary ( ( "/" | "*" ) unary )* ;
std::shared_ptr<Expr> Parser::parse_factor() {
    auto left = parse_unary();

    while (validate_token_and_advance({TokenType::SLASH, TokenType::STAR})) {
        auto op = get_prev_tok();
        auto right = parse_unary();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// unary → ( "!" | "-" ) unary | primary ;
std::shared_ptr<Expr> Parser::parse_unary() {
    if (validate_token_and_advance({TokenType::BANG, TokenType::MINUS})) {
        auto op = get_prev_tok();
        std::shared_ptr<Expr> right = parse_unary();
        return std::make_shared<Unary>(op, right);
    }

    return parse_primary();
}

// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
std::shared_ptr<Expr> Parser::parse_primary() {
    if (validate_token_and_advance({TokenType::FALSE})) {
        return std::make_shared<Literal>(false);
    }
    if (validate_token_and_advance({TokenType::TRUE})) {
        return std::make_shared<Literal>(true);
    }
    if (validate_token_and_advance({TokenType::NIL})) {
        return std::make_shared<Literal>("nil");
    }
    if (validate_token_and_advance({TokenType::NUMBER, TokenType::STRING})) {
        auto tok = get_prev_tok();
        return std::make_shared<Literal>(tok->literal);
    }

    if (validate_token_and_advance({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = parse_expr();
        validate_and_throw_err(TokenType::RIGHT_PAREN,
                               "Expected ) character but not found.");
        return std::make_shared<Grouping>(expr);
    }

    throw ParserException(get_cur_tok(),
                          "Parsering primary error: Expect expression");
}

bool Parser::consumed_all_tokens() { return current_tok_pos >= tokens.size(); }

bool Parser::validate_token_and_advance(std::vector<TokenType> tok_types) {
    if (consumed_all_tokens()) {
        return false;
    }

    for (TokenType tok_type : tok_types) {
        if (tokens[current_tok_pos]->type == tok_type) {
            advance();
            return true;
        }
    }

    return false;
}

// get current token and move to the next tok
std::shared_ptr<Token> Parser::advance() {
    if (!consumed_all_tokens()) {
        current_tok_pos++;
    }
    return get_prev_tok();
}

// get current token without moving to the next tok
std::shared_ptr<Token> Parser::get_cur_tok() {
    return tokens.at(current_tok_pos);
}

std::shared_ptr<Token> Parser::get_prev_tok() {
    return tokens.at(current_tok_pos - 1);
}

// check current token type before advance
std::shared_ptr<Token> Parser::validate_and_throw_err(TokenType type,
                                                      std::string msg) {
    if (consumed_all_tokens() || get_cur_tok()->type != type) {
        throw ParserException(get_cur_tok(), msg);
    }
    return advance();
}
