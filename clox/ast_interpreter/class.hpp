#pragma once
#include "clox/ast_interpreter/callable.hpp"
#include "clox/common/constants.hpp"
#include "clox/common/error_manager.hpp"
#include "clox/utils/helper.hpp"

#include <memory>
#include <sys/types.h>

class LoxMethod : public LoxFunction {
  public:
    using LoxFunction::LoxFunction;
    std::string to_string() const override {
        return "<Method " + func_stmt->name->lexeme + ">";
    }

    void bind_this_kw_to_class_method(LoxInstance &instance) {
        // Use no-op deleter to make sure that "this" is not deallocate
        // after shared_ptr run out of scope.
        this->parent_env->identifier_table["this"] =
            std::shared_ptr<LoxInstance>(&instance,
                                         smart_pointer_no_op_deleter);
    }
};

class LoxClass : public LoxCallable {
  private:
    std::string name;
    std::shared_ptr<LoxClass> superclass;
    std::unordered_map<std::string, std::shared_ptr<LoxMethod>> methods;

    friend class LoxInstance;
    friend class AstInterpreter;

  public:
    LoxClass(
        std::string name, std::shared_ptr<LoxClass> superclass,
        std::unordered_map<std::string, std::shared_ptr<LoxMethod>> &methods)
        : name(name), superclass(superclass), methods(methods) {}

    uint get_param_num() override {
        auto initializer = get_method(INIT_METHOD);
        if (initializer == nullptr) {
            return 0;
        }
        return initializer->get_param_num();
    }

    // Class constructor
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        // Allocate memory for new instance
        auto lox_class_sp =
            std::shared_ptr<LoxClass>(this, smart_pointer_no_op_deleter);
        auto lox_instance = std::make_shared<LoxInstance>(lox_class_sp);

        // Run the user-defined initializer if it exists
        auto initializer = get_method(INIT_METHOD);
        if (initializer != nullptr) {
            initializer->bind_this_kw_to_class_method(*lox_instance);
            initializer->invoke(interpreter, args);
        }
        return lox_instance;
    }

    std::shared_ptr<LoxMethod> get_method(std::string name) {
        if (methods.count(name) > 0) {
            return methods[name];
        }

        if (superclass != nullptr) {
            return superclass->get_method(name);
        }

        return nullptr;
    }

    std::string to_string() const override { return "<Class " + name + ">"; }
};

class LoxInstance {
  private:
    std::shared_ptr<LoxClass> lox_class;
    std::unordered_map<std::string, ExprVal> props;
    friend class AstInterpreter;

  public:
    LoxInstance(std::shared_ptr<LoxClass> lox_class) : lox_class(lox_class) {}

    ExprVal get_field(std::shared_ptr<Token> field_token) {
        std::string field_name = field_token->lexeme;
        // Find prop
        if (props.count(field_name) > 0) {
            return props[field_name];
        }

        // Find method
        std::shared_ptr<LoxMethod> method = lox_class->get_method(field_name);
        if (method != nullptr) {
            method->bind_this_kw_to_class_method(*this);
            return method;
        }

        throw RuntimeException(field_token, "Instance field " + field_name +
                                                " does not exists.");
    }

    std::string to_string() const {
        return "<Instance " + lox_class->name + ">";
    }
};
