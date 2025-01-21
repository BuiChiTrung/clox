
#include "parser.hpp"
#include "ast/stmt.hpp"
#include "clox/ast/expr.hpp"
#include "error_manager.hpp"
#include "token.hpp"
#include <iostream>
#include <malloc/_malloc_type.h>
#include <memory>
#include <variant>

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

// program → statement*
std::vector<std::shared_ptr<Stmt>> Parser::parse_program() {
    std::vector<std::shared_ptr<Stmt>> stmts{};
    while (!consumed_all_tokens()) {
        stmts.push_back(parse_stmt());
    }
    return stmts;
}

// statement → exprStmt | printStmt | varStmt | assignStmt
std::shared_ptr<Stmt> Parser::parse_stmt() {
    try {
        if (validate_token_and_advance({TokenType::VAR})) {
            return parse_var_stmt();
        }
        if (validate_token_and_advance({TokenType::PRINT})) {
            return parse_print_stmt();
        }
        // TODO(trung.bc): not use assign stmt as fallback stmt
        return parse_assign_stmt();
        // return parse_expr_stmt();
    }
    catch (ParserException &err) {
        std::cout << err.message;
        panic_mode_synchornize();
        return nullptr;
    }
}

// varStmt → "var" IDENTIFIER ( "=" expression )? ";"
std::shared_ptr<Stmt> Parser::parse_var_stmt() {
    validate_and_throw_err(TokenType::IDENTIFIER, "Expected a variable name");
    std::shared_ptr<Token> tok_var = get_prev_tok();

    std::shared_ptr<Expr> var_initializer = nullptr;
    LiteralVariant var_value = std::monostate();
    if (validate_token_and_advance({TokenType::EQUAL})) {
        var_initializer = parse_expr();
    }

    validate_and_throw_err(
        TokenType::SEMICOLON,
        "Expected ; at the end of variable declaration statement");

    return std::make_shared<VarStmt>(tok_var, var_initializer);
}

// printStmt  → "print" expression ";"
std::shared_ptr<Stmt> Parser::parse_print_stmt() {
    auto stmt = std::make_shared<PrintStmt>(parse_expr());
    validate_and_throw_err(TokenType::SEMICOLON,
                           "Expected ; at the end of print statement");
    return stmt;
}

// assignStmt -> "l_value" = "expr" ";"
std::shared_ptr<Stmt> Parser::parse_assign_stmt() {
    std::shared_ptr<Expr> expr = parse_expr();

    if (validate_token_and_advance({TokenType::EQUAL})) {
        // TODO(trung.bc): support complex assignment - obj.x = <value>.
        // For now, only support variable assignment.
        std::shared_ptr<Variable> var =
            std::dynamic_pointer_cast<Variable>(expr);
        if (!var) {
            throw ParserException(
                get_cur_tok(),
                "Expected to assign new value to a variable only");
        }

        // can be both r-value, l-value
        std::shared_ptr<Expr> value = parse_expr();
        validate_and_throw_err(TokenType::SEMICOLON,
                               "Expected ; at the end of print statement");
        return std::make_shared<AssignStmt>(var, value);
    }

    // TODO(trung.bc): move logic to handle expr stmt out of this func
    validate_and_throw_err(TokenType::SEMICOLON,
                           "Expected ; at the end of expresssion statement");
    return std::make_shared<ExprStmt>(expr);
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

// logic → equality ( ( "and" | "or" ) equality )*
std::shared_ptr<Expr> Parser::parse_logic_expr() {
    auto left = parse_equality_expr();

    while (validate_token_and_advance({TokenType::AND, TokenType::OR})) {
        auto op = get_prev_tok();
        auto right = parse_equality_expr();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// equality → comparison ( ( "!=" | "==" ) comparison )*
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

// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )*
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

// term → factor ( ( "-" | "+" ) factor )*
std::shared_ptr<Expr> Parser::parse_term() {
    auto left = parse_factor();

    while (validate_token_and_advance({TokenType::MINUS, TokenType::PLUS})) {
        auto op = get_prev_tok();
        auto right = parse_factor();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// factor → unary ( ( "/" | "*" ) unary )*
std::shared_ptr<Expr> Parser::parse_factor() {
    auto left = parse_unary();

    while (validate_token_and_advance({TokenType::SLASH, TokenType::STAR})) {
        auto op = get_prev_tok();
        auto right = parse_unary();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// unary → ( "!" | "-" ) unary | primary
std::shared_ptr<Expr> Parser::parse_unary() {
    if (validate_token_and_advance({TokenType::BANG, TokenType::MINUS})) {
        auto op = get_prev_tok();
        std::shared_ptr<Expr> right = parse_unary();
        return std::make_shared<Unary>(op, right);
    }

    return parse_primary();
}

// primary → IDENTIFIER | NUMBER | STRING | "true" | "false" | "nil" | "("
// expression ")"
std::shared_ptr<Expr> Parser::parse_primary() {
    if (validate_token_and_advance({TokenType::IDENTIFIER})) {
        return std::make_shared<Variable>(get_prev_tok());
    }
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

void Parser::panic_mode_synchornize() {
    advance();

    while (!consumed_all_tokens()) {
        if (get_prev_tok()->type == TokenType::SEMICOLON)
            return;

        switch (get_cur_tok()->type) {
        case TokenType::CLASS:
        case TokenType::FUNC:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            advance();
        }
    }
}