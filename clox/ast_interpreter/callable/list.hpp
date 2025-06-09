#pragma once
#include "clox/ast_interpreter/callable/class.hpp"
#include "clox/ast_interpreter/environment.hpp"
#include "clox/ast_interpreter/helper.hpp"
#include "clox/common/expr_val.hpp"
#include "clox/utils/helper.hpp"
#include <memory>

class ListInstance : public LoxInstance {
  private:
    std::vector<ExprVal> elements = {};

  public:
    using LoxInstance::LoxInstance;

    std::string to_string() const override {
        std::string res = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            res += cast_expr_val_to_string(elements[i]);
            if (i < elements.size() - 1) {
                res += ", ";
            }
        }
        return res + "]";
    }

    friend class List;
    friend class ListPush;
    friend class ListPop;
    friend class ListAt;
    friend class ListSize;
};

class ListMethod : public LoxMethod {
  public:
    using LoxMethod::LoxMethod;

    std::shared_ptr<ListInstance> get_list_instance() {
        ExprVal _this = this->enclosing_env->get_identifier("this", nullptr);
        auto lox_instance = std::get<std::shared_ptr<LoxInstance>>(_this);
        if (!lox_instance) {
            throw RuntimeException(nullptr,
                                   "Can only call List methods on a List "
                                   "instance.");
        }

        auto list_instance =
            std::dynamic_pointer_cast<ListInstance>(lox_instance);
        if (!list_instance) {
            throw RuntimeException(nullptr,
                                   "Can only call List methods on a List "
                                   "instance.");
        }
        return list_instance;
    }
};

class ListPush : public ListMethod {
  public:
    using ListMethod::ListMethod;

    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::shared_ptr<ListInstance> list_instance = get_list_instance();
        list_instance->elements.push_back(args[0]);
        return NIL;
    }

    uint get_param_num() override { return 1; }
};

class ListPop : public ListMethod {
  public:
    using ListMethod::ListMethod;

    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::shared_ptr<ListInstance> list_instance = get_list_instance();

        if (list_instance->elements.empty()) {
            throw RuntimeException(nullptr, "Cannot pop from an empty list.");
        }
        list_instance->elements.pop_back();
        return NIL;
    }

    uint get_param_num() override { return 0; }
};

class ListAt : public ListMethod {
  public:
    using ListMethod::ListMethod;

    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::shared_ptr<ListInstance> list_instance = get_list_instance();

        double index = std::get<double>(args[0]);
        if (!is_double_int(index)) {
            throw RuntimeException(nullptr,
                                   "Index must be an integer for List access.");
        }
        if (index < 0 || index >= list_instance->elements.size()) {
            throw RuntimeException(nullptr,
                                   "Index out of bounds for List access.");
        }

        return list_instance->elements[index];
    }

    uint get_param_num() override { return 1; }
};

class ListSize : public ListMethod {
  public:
    using ListMethod::ListMethod;

    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        std::shared_ptr<ListInstance> list_instance = get_list_instance();

        return double(list_instance->elements.size());
    }

    uint get_param_num() override { return 0; }
};

class List : public LoxClass {
  public:
    List(std::shared_ptr<Environment> class_env) {
        name = "List";
        methods["push"] = std::make_shared<ListPush>(nullptr, class_env);
        methods["pop"] = std::make_shared<ListPop>(nullptr, class_env);
        methods["at"] = std::make_shared<ListAt>(nullptr, class_env);
        methods["size"] = std::make_shared<ListSize>(nullptr, class_env);
    }

    // Class constructor
    ExprVal invoke(AstInterpreter &interpreter,
                   std::vector<ExprVal> &args) override {
        auto lox_class_sp =
            std::shared_ptr<LoxClass>(this, smart_pointer_no_op_deleter);
        auto list_instance = std::make_shared<ListInstance>(lox_class_sp);

        for (auto &arg : args) {
            list_instance->elements.push_back(arg);
        }

        return list_instance;
    }

    uint get_param_num() override { return UNLIMITED_ARGS_NUM; }
    std::string to_string() const override { return "<Class List>"; }
};
