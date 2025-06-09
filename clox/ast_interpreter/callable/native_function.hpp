
#include "clox/ast_interpreter/callable/callable.hpp"
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
            std::cout << cast_expr_val_to_string(args[i]);
            if (i < args.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        return NIL;
    }

    std::string to_string() const override { return "<native-fn print>"; }

    uint get_param_num() override { return UNLIMITED_ARGS_NUM; }
};

class ReadNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::string input;
        std::cin >> input;
        return input;
    }

    std::string to_string() const override { return "<native-fn read>"; }
};

class ReadlineNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    std::string to_string() const override { return "<native-fn readline>"; }
};

class BoolCastNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        return cast_expr_val_to_bool(args[0]);
    }

    std::string to_string() const override { return "<native-fn bool>"; }

    uint get_param_num() override { return 1; }
};

class StringCastNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        return cast_expr_val_to_string(args[0]);
    }

    std::string to_string() const override { return "<native-fn str>"; }

    uint get_param_num() override { return 1; }
};

class DoubleCastNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        return cast_expr_val_to_double(args[0]);
    }

    std::string to_string() const override { return "<native-fn num>"; }

    uint get_param_num() override { return 1; }
};

class IntCastNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        return cast_expr_val_to_int(args[0]);
    }

    std::string to_string() const override { return "<native-fn int>"; }

    uint get_param_num() override { return 1; }
};