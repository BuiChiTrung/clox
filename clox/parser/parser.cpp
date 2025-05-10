
#include "clox/parser/parser.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include <memory>
#include <vector>

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens) : tokens(tokens) {}

std::shared_ptr<Expr> Parser::parse_single_expr() {
    try {
        return parse_expr();
    } catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        return nullptr;
    }
}

// program → declaration*
std::vector<std::shared_ptr<Stmt>> Parser::parse_program() {
    std::vector<std::shared_ptr<Stmt>> stmts{};
    while (!consumed_all_tokens()) {
        stmts.push_back(parse_declaration());
    }
    return stmts;
}

// declaration → varDecl | funcDecl | statement
std::shared_ptr<Stmt> Parser::parse_declaration() {
    try {
        if (validate_token(TokenType::VAR)) {
            return parse_var_decl();
        }
        if (validate_token(TokenType::FUNC)) {
            return parse_function_decl();
        }
        return parse_stmt();
    } catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        panic_mode_synchornize();
        return nullptr;
    }
}

// statement → block | forStmt | whileStmt | ifStmt | exprStmt | printStmt |
// assignStmt | returnStmt
std::shared_ptr<Stmt> Parser::parse_stmt() {
    if (validate_token(TokenType::PRINT)) {
        return parse_print_stmt();
    }
    if (validate_token(TokenType::LEFT_BRACE)) {
        return parse_block_stmt();
    }
    if (validate_token(TokenType::WHILE)) {
        return parse_while_stmt();
    }
    if (validate_token(TokenType::FOR)) {
        return parse_for_stmt();
    }
    if (validate_token(TokenType::IF)) {
        return parse_if_stmt();
    }
    if (validate_token(TokenType::RETURN)) {
        return parse_return_stmt();
    }
    return parse_assign_stmt();
}

// returnStmt → return expression? ";"
std::shared_ptr<Stmt> Parser::parse_return_stmt() {
    assert_tok_and_advance(TokenType::RETURN, "Expected return statement");

    std::shared_ptr<Token> return_kw = get_prev_tok();
    std::shared_ptr<Expr> expr = nullptr;

    if (!validate_token(TokenType::SEMICOLON)) {
        expr = parse_expr();
    }

    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ';' at the end of return statement");
    return std::make_shared<ReturnStmt>(return_kw, expr);
}

// funcDecl → IDENTIFIER "(" parameters ")" block ;
std::shared_ptr<Stmt> Parser::parse_function_decl() {
    assert_tok_and_advance(TokenType::FUNC, "Expected function declaration");

    std::shared_ptr<Token> func_name =
        assert_tok_and_advance(TokenType::IDENTIFIER, "Expected function name");

    std::shared_ptr<Token> left_parenthesis = assert_tok_and_advance(
        TokenType::LEFT_PAREN, "Expected '(' after function name");

    std::vector<std::shared_ptr<IdentifierExpr>> func_params =
        parse_func_params();

    if (!validate_token_and_advance({TokenType::RIGHT_PAREN})) {
        throw ParserException(left_parenthesis,
                              "Expected ')' at the end "
                              "of function params list to match '('");
    }

    std::shared_ptr<Stmt> func_body = parse_block_stmt();

    return std::make_shared<FunctionDecl>(func_name, func_params, func_body);
}

// parameters -> "" | (IDENTIFIER (","IDENTIFIER)*)
std::vector<std::shared_ptr<IdentifierExpr>> Parser::parse_func_params() {
    if (validate_token(TokenType::RIGHT_PAREN)) {
        return {};
    }

    std::vector<std::shared_ptr<IdentifierExpr>> params{};
    do {
        std::shared_ptr<Token> var_name = assert_tok_and_advance(
            TokenType::IDENTIFIER, "Expected function parameter");
        params.push_back(std::make_shared<IdentifierExpr>(var_name));
    } while (validate_token_and_advance({TokenType::COMMA}));

    if (params.size() > MAX_ARGS_NUM) {
        ErrorManager::handle_err(
            get_cur_tok(),
            "Error: Number of params for function exceed limit " +
                std::to_string(MAX_ARGS_NUM));
    }

    return params;
}

