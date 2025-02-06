
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

// statement → block | whileStmt | ifStmt | exprStmt | printStmt | varStmt |
// assignStmt
std::shared_ptr<Stmt> Parser::parse_stmt() {
    try {
        if (validate_token_and_advance({TokenType::IF})) {
            return parse_if_stmt();
        }
        if (validate_token_and_advance({TokenType::WHILE})) {
            return parse_while_stmt();
        }
        if (validate_token_and_advance({TokenType::LEFT_BRACE})) {
            return parse_block();
        }
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

// whileStmt → "while" expression block
std::shared_ptr<Stmt> Parser::parse_while_stmt() {
    auto condition = parse_expr();

    assert_tok_and_advance(TokenType::LEFT_BRACE,
                           "Expected { after while statement");
    auto body = parse_block();

    return std::make_shared<WhileStmt>(condition, body);
}

// ifStmt -> "if" expression block ("else" block)?
std::shared_ptr<Stmt> Parser::parse_if_stmt() {
    auto condition = parse_expr();

    assert_tok_and_advance(TokenType::LEFT_BRACE,
                           "Expected { after if statement");
    auto if_block = parse_block();

    std::shared_ptr<Stmt> else_block = nullptr;
    if (validate_token_and_advance({TokenType::ELSE})) {
        assert_tok_and_advance(TokenType::LEFT_BRACE,
                               "Expected { after else statement");
        else_block = parse_block();
    }

    return std::make_shared<IfStmt>(condition, if_block, else_block);
}

// block → "{" statement* "}"
std::shared_ptr<Stmt> Parser::parse_block() {
    std::vector<std::shared_ptr<Stmt>> stmts{};

    while (!consumed_all_tokens() and
           get_cur_tok()->type != TokenType::RIGHT_BRACE) {
        stmts.push_back(parse_stmt());
    }

    assert_tok_and_advance(
        TokenType::RIGHT_BRACE,
        "Expected close bracket '}' at the end of the block");

    return std::make_shared<BlockStmt>(stmts);
}

// varStmt → "var" IDENTIFIER ( "=" expression )? ";"
std::shared_ptr<Stmt> Parser::parse_var_stmt() {
    assert_tok_and_advance(TokenType::IDENTIFIER, "Expected a variable name");
    std::shared_ptr<Token> tok_var = get_prev_tok();

    std::shared_ptr<Expr> var_initializer = nullptr;
    LiteralVariant var_value = std::monostate();
    if (validate_token_and_advance({TokenType::EQUAL})) {
        var_initializer = parse_expr();
    }

    assert_tok_and_advance(
        TokenType::SEMICOLON,
        "Expected ; at the end of variable declaration statement");

    return std::make_shared<VarStmt>(tok_var, var_initializer);
}

// printStmt  → "print" expression ";"
std::shared_ptr<Stmt> Parser::parse_print_stmt() {
    auto stmt = std::make_shared<PrintStmt>(parse_expr());
    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; at the end of print statement");
    return stmt;
}

// assignStmt -> "l_value" = "expr" ";"
std::shared_ptr<Stmt> Parser::parse_assign_stmt() {
    std::shared_ptr<Expr> expr = parse_expr();

    if (validate_token_and_advance({TokenType::EQUAL})) {
        // TODO(trung.bc): support complex assignment - obj.x = <value>.
        // For now, only support variable assignment.
        std::shared_ptr<VariableExpr> var =
            std::dynamic_pointer_cast<VariableExpr>(expr);
        if (!var) {
            throw ParserException(
                get_cur_tok(),
                "Expected to assign new value to a variable only");
        }

        // can be both r-value, l-value
        std::shared_ptr<Expr> value = parse_expr();
        assert_tok_and_advance(TokenType::SEMICOLON,
                               "Expected ; at the end of print statement");
        return std::make_shared<AssignStmt>(var, value);
    }

    // TODO(trung.bc): move logic to handle expr stmt out of this func
    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; at the end of expresssion statement");
    return std::make_shared<ExprStmt>(expr);
}

// exprStmt → expression ";" ;
std::shared_ptr<Stmt> Parser::parse_expr_stmt() {
    auto stmt = std::make_shared<ExprStmt>(parse_expr());
    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; at the end of expresssion statement");
    return stmt;
}

// expression → logic_or ;
std::shared_ptr<Expr> Parser::parse_expr() { return parse_logic_or_expr(); }

// logic_or → logic_and ( "or" logic_and )*
std::shared_ptr<Expr> Parser::parse_logic_or_expr() {
    auto left = parse_logic_and_expr();

    while (validate_token_and_advance({TokenType::OR})) {
        auto op = get_prev_tok();
        auto right = parse_logic_and_expr();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

// logic_and → equality ( "and" equality )*
std::shared_ptr<Expr> Parser::parse_logic_and_expr() {
    auto left = parse_equality_expr();

    while (validate_token_and_advance({TokenType::AND})) {
        auto op = get_prev_tok();
        auto right = parse_equality_expr();
        left = std::make_shared<BinaryExpr>(left, op, right);
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
        left = std::make_shared<BinaryExpr>(left, op, right);
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
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

// term → factor ( ( "-" | "+" ) factor )*
std::shared_ptr<Expr> Parser::parse_term() {
    auto left = parse_factor();

    while (validate_token_and_advance({TokenType::MINUS, TokenType::PLUS})) {
        auto op = get_prev_tok();
        auto right = parse_factor();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

// factor → unary ( ( "/" | "*" ) unary )*
std::shared_ptr<Expr> Parser::parse_factor() {
    auto left = parse_unary();

    while (validate_token_and_advance({TokenType::SLASH, TokenType::STAR})) {
        auto op = get_prev_tok();
        auto right = parse_unary();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

// unary → ( "!" | "-" ) unary | primary
std::shared_ptr<Expr> Parser::parse_unary() {
    if (validate_token_and_advance({TokenType::BANG, TokenType::MINUS})) {
        auto op = get_prev_tok();
        std::shared_ptr<Expr> right = parse_unary();
        return std::make_shared<UnaryExpr>(op, right);
    }

    return parse_primary();
}

// primary → IDENTIFIER | NUMBER | STRING | "true" | "false" | "nil" | "("
// expression ")"
std::shared_ptr<Expr> Parser::parse_primary() {
    if (validate_token_and_advance({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(get_prev_tok());
    }
    if (validate_token_and_advance({TokenType::FALSE})) {
        return std::make_shared<LiteralExpr>(false);
    }
    if (validate_token_and_advance({TokenType::TRUE})) {
        return std::make_shared<LiteralExpr>(true);
    }
    if (validate_token_and_advance({TokenType::NIL})) {
        return std::make_shared<LiteralExpr>(std::monostate());
    }
    if (validate_token_and_advance({TokenType::NUMBER, TokenType::STRING})) {
        auto tok = get_prev_tok();
        return std::make_shared<LiteralExpr>(tok->literal);
    }

    if (validate_token_and_advance({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = parse_expr();
        assert_tok_and_advance(TokenType::RIGHT_PAREN,
                               "Expected ) character but not found.");
        return std::make_shared<GroupExpr>(expr);
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

std::shared_ptr<Token> Parser::assert_tok_and_advance(TokenType type,
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