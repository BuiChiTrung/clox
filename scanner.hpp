#pragma once
#include "token.hpp"
#include <vector>

class Scanner {
private:
  std::string src;
  std::vector<Token> tokens;
  uint lexeme_start_pos = 0;
  uint current_pos = 0;
  uint line = 1;

public:
  Scanner(std::string src);
  std::vector<Token> scan_tokens();

private:
  void scan_token();
  bool is_end_of_src();
  void add_token(TokenType type, std::string literal = "");
  bool next_char_is(char expected);
  void parse_str();
  void parse_num();
  void parse_identifier();
};