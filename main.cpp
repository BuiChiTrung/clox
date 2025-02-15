#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/ast_interpreter/printer_visitor.hpp"
#include "clox/error_manager.hpp"
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
    static std::unique_ptr<InterpreterVisitor> interpreter_visitor;
    static std::unique_ptr<PrinterVisitor> printer_visitor;
    // Process one line or a whole file
    static void run(std::string source) {
        std::unique_ptr<Scanner> scanner(new Scanner(source));
        std::vector<std::shared_ptr<Token>> tokens = scanner->scan_tokens();

        auto parser = Parser(tokens);
        auto stmts = parser.parse_program();
        if (ErrorManager::had_err) {
            std::cout << "Parser error occurs" << std::endl;
            return;
        }

        interpreter_visitor->interpret_program(stmts);
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
        ErrorManager::had_err = false;
        std::string line;

        std::cout << "Enter lines of text (Ctrl+D or Ctrl+Z to end):"
                  << std::endl;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            std::cout << "> ";
            run(line);
        }
    }
};

std::unique_ptr<PrinterVisitor> CLox::printer_visitor =
    std::make_unique<PrinterVisitor>();
std::unique_ptr<InterpreterVisitor> CLox::interpreter_visitor =
    std::make_unique<InterpreterVisitor>();

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