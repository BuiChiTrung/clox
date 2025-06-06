
#include "clox/ast_interpreter/callable.hpp"
#include "clox/ast_interpreter/helper.hpp"
#include "clox/common/constants.hpp"
#include <iostream>

class ClockNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> unix_time = now.time_since_epoch();

        return unix_time.count();
    }

    std::string to_string() const override { return "<native-fn clock>"; }
};

class PrintNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        for (int i = 0; i < args.size(); ++i) {
            std::cout << exprval_to_string(args[i]);
            if (i < args.size() - 1) {
                std::cout << " "; // Print space between args
            }
        }
        std::cout << std::endl;
        return NIL;
    }

    std::string to_string() const override { return "<native-fn print>"; }

    uint get_param_num() override { return UNLIMITED_ARGS_NUM; }
};