// forStmt → "for" (varDecl | assignStmt | ";") (expression)? ";" (assignStmt)?
// block
std::shared_ptr<Stmt> Parser::parse_for_stmt() {
    assert_tok_and_advance(TokenType::FOR, "Expected for loop");

    std::shared_ptr<Stmt> initializer;
    if (validate_token_and_advance({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (validate_token(TokenType::VAR)) {
        initializer = parse_var_decl();
    } else {
        initializer = parse_assign_stmt();
    }

    std::shared_ptr<Expr> condition(new LiteralExpr(true));
    if (!validate_token(TokenType::SEMICOLON)) {
        condition = parse_expr();
    }
    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; after for loop condition.");

    std::shared_ptr<Stmt> increment = nullptr;
    if (!validate_token(TokenType::LEFT_BRACE)) {
        increment = parse_assign_stmt();
    }

    auto body = std::dynamic_pointer_cast<BlockStmt>(parse_block_stmt());
    if (increment != nullptr) {
        body->stmts.push_back(increment);
    }

    std::shared_ptr<WhileStmt> while_stmt(new WhileStmt(condition, body));
    if (initializer != nullptr) {
        std::vector<std::shared_ptr<Stmt>> stmts = {initializer, while_stmt};
        std::shared_ptr<BlockStmt> for_stmt(new BlockStmt(stmts));
        return for_stmt;
    }
    return while_stmt;
}

// whileStmt → "while" expression block
std::shared_ptr<Stmt> Parser::parse_while_stmt() {
    assert_tok_and_advance(TokenType::WHILE, "Expected while loop");
    auto condition = parse_expr();

    auto body = parse_block_stmt();

    return std::make_shared<WhileStmt>(condition, body);
}

// ifStmt -> "if" expression block ("elif" block)+ ("else" block)?
std::shared_ptr<Stmt> Parser::parse_if_stmt() {
    assert_tok_and_advance(TokenType::IF, "Expected if statement");
    static std::vector<std::shared_ptr<Expr>> conditions;
    static std::vector<std::shared_ptr<Stmt>> if_blocks;
    conditions.push_back(parse_expr());
    if_blocks.push_back(parse_block_stmt());

    while (validate_token_and_advance({TokenType::ELIF})) {
        conditions.push_back(parse_expr());
        if_blocks.push_back(parse_block_stmt());
    }

    std::shared_ptr<Stmt> else_block = nullptr;
    if (validate_token_and_advance({TokenType::ELSE})) {
        else_block = parse_block_stmt();
    }

    return std::make_shared<IfStmt>(conditions, if_blocks, else_block);
}

// block → "{" statement* "}"
std::shared_ptr<Stmt> Parser::parse_block_stmt() {
    assert_tok_and_advance(TokenType::LEFT_BRACE,
                           "Expected block of statements wrapped inside '{}'");

    auto left_brace = get_prev_tok();
    std::vector<std::shared_ptr<Stmt>> stmts{};

    while (!consumed_all_tokens() and !validate_token(TokenType::RIGHT_BRACE)) {
        stmts.push_back(parse_declaration());
    }

    if (!validate_token_and_advance({TokenType::RIGHT_BRACE})) {
        throw ParserException(left_brace,
                              "Expected close bracket '}' at the end "
                              "of the block to match '{'");
    }

    return std::make_shared<BlockStmt>(stmts);
}

// varDecl → "var" IDENTIFIER ( "=" expression )? ";"
std::shared_ptr<Stmt> Parser::parse_var_decl() {
    assert_tok_and_advance(TokenType::VAR, "Expected 'var'");
    assert_tok_and_advance(TokenType::IDENTIFIER, "Expected a variable name");
    std::shared_ptr<Token> tok_var = get_prev_tok();

    std::shared_ptr<Expr> var_initializer = nullptr;
    ExprVal var_value = NIL;
    if (validate_token_and_advance({TokenType::EQUAL})) {
        var_initializer = parse_expr();
    }

    assert_tok_and_advance(
        TokenType::SEMICOLON,
        "Expected ; at the end of variable declaration statement");

    return std::make_shared<VarDecl>(tok_var, var_initializer);
}

// printStmt  → "print" expression ";"
std::shared_ptr<Stmt> Parser::parse_print_stmt() {
    assert_tok_and_advance(TokenType::PRINT, "Expected 'print'");
    // assert_tok_and_advance(TokenType::LEFT_PAREN, "Expected '(' after
    // print");
    std::shared_ptr<PrintStmt> stmt(new PrintStmt(parse_expr()));
    // assert_tok_and_advance(TokenType::RIGHT_PAREN, "Expected ')' after
    // print");
    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; at the end of print statement");
    return stmt;
}

// assignStmt -> l_value "=" expression";" | exprStmt
// exprStmt → expression ";" ;
std::shared_ptr<Stmt> Parser::parse_assign_stmt() {
    std::shared_ptr<Expr> expr = parse_expr();

    if (validate_token_and_advance({TokenType::EQUAL})) {
        // TODO(trung.bc): support complex assignment - obj.x = <value>.
        // For now, only support variable assignment.
        std::shared_ptr<IdentifierExpr> var =
            std::dynamic_pointer_cast<IdentifierExpr>(expr);
        if (!var) {
            throw ParserException(
                get_cur_tok(),
                "Expected to assign new value to a variable only");
        }

        // can be both r-value, l-value
        std::shared_ptr<Expr> value = parse_expr();
        assert_tok_and_advance(TokenType::SEMICOLON,
                               "Expected ; at the end of assign statement");
        return std::make_shared<AssignStmt>(var, value);
    }

    assert_tok_and_advance(TokenType::SEMICOLON,
                           "Expected ; at the end of assign statement");
    return std::make_shared<ExprStmt>(expr);
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

// factor → unary ( ( "/" | "*" | "%" ) unary )*
std::shared_ptr<Expr> Parser::parse_factor() {
    auto left = parse_unary();

    while (validate_token_and_advance(
        {TokenType::SLASH, TokenType::STAR, TokenType::MOD})) {
        auto op = get_prev_tok();
        auto right = parse_unary();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

// unary → ( "!" | "-" ) unary | call
std::shared_ptr<Expr> Parser::parse_unary() {
    if (validate_token_and_advance({TokenType::BANG, TokenType::MINUS})) {
        auto op = get_prev_tok();
        std::shared_ptr<Expr> right = parse_unary();
        return std::make_shared<UnaryExpr>(op, right);
    }

    return parse_call();
}

// call → primary ("(" arguments ")")*
std::shared_ptr<Expr> Parser::parse_call() {
    std::shared_ptr<Expr> func_call_expr = parse_primary();

    while (validate_token_and_advance({TokenType::LEFT_PAREN})) {
        std::vector<std::shared_ptr<Expr>> arguments =
            parse_func_call_arguments();

        assert_tok_and_advance(TokenType::RIGHT_PAREN,
                               "Expected ')' after function invocation");
        std::shared_ptr<Token> right_paren = get_prev_tok();

        func_call_expr = std::make_shared<FuncCallExpr>(func_call_expr,
                                                        right_paren, arguments);
    }

    return func_call_expr;
}

// arguments -> "" | (expression (","expression)*)
std::vector<std::shared_ptr<Expr>> Parser::parse_func_call_arguments() {
    if (validate_token(TokenType::RIGHT_PAREN)) {
        return {};
    }

    std::vector<std::shared_ptr<Expr>> args{};
    do {
        args.push_back(parse_expr());
    } while (validate_token_and_advance({TokenType::COMMA}));

    if (args.size() > MAX_ARGS_NUM) {
        ErrorManager::handle_err(
            get_cur_tok(),
            "Error: Number of arguments for function call exceed limit " +
                std::to_string(MAX_ARGS_NUM));
    }

    return args;
}

// primary → IDENTIFIER | NUMBER | STRING | "true" | "false" | "nil" | "("
// expression ")"
std::shared_ptr<Expr> Parser::parse_primary() {
    if (validate_token_and_advance({TokenType::IDENTIFIER})) {
        return std::make_shared<IdentifierExpr>(get_prev_tok());
    }
    if (validate_token_and_advance({TokenType::FALSE})) {
        return std::make_shared<LiteralExpr>(false);
    }
    if (validate_token_and_advance({TokenType::TRUE})) {
        return std::make_shared<LiteralExpr>(true);
    }
    if (validate_token_and_advance({TokenType::NIL})) {
        return std::make_shared<LiteralExpr>(NIL);
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
        if (validate_token(tok_type)) {
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
    if (consumed_all_tokens()) {
        return std::make_shared<Token>();
    }
    return tokens.at(current_tok_pos);
}

std::shared_ptr<Token> Parser::get_prev_tok() {
    if (current_tok_pos - 1 < 0) {
        throw ParserException(get_cur_tok(),
                              "Error: get previous token at invalid position");
    }
    return tokens.at(current_tok_pos - 1);
}

std::shared_ptr<Token> Parser::assert_tok_and_advance(TokenType type,
                                                      std::string msg) {
    if (!validate_token(type)) {
        throw ParserException(get_cur_tok(), msg);
    }
    return advance();
}

bool Parser::validate_token(TokenType tok_type) {
    return !consumed_all_tokens() and get_cur_tok()->type == tok_type;
}

void Parser::panic_mode_synchornize() {

    while (!consumed_all_tokens()) {
        switch (get_cur_tok()->type) {
        case TokenType::SEMICOLON:
            advance();
            return;
        case TokenType::CLASS:
        case TokenType::FUNC:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
        case TokenType::LEFT_BRACE:
            return;
        default:
            advance();
        }
    }
}