// #include "ast/expr.hpp"
// #include "error_manager.hpp"
// #include "token.hpp"
// #include <exception>
// #include <memory>
// class Parser {
//   private:
//     std::vector<Token> tokens;
//     uint32_t current_tok_pos = 0;

//   public:
//     Parser(std::vector<Token> tokens);

//     std::shared_ptr<Expr> parse();
//     std::shared_ptr<Expr> expression();
//     std::shared_ptr<Expr> equality();
//     std::shared_ptr<Expr> comparision();
//     std::shared_ptr<Expr> term();
//     std::shared_ptr<Expr> factor();
//     std::shared_ptr<Expr> unary();
//     std::shared_ptr<Expr> primary();

//     bool consumed_all_tokens();
//     bool match(std::vector<TokenType> tok_types);
//     bool check(TokenType tok_type);
//     Token previous_tok();
//     Token advance();
//     Token peek();
//     Token consume(TokenType type, std::string msg);

//     class Exception : std::exception {};
//     std::shared_ptr<Exception> error(Token tok, std::string msg);
// };

// inline Parser::Parser(std::vector<Token> tokens) : tokens(tokens) {}

// inline std::shared_ptr<Expr> Parser::parse() {
//     try {
//         return expression();
//     }
//     catch (std::shared_ptr<Parser::Exception> e) {
//         return nullptr;
//     }
// }

// // expression → equality ;
// inline std::shared_ptr<Expr> Parser::expression() { return equality(); }

// // equality → comparison ( ( "!=" | "==" ) comparison )* ;
// inline std::shared_ptr<Expr> Parser::equality() {
//     auto left = comparision();

//     while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
//         Token op = previous_tok();
//         auto right = comparision();
//         left = std::make_shared<Binary>(left, op, right);
//     }

//     return left;
// }

// // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// inline std::shared_ptr<Expr> Parser::comparision() {
//     auto left = term();

//     while (match({TokenType::GREATER, TokenType::LESS,
//     TokenType::GREATER_EQUAL,
//                   TokenType::LESS_EQUAL})) {
//         Token op = previous_tok();
//         auto right = term();
//         left = std::make_shared<Binary>(left, op, right);
//     }

//     return left;
// }

// // term → factor ( ( "-" | "+" ) factor )* ;
// inline std::shared_ptr<Expr> Parser::term() {
//     auto left = factor();

//     while (match({TokenType::MINUS, TokenType::PLUS})) {
//         Token op = previous_tok();
//         auto right = factor();
//         left = std::make_shared<Binary>(left, op, right);
//     }

//     return left;
// }

// // factor → unary ( ( "/" | "*" ) unary )* ;
// inline std::shared_ptr<Expr> Parser::factor() {
//     auto left = unary();

//     while (match({TokenType::SLASH, TokenType::STAR})) {
//         Token op = previous_tok();
//         auto right = unary();
//         left = std::make_shared<Binary>(left, op, right);
//     }

//     return left;
// }

// // unary → ( "!" | "-" ) unary | primary ;
// inline std::shared_ptr<Expr> Parser::unary() {
//     if (match({TokenType::BANG, TokenType::MINUS})) {
//         Token op = previous_tok();
//         std::shared_ptr<Expr> right = unary();
//         return std::make_shared<Unary>(op, right);
//     }

//     return primary();
// }

// // primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
// inline std::shared_ptr<Expr> Parser::primary() {
//     if (match({TokenType::FALSE})) {
//         return std::make_shared<Literal>(false);
//     }
//     if (match({TokenType::TRUE})) {
//         return std::make_shared<Literal>(true);
//     }
//     if (match({TokenType::NIL})) {
//         return std::make_shared<Literal>("nil");
//     }
//     if (match({TokenType::NUMBER, TokenType::STRING})) {
//         Token tok = previous_tok();
//         return std::make_shared<Literal>(tok.literal);
//     }

//     if (match({TokenType::LEFT_PAREN})) {
//         std::shared_ptr<Expr> expr = expression();
//         match({TokenType::RIGHT_PAREN});
//         return std::make_shared<Grouping>(expr);
//     }

//     throw error(peek(), "Parser::primary:: Expect expression");
// }

// inline bool Parser::consumed_all_tokens() {
//     return current_tok_pos >= tokens.size();
// }

// inline bool Parser::match(std::vector<TokenType> tok_types) {
//     if (consumed_all_tokens()) {
//         return false;
//     }

//     for (TokenType tok_type : tok_types) {
//         if (tokens[current_tok_pos].type == tok_type) {
//             advance();
//             return true;
//         }
//     }

//     return false;
// }

// inline bool Parser::check(TokenType tok_type) {
//     if (consumed_all_tokens()) {
//         return false;
//     }

//     return peek().type == tok_type;
// }

// inline Token Parser::advance() {
//     if (!consumed_all_tokens()) {
//         current_tok_pos++;
//     }
//     return previous_tok();
// }

// inline Token Parser::peek() { return tokens.at(current_tok_pos); }

// inline Token Parser::previous_tok() { return tokens.at(current_tok_pos - 1);
// }

// inline Token Parser::consume(TokenType type, std::string msg) {
//     if (check(type)) {
//         return advance();
//     }

//     throw error(peek(), msg);
// }

// inline std::shared_ptr<Parser::Exception> error(Token tok, std::string msg) {
//     ErrorManager::err(tok, msg);
//     return std::make_shared<Parser::Exception>();
// }

// /*
//   private void synchronize() {
//     advance();

//     while (!isAtEnd()) {
//       if (previous().type == SEMICOLON) return;

//       switch (peek().type) {
//         case CLASS:
//         case FUN:
//         case VAR:
//         case FOR:
//         case IF:
//         case WHILE:
//         case PRINT:
//         case RETURN:
//           return;
//       }

//       advance();
//     }
//   }
// */