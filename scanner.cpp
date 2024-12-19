#include "scanner.hpp"
#include "error_manager.hpp"
#include <cctype>
#include <string>

Scanner::Scanner(std::string src) { this->src = src; }

std::vector<Token> Scanner::scan_tokens() {
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
        add_token(LEFT_PAREN);
        break;
    case ')':
        add_token(RIGHT_PAREN);
        break;
    case '{':
        add_token(LEFT_BRACE);
        break;
    case '}':
        add_token(RIGHT_BRACE);
        break;
    case ',':
        add_token(COMMA);
        break;
    case '.':
        add_token(DOT);
        break;
    case '-':
        add_token(MINUS);
        break;
    case '+':
        add_token(PLUS);
        break;
    case ';':
        add_token(SEMICOLON);
        break;
    case '*':
        add_token(STAR);
        break;
    case '!':
        add_token(next_char_is('=') ? BANG_EQUAL : BANG);
        break;
    case '=':
        add_token(next_char_is('=') ? EQUAL_EQUAL : EQUAL);
        break;
    case '<':
        add_token(next_char_is('=') ? LESS_EQUAL : LESS);
        break;
    case '>':
        add_token(next_char_is('=') ? GREATER_EQUAL : GREATER);
        break;
    case '/':
        if (next_char_is('/')) {
            while (!is_end_of_src() && src.at(current_pos) != '\n') {
                current_pos++;
            }
        }
        else {
            add_token(SLASH);
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
        }
        else if (std::isalpha(c) || c == '_') {
            parse_identifier();
            break;
        }
        else {
            ErrorManager::err(line, "Unexpected character.");
            break;
        }
    }
}

void Scanner::add_token(TokenType type, std::any literal) {
    std::string lexeme =
        src.substr(lexeme_start_pos, current_pos - lexeme_start_pos);
    tokens.push_back(Token(type, lexeme, literal, line));
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
    uint str_start_pos = current_pos + 1;
    while (!is_end_of_src() and src.at(current_pos) != '"') {
        if (src.at(current_pos) == '\n') {
            line++;
        }
        current_pos++;
    }

    if (is_end_of_src()) {
        ErrorManager::err(line, "Unterminated string");
        return;
    }

    std::string str = src.substr(str_start_pos, current_pos - str_start_pos);
    add_token(STRING, str);
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
    add_token(NUMBER, num);
}

void Scanner::parse_identifier() {
    uint str_start_pos = current_pos;

    while (!is_end_of_src() &&
           (std::isalnum(src.at(current_pos)) || src.at(current_pos) == '_')) {
        current_pos++;
    }

    std::string identifier_str =
        src.substr(str_start_pos, current_pos - str_start_pos);
    if (reserved_kws.find(identifier_str) == reserved_kws.end()) {
        add_token(IDENTIFIER, identifier_str);
    }
    else {
        add_token(reserved_kws.at(identifier_str));
    }
}