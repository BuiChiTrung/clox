#include "clox/ast/interpreter_visitor.hpp"
#include "clox/ast/return.hpp"
#include "clox/ast/stmt.hpp"
#include "clox/environment.hpp"
#include "clox/token.hpp"

#include <chrono>
#include <memory>
#include <sys/types.h>

// TODO(trung.bc): split to another file
class LoxCallable {
  public:
    virtual uint get_param_num() const { return 0; }

    virtual ExprVal invoke(InterpreterVisitor *interpreter,
                           std::vector<ExprVal> &args) = 0;

    virtual std::string to_string() const = 0;
};

class ClockNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(InterpreterVisitor *interpreter,
                   std::vector<ExprVal> &args) override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> unix_time = now.time_since_epoch();

        return unix_time.count();
    }

    std::string to_string() const override { return "<native-fn clock>"; }
};

class LoxFunction : public LoxCallable {
    const FunctionStmt &func_stmt;
    // Can be global env or the env of the outer func defined this func
    std::shared_ptr<Environment> parent_env;

  public:
    LoxFunction(const FunctionStmt &func_stmt,
                std::shared_ptr<Environment> parent_env)
        : func_stmt(func_stmt), parent_env(parent_env) {}

    uint get_param_num() const override { return func_stmt.params.size(); }

    ExprVal invoke(InterpreterVisitor *interpreter,
                   std::vector<ExprVal> &args) override {
        // Each time a func is invoked an env should be created to save var
        // defined in the func scope
        std::shared_ptr<Environment> func_env(new Environment(parent_env));
        for (int i = 0; i < func_stmt.params.size(); ++i) {
            func_env->add_new_variable(func_stmt.params[i]->name->lexeme,
                                       args[i]);
        }

        auto block = std::dynamic_pointer_cast<BlockStmt>(func_stmt.body);
        try {
            interpreter->visit_block_stmt(*block, func_env);
        } catch (Return r) {
            return r.return_val;
        }

        return NIL;
    }

    std::string to_string() const override {
        return "<fn " + func_stmt.name->lexeme + ">";
    }
};