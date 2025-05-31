#pragma once

#include "clox/common/constants.hpp"
#include "clox/common/token.hpp"
#include <memory>
#include <vector>

// Scan the whole file or a single line to return a vector of tokens
class Scanner {
  private:
    std::string src;
    std::vector<std::shared_ptr<Token>> tokens;
    uint lexeme_start_pos = 0;
    uint current_pos = 0;
    uint line = 1;

  public:
    Scanner(std::string src);
    std::vector<std::shared_ptr<Token>> scan_tokens();

  private:
    void scan_token();
    bool is_end_of_src();
    void add_token(TokenType type, ExprVal literal = NIL);
    bool next_char_is(char expected);
    void parse_str();
    void parse_num();
    void parse_identifier();
};