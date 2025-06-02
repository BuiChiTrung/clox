#pragma once
#include "clox/ast_interpreter/callable.hpp"
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
    std::unordered_map<std::string, std::shared_ptr<LoxMethod>> methods;
    friend class LoxInstance;
    friend class AstInterpreter;

  public:
    LoxClass(
        std::string name,
        std::unordered_map<std::string, std::shared_ptr<LoxMethod>> &methods)
        : name(name), methods(methods) {}

    uint get_param_num() override {
        auto constructor = get_method(name);
        if (constructor == nullptr) {
            return 0;
        }
        return constructor->get_param_num();
    }

    // Class constructor, a method equal to class_name
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        // Allocate memory for new instance
        auto lox_class_sp =
            std::shared_ptr<LoxClass>(this, smart_pointer_no_op_deleter);
        auto lox_instance = std::make_shared<LoxInstance>(lox_class_sp);

        // Run the user-defined constructor if it exists
        auto constructor = get_method(name);
        if (constructor != nullptr) {
            constructor->bind_this_kw_to_class_method(*lox_instance);
            constructor->invoke(interpreter, args);
        }
        return lox_instance;
    }

    std::shared_ptr<LoxMethod> get_method(std::string name) {
        if (methods.count(name) == 0) {
            return nullptr;
        }
        return methods[name];
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
        if (field_name == lox_class->name) {
            throw RuntimeException(
                field_token,
                "Constructor cannot be called by a class instance.");
        }

        if (lox_class->methods.count(field_name)) {
            auto method = lox_class->get_method(field_name);
            method->bind_this_kw_to_class_method(*this);
            return method;
        }

        throw RuntimeException(field_token, "Instance field" + field_name +
                                                " does not exists.");
    }

    std::string to_string() const {
        return "<Instance " + lox_class->name + ">";
    }
};
