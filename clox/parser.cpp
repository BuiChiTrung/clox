
#include "parser.hpp"
#include "error_manager.hpp"

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens) : tokens(tokens) {}

std::shared_ptr<Expr> Parser::parse() {
    try {
        return expression();
    }
    catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        return nullptr;
    }
}

// expression → equality ;
std::shared_ptr<Expr> Parser::expression() { return logic(); }

// logic → equality ( ( "and" | "or" ) equality )* ;
std::shared_ptr<Expr> Parser::logic() {
    auto left = equality();

    while (match({TokenType::AND, TokenType::OR})) {
        auto op = get_previous_tok();
        auto right = equality();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// equality → comparison ( ( "!=" | "==" ) comparison )* ;
std::shared_ptr<Expr> Parser::equality() {
    auto left = comparision();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = get_previous_tok();
        auto right = comparision();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
std::shared_ptr<Expr> Parser::comparision() {
    auto left = term();

    while (match({TokenType::GREATER, TokenType::LESS, TokenType::GREATER_EQUAL,
                  TokenType::LESS_EQUAL})) {
        auto op = get_previous_tok();
        auto right = term();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// term → factor ( ( "-" | "+" ) factor )* ;
std::shared_ptr<Expr> Parser::term() {
    auto left = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = get_previous_tok();
        auto right = factor();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// factor → unary ( ( "/" | "*" ) unary )* ;
std::shared_ptr<Expr> Parser::factor() {
    auto left = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = get_previous_tok();
        auto right = unary();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}

// unary → ( "!" | "-" ) unary | primary ;
std::shared_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = get_previous_tok();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(op, right);
    }

    return primary();
}

// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) {
        return std::make_shared<Literal>(false);
    }
    if (match({TokenType::TRUE})) {
        return std::make_shared<Literal>(true);
    }
    if (match({TokenType::NIL})) {
        return std::make_shared<Literal>("nil");
    }
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        auto tok = get_previous_tok();
        return std::make_shared<Literal>(tok->literal);
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ) character but not found.");
        return std::make_shared<Grouping>(expr);
    }

    throw ParserException(peek(), "Parsering primary error: Expect expression");
}

bool Parser::consumed_all_tokens() { return current_tok_pos >= tokens.size(); }

bool Parser::match(std::vector<TokenType> tok_types) {
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

bool Parser::is_cur_tok_type_match(TokenType tok_type) {
    if (consumed_all_tokens()) {
        return false;
    }

    return peek()->type == tok_type;
}

// get current token and move to the next tok
std::shared_ptr<Token> Parser::advance() {
    if (!consumed_all_tokens()) {
        current_tok_pos++;
    }
    return get_previous_tok();
}

// get current token without moving to the next tok
std::shared_ptr<Token> Parser::peek() { return tokens.at(current_tok_pos); }

std::shared_ptr<Token> Parser::get_previous_tok() {
    return tokens.at(current_tok_pos - 1);
}

// check current token type before advance
std::shared_ptr<Token> Parser::consume(TokenType type, std::string msg) {
    if (is_cur_tok_type_match(type)) {
        return advance();
    }

    throw ParserException(peek(), msg);
}
