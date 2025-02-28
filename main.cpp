#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/parser.hpp"
#include "clox/scanner/scanner.hpp"
#include "clox/scanner/token.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

class CLox {
  private:
    static std::unique_ptr<AstInterpreter> ast_interpreter;
    // Process one line or a whole file
    static void run(std::string source) {
        std::unique_ptr<Scanner> scanner(new Scanner(source));
        std::vector<std::shared_ptr<Token>> tokens = scanner->scan_tokens();

        Parser parser = Parser(tokens);
        std::vector<std::shared_ptr<Stmt>> stmts = parser.parse_program();
        // std::cout << stmts.size() << "stmt size ";
        if (ErrorManager::had_err) {
            std::cout << "Parser error occurs" << std::endl;
            return;
        }

        ast_interpreter->interpret_program(stmts);
        if (ErrorManager::had_runtime_err) {
            std::cout << "Runtime error occurs" << std::endl;
            return;
        }
    }

  public:
    static void run_file(std::string path) {
        ErrorManager::had_err = false;
        std::ifstream file(path);
        std::string content;

        if (file.is_open()) {
            // Read entire file into a string
            content.assign((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

            run(content);
            file.close();
        } else {
            std::cerr << "Unable to open file" << std::endl;
        }
    }

    // Interactive mode
    static void run_prompt() {
        const std::string prompt_start = "==> ";
        std::string line;

        std::cout << "Enter lines of text (Ctrl+D or Ctrl+Z to end):"
                  << std::endl;
        std::cout << prompt_start;

        while (std::getline(std::cin, line)) {
            ErrorManager::had_err = false;
            ErrorManager::had_runtime_err = false;
            run(line);
            std::cout << prompt_start;
        }
    }
};

std::unique_ptr<AstInterpreter> CLox::ast_interpreter =
    std::make_unique<AstInterpreter>();

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cout << "Usage: lox [script]" << std::endl;
        exit(1);
    } else if (argc == 2) {
        CLox::run_file(argv[1]);
        if (ErrorManager::had_err) {
            exit(65);
        }
        if (ErrorManager::had_runtime_err) {
            exit(70);
        }
    } else {
        CLox::run_prompt();
    }
}