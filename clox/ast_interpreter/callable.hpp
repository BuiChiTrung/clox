#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/ast_interpreter/environment.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

class LoxCallable {
  public:
    virtual uint get_param_num() { return 0; }

    virtual ExprVal invoke(AstInterpreter *interpreter,
                           std::vector<ExprVal> &args) = 0;

    virtual std::string to_string() const = 0;
};

class ClockNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(AstInterpreter *interpreter,
                   std::vector<ExprVal> &args) override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> unix_time = now.time_since_epoch();

        return unix_time.count();
    }

    std::string to_string() const override { return "<native-fn clock>"; }
};

class LoxFunction : public LoxCallable {
    const FunctionDecl &func_stmt;
    // Can be global env or the env of the outer func defined this func
    std::shared_ptr<Environment> parent_env;
    friend class LoxInstance;

  public:
    LoxFunction(const FunctionDecl &func_stmt,
                std::shared_ptr<Environment> parent_env)
        : func_stmt(func_stmt), parent_env(parent_env) {}

    uint get_param_num() override { return func_stmt.params.size(); }

    ExprVal invoke(AstInterpreter *interpreter,
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
        } catch (ReturnVal r) {
            return r.return_val;
        }

        return NIL;
    }

    std::string to_string() const override {
        return "<fn " + func_stmt.name->lexeme + ">";
    }

    void bind_this_kw_to_class_method(LoxInstance &instance) {
        // Use no-op deleter to make sure that "this" is not deallocate
        // after shared_ptr run out of scope.
        this->parent_env->identifier_table["this"] =
            std::shared_ptr<LoxInstance>(&instance, [](LoxInstance *) {});
    }
};

class LoxClass : public LoxCallable {
  private:
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
    friend class LoxInstance;
    friend class AstInterpreter;

  public:
    LoxClass(
        std::string name,
        std::unordered_map<std::string, std::shared_ptr<LoxFunction>> &methods)
        : name(name), methods(methods) {}

    uint get_param_num() override {
        auto constructor = get_method(name);
        if (constructor == nullptr) {
            return 0;
        }
        return constructor->get_param_num();
    }

    // Class constructor, a method equal to class_name
    ExprVal invoke(AstInterpreter *interpreter,
                   std::vector<ExprVal> &args) override {
        auto lox_instance = std::make_shared<LoxInstance>(*this);
        auto constructor = get_method(name);
        if (constructor != nullptr) {
            constructor->bind_this_kw_to_class_method(*lox_instance);
            constructor->invoke(interpreter, args);
        }
        return lox_instance;
    }

    std::shared_ptr<LoxFunction> get_method(std::string name) {
        if (methods.count(name) == 0) {
            return nullptr;
        }
        return methods[name];
    }

    std::string to_string() const override { return "<Class " + name + ">"; }
};

class LoxInstance {
  private:
    LoxClass &lox_class;
    std::unordered_map<std::string, ExprVal> props;
    friend class AstInterpreter;

  public:
    LoxInstance(LoxClass &lox_class) : lox_class(lox_class) {}

    std::string to_string() const {
        return "<Instance " + lox_class.name + ">";
    }

    ExprVal get_field(std::shared_ptr<Token> field_token) {
        std::string field_name = field_token->lexeme;
        if (props.count(field_name) > 0) {
            return props[field_name];
        }

        if (field_name == lox_class.name) {
            throw RuntimeException(
                field_token,
                "Constructor cannot be called by a class instance.");
        }

        if (lox_class.methods.count(field_name)) {
            auto method = lox_class.get_method(field_name);
            method->bind_this_kw_to_class_method(*this);
            return method;
        }

        throw RuntimeException(field_token, "Instance field" + field_name +
                                                " does not exists.");
    }
};
