#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/common/error_manager.hpp"
#include "clox/common/token.hpp"
#include "clox/middleware/identifier_resolver.hpp"
#include "clox/parser/parser.hpp"
#include "clox/scanner/scanner.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

class CLox {
  private:
    // Process one line or a whole file
    static void run(std::string source) {
        std::unique_ptr<Scanner> scanner(new Scanner(source));
        std::vector<std::shared_ptr<Token>> tokens = scanner->scan_tokens();

        Parser parser = Parser(tokens);
        std::vector<std::shared_ptr<Stmt>> stmts = parser.parse_program();
        if (ErrorManager::had_static_err) {
            std::cout << "Parser error occurs" << std::endl;
            return;
        }

        IdentifierResolver resolver = IdentifierResolver(ast_interpreter);
        resolver.resolve_program(stmts);
        if (ErrorManager::had_static_err) {
            std::cout << "Resolver error occurs" << std::endl;
            return;
        }

        ast_interpreter->interpret_program(stmts);
        if (ErrorManager::had_runtime_err) {
            std::cout << "Runtime error occurs" << std::endl;
            return;
        }
    }

  public:
    static std::shared_ptr<AstInterpreter> ast_interpreter;
    static void run_file(std::string path) {
        ErrorManager::had_static_err = false;
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
            ErrorManager::had_static_err = false;
            ErrorManager::had_runtime_err = false;
            run(line);
            std::cout << prompt_start;
        }
    }
};

std::shared_ptr<AstInterpreter> CLox::ast_interpreter;

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cout << "Usage: lox [script]" << std::endl;
        exit(1);
    } else if (argc == 2) {
        CLox::ast_interpreter = std::make_shared<AstInterpreter>(false);
        CLox::run_file(argv[1]);
        if (ErrorManager::had_static_err) {
            exit(65);
        }
        if (ErrorManager::had_runtime_err) {
            exit(70);
        }
    } else {
        CLox::ast_interpreter = std::make_shared<AstInterpreter>(true);
        CLox::run_prompt();
    }
}