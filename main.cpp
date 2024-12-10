#include "lox.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << argc << " " << argv << std::endl;
  if (argc > 2) {
    std::cout << "Usage: lox [script]" << std::endl;
    exit(1);
  } else if (argc == 2) {
    CLox::run_file(argv[1]);
    if (CLox::had_error) {
      exit(1);
    }
  } else {
    CLox::run_prompt();
  }
}