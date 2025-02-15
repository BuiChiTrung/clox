#include "clox/scanner/scanner.hpp"
#include "clox/error_manager/error_manager.hpp"
#include <cctype>
#include <memory>
#include <string>

Scanner::Scanner(std::string src) { this->src = src; }

std::vector<std::shared_ptr<Token>> Scanner::scan_tokens() {
    while (!is_end_of_src()) {
        lexeme_start_pos = current_pos;
        scan_token();
    }

    return tokens;
}

bool Scanner::is_end_of_src() {
    return this->current_pos >= this->src.length();
}

void Scanner::scan_token() {
    char c = src.at(current_pos);
    current_pos++;

    switch (c) {
    case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
    case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
    case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
    case '}':
        add_token(TokenType::RIGHT_BRACE);
        break;
    case ',':
        add_token(TokenType::COMMA);
        break;
    case '.':
        add_token(TokenType::DOT);
        break;
    case '-':
        add_token(TokenType::MINUS);
        break;
    case '+':
        add_token(TokenType::PLUS);
        break;
    case ';':
        add_token(TokenType::SEMICOLON);
        break;
    case '*':
        add_token(TokenType::STAR);
        break;
    case '%':
        add_token(TokenType::MOD);
        break;
    case '!':
        add_token(next_char_is('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        add_token(next_char_is('=') ? TokenType::EQUAL_EQUAL
                                    : TokenType::EQUAL);
        break;
    case '<':
        add_token(next_char_is('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        add_token(next_char_is('=') ? TokenType::GREATER_EQUAL
                                    : TokenType::GREATER);
        break;
    case '/':
        if (next_char_is('/')) {
            while (!is_end_of_src() && src.at(current_pos) != '\n') {
                current_pos++;
            }
        } else {
            add_token(TokenType::SLASH);
        }
        break;
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;
    case '"':
        parse_str();
        break;
    default:
        if (std::isdigit(c)) {
            parse_num();
            break;
        } else if (std::isalpha(c) || c == '_') {
            parse_identifier();
            break;
        } else {
            ErrorManager::handle_err(line, "Unexpected character.");
            break;
        }
    }
}

void Scanner::add_token(TokenType type, ExprVal literal) {
    std::string lexeme =
        src.substr(lexeme_start_pos, current_pos - lexeme_start_pos);
    tokens.push_back(std::make_shared<Token>(type, lexeme, literal, line));
}

bool Scanner::next_char_is(char expected) {
    if (is_end_of_src()) {
        return false;
    }

    if (src.at(current_pos) == expected) {
        current_pos++;
        return true;
    }

    return false;
}

void Scanner::parse_str() {
    uint str_start_pos = current_pos;

    while (!is_end_of_src() and src.at(current_pos) != '"') {
        if (src.at(current_pos) == '\n') {
            line++;
        }
        current_pos++;
    }

    if (is_end_of_src()) {
        ErrorManager::handle_err(line, "Unterminated string");
        return;
    }

    std::string str = src.substr(str_start_pos, current_pos - str_start_pos);
    add_token(TokenType::STRING, str);
    current_pos++;
}

void Scanner::parse_num() {
    uint str_start_pos = current_pos - 1;

    while (!is_end_of_src() and std::isdigit(src.at(current_pos))) {
        current_pos++;
    }

    if (!is_end_of_src() && src.at(current_pos) == '.') {
        current_pos++;
        while (!is_end_of_src() and std::isdigit(src.at(current_pos))) {
            current_pos++;
        }
    }

    std::string num_str =
        src.substr(str_start_pos, current_pos - str_start_pos);
    double num = std::stod(num_str);
    add_token(TokenType::NUMBER, num);
}

void Scanner::parse_identifier() {
    uint str_start_pos = current_pos - 1;

    while (!is_end_of_src() &&
           (std::isalnum(src.at(current_pos)) || src.at(current_pos) == '_')) {
        current_pos++;
    }

    std::string identifier_str =
        src.substr(str_start_pos, current_pos - str_start_pos);
    if (reserved_kws.find(identifier_str) == reserved_kws.end()) {
        add_token(TokenType::IDENTIFIER, identifier_str);
    } else {
        add_token(reserved_kws.at(identifier_str));
    }
}