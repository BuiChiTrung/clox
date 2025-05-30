#include "ast_interpreter.hpp"
#include "clox/ast_interpreter/callable.hpp"
#include "clox/ast_interpreter/class.hpp"
#include "clox/ast_interpreter/environment.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include "clox/utils/helper.hpp"

#include <iostream>
#include <memory>
#include <variant>

AstInterpreter::AstInterpreter(const bool is_interactive_mode)
    : global_env(std::make_shared<Environment>()),
      is_interactive_mode(is_interactive_mode) {
    env = global_env;
    global_env->add_new_variable("clock", std::make_shared<ClockNativeFunc>());
}

// func to test if the interpreter can exec a single expression
ExprVal AstInterpreter::interpret_single_expr(Expr &expression) {
    try {
        ExprVal result = evaluate_expr(expression);
        return result;
    } catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
        return NIL;
    }
}

void AstInterpreter::interpret_program(
    const std::vector<std::shared_ptr<Stmt>> &stmts) {
    try {
        for (const auto &stmt : stmts) {
            // exec stmt
            stmt->accept(*this);
        }
    } catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
    }
}

ExprVal AstInterpreter::evaluate_expr(Expr &expr) { return expr.accept(*this); }

inline std::string exprval_to_string(const ExprVal &value) {
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, double>) {
                return double_to_string(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T,
                                                std::shared_ptr<LoxCallable>> ||
                                 std::is_same_v<T,
                                                std::shared_ptr<LoxInstance>>) {
                return arg ? arg->to_string() : "nil";
            } else {
                return "nil";
            }
        },
        value);
}

void AstInterpreter::visit_expr_stmt(const ExprStmt &e) {
    ExprVal val = evaluate_expr(*e.expr);
    if (is_interactive_mode) {
        std::cout << exprval_to_string(val) << std::endl;
    }
}

void AstInterpreter::visit_assign_stmt(const AssignStmt &a) {
    ExprVal new_value = evaluate_expr(*a.value);
    // env->assign_new_value_to_variable(a.var->name, new_value);

    const IdentifierExpr *ptr = a.var.get();
    int depth = identifier_scope_depth[ptr];
    move_up_env(depth)->identifier_table[a.var->name->lexeme] = new_value;
}

void AstInterpreter::visit_print_stmt(const PrintStmt &p) {
    ExprVal val = evaluate_expr(*p.expr);
    std::cout << exprval_to_string(val) << std::endl;
}

void AstInterpreter::visit_var_decl(const VarDecl &v) {
    ExprVal var_value = NIL;
    if (v.initializer != nullptr) {
        var_value = evaluate_expr(*v.initializer);
    }
    env->add_new_variable(v.var_name->lexeme, var_value);
}

void AstInterpreter::visit_if_stmt(const IfStmt &i) {
    bool exec_else_block = true;

    for (int j = 0; j < i.conditions.size(); ++j) {
        ExprVal expr_val = evaluate_expr(*i.conditions[j]);
        if (cast_literal_to_bool(expr_val)) {
            i.if_blocks[j]->accept(*this);
            exec_else_block = false;
            break;
        }
    }

    if (exec_else_block && i.else_block != nullptr) {
        i.else_block->accept(*this);
    }
}

void AstInterpreter::visit_while_stmt(const WhileStmt &w) {
    while (cast_literal_to_bool(evaluate_expr(*w.condition))) {
        w.body->accept(*this);
    }
}

void AstInterpreter::visit_function_decl(const FunctionDecl &func_decl) {
    auto func = std::make_shared<LoxFunction>(func_decl, env);
    env->add_new_variable(func_decl.name->lexeme, func);
}

void AstInterpreter::visit_class_decl(const ClassDecl &class_decl) {
    std::unordered_map<std::string, std::shared_ptr<LoxMethod>> methods = {};

    auto class_env = std::make_shared<Environment>(env);
    class_env->add_new_variable("this", NIL);

    for (auto method : class_decl.methods) {
        auto lox_method = std::make_shared<LoxMethod>(*method.get(), env);
        methods[method->name->lexeme] = lox_method;
    }
    auto lox_class =
        std::make_shared<LoxClass>(class_decl.name->lexeme, methods);
    env->add_new_variable(class_decl.name->lexeme, lox_class);
}

void AstInterpreter::visit_block_stmt(const BlockStmt &b,
                                      std::shared_ptr<Environment> block_env) {
    auto cur_env = this->env;
    if (block_env == nullptr) {
        block_env = std::make_shared<Environment>(cur_env);
    }

    this->env = block_env;
    try {
        for (auto stmt : b.stmts) {
            stmt->accept(*this);
        }
    } catch (ReturnVal &r) {
        this->env = cur_env;
        throw r;
    }
    this->env = cur_env;
}

void AstInterpreter::visit_return_stmt(const ReturnStmt &r) {
    ExprVal return_val = NIL;
    if (r.expr != nullptr) {
        return_val = evaluate_expr(*r.expr);
    }

    throw ReturnVal(return_val);
}

void AstInterpreter::visit_set_class_field(
    const SetClassFieldStmt &set_class_field_stmt) {
    std::shared_ptr<LoxInstance> lox_instance = nullptr;
    try {
        lox_instance = std::get<std::shared_ptr<LoxInstance>>(
            set_class_field_stmt.lox_instance->accept(*this));
    } catch (std::bad_variant_access) {
        throw RuntimeException(set_class_field_stmt.field_token,
                               "Can only call property of a lox instance.");
    }

    ExprVal value = evaluate_expr(*set_class_field_stmt.value);
    lox_instance->props[set_class_field_stmt.field_token->lexeme] = value;
}

ExprVal
AstInterpreter::visit_identifier(const IdentifierExpr &identifier_expr) {
    return evaluate_identifier(identifier_expr);
}

ExprVal AstInterpreter::visit_this(const ThisExpr &this_expr) {
    return evaluate_identifier(this_expr);
}

ExprVal
AstInterpreter::evaluate_identifier(const IdentifierExpr &identifier_expr_ptr) {
    const IdentifierExpr *ptr = &identifier_expr_ptr;
    if (identifier_scope_depth.count(ptr) == 0) {
        throw RuntimeException(ptr->name,
                               "Reference to non-exist identifier: " +
                                   ptr->name->lexeme);
    }

    int depth = identifier_scope_depth[ptr];

    return move_up_env(depth)->identifier_table[ptr->name->lexeme];
}

ExprVal AstInterpreter::visit_literal(const LiteralExpr &l) { return l.value; }

ExprVal AstInterpreter::visit_grouping(const GroupExpr &g) {
    return evaluate_expr(*g.expr);
}

ExprVal AstInterpreter::visit_func_call(const FuncCallExpr &f) {
    ExprVal callee = evaluate_expr(*f.callee);

    if (!std::holds_alternative<std::shared_ptr<LoxCallable>>(callee)) {
        throw RuntimeException(f.close_parenthesis,
                               "Can only call functions and class's method.");
    }

    auto func = std::get<std::shared_ptr<LoxCallable>>(callee);
    // Check func number of params = number of args passed to it.
    if (func->get_param_num() != f.args.size()) {
        throw RuntimeException(
            f.close_parenthesis,
            "Expected " + std::to_string(func->get_param_num()) +
                " args to be passed to the function, but got " +
                std::to_string(f.args.size()));
    }

    std::vector<ExprVal> arg_vals{};
    for (auto arg : f.args) {
        arg_vals.push_back(evaluate_expr(*arg));
    }

    return func->invoke(*this, arg_vals);
}

ExprVal AstInterpreter::visit_get_class_field(const GetClassFieldExpr &expr) {
    std::shared_ptr<LoxInstance> lox_instance = nullptr;
    try {
        lox_instance = std::get<std::shared_ptr<LoxInstance>>(
            expr.lox_instance->accept(*this));
    } catch (std::bad_variant_access) {
        throw RuntimeException(expr.field_token,
                               "Can only call property of a lox instance.");
    }

    return lox_instance->get_field(expr.field_token);
}

ExprVal AstInterpreter::visit_unary(const UnaryExpr &u) {
    ExprVal right = evaluate_expr(*u.operand);

    switch (u.operation->type) {
    case TokenType::BANG:
        return !cast_literal_to_bool(right);
    case TokenType::MINUS:
        check_number_operand(u.operation, right);
        return -std::get<double>(right);
    default:
        ErrorManager::handle_err(u.operation->line, "Invalid unary expression");
        return NIL;
    }
}

ExprVal AstInterpreter::visit_binary(const BinaryExpr &b) {
    ExprVal left = evaluate_expr(*b.left_operand);
    ExprVal right = evaluate_expr(*b.right_operand);

    auto left_double_ptr = std::get_if<double>(&left);
    auto right_double_ptr = std::get_if<double>(&right);
    auto left_string_ptr = std::get_if<std::string>(&left);
    auto right_string_ptr = std::get_if<std::string>(&right);

    switch (b.operation->type) {
    // Special case: + op can be used to concate 2 strings
    case TokenType::PLUS:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr + *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr + *right_string_ptr;
        }
        if (left_double_ptr && right_string_ptr) {
            return double_to_string(*left_double_ptr) + *right_string_ptr;
        }
        if (left_string_ptr && right_double_ptr) {
            return *left_string_ptr + double_to_string(*right_double_ptr);
        }
        throw RuntimeException(b.operation,
                               "Operands must be number or string");
    case TokenType::MINUS:
        check_number_operands(b.operation, left, right);
        return *left_double_ptr - *right_double_ptr;
    case TokenType::STAR:
        check_number_operands(b.operation, left, right);
        return *left_double_ptr * *right_double_ptr;
    case TokenType::MOD:
        check_int_operands(b.operation, left, right);
        return double(int(*left_double_ptr) % int(*right_double_ptr));
    case TokenType::SLASH:
        check_number_operands(b.operation, left, right);
        if (*right_double_ptr == 0) {
            throw RuntimeException(b.operation, "Devide by 0");
        }
        return *left_double_ptr / *right_double_ptr;
    case TokenType::GREATER:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr > *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr > *right_string_ptr;
        }
        throw RuntimeException(b.operation,
                               "Expected compare 2 numbers or 2 strings");
    case TokenType::LESS:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr < *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr < *right_string_ptr;
        }
        throw RuntimeException(b.operation,
                               "Expected compare 2 numbers or 2 strings");
    case TokenType::GREATER_EQUAL:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr >= *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr >= *right_string_ptr;
        }
        throw RuntimeException(b.operation,
                               "Expected compare 2 numbers or 2 strings");
    case TokenType::LESS_EQUAL:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr <= *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr <= *right_string_ptr;
        }
        throw RuntimeException(b.operation,
                               "Expected compare 2 numbers or 2 strings");
    // Special case: support compare mixed type another type with bool
    case TokenType::BANG_EQUAL:
        return !is_equal(left, right);
    case TokenType::EQUAL_EQUAL:
        return is_equal(left, right);
    // Special case: we may know the result after evaluating the left expr
    case TokenType::AND:
        if (!cast_literal_to_bool(left)) {
            return false;
        }
        return cast_literal_to_bool(right);
    case TokenType::OR:
        if (cast_literal_to_bool(left)) {
            return true;
        }
        return cast_literal_to_bool(right);
    default:
        ErrorManager::handle_err(b.operation->line,
                                 "Invalid binary expression");
        return NIL;
    }
}

bool AstInterpreter::cast_literal_to_bool(const ExprVal &val) {
    if (const auto boolPtr(std::get_if<bool>(&val)); boolPtr) {
        return *boolPtr;
    }
    if (const auto doublePtr(std::get_if<double>(&val)); doublePtr) {
        return *doublePtr != 0;
    }
    if (const auto strPtr(std::get_if<std::string>(&val)); strPtr) {
        return strPtr->length() != 0;
    }
    if (std::holds_alternative<std::monostate>(val)) {
        return false;
    }

    ErrorManager::handle_err(0, "Unsupported type to cast to bool");
    exit(1);
}

bool AstInterpreter::is_equal(const ExprVal &left, const ExprVal &right) {
    if (std::holds_alternative<bool>(left) ||
        std::holds_alternative<bool>(right)) {
        return cast_literal_to_bool(left) == cast_literal_to_bool(right);
    }

    // Compare variant: Type check then value check.
    return left == right;
}

void AstInterpreter::check_number_operand(std::shared_ptr<Token> tok,
                                          const ExprVal &right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
}

void AstInterpreter::check_int_operands(std::shared_ptr<Token> tok,
                                        const ExprVal &left,
                                        const ExprVal &right) {
    check_number_operands(tok, left, right);

    double left_double = std::get<double>(left);
    if (static_cast<int>(left_double) != left_double) {
        throw RuntimeException(tok, "Left operand must be an int");
    }
    double right_double = std::get<double>(right);
    if (static_cast<int>(right_double) != right_double) {
        throw RuntimeException(tok, "Right operand must be an int");
    }
}

void AstInterpreter::check_number_operands(std::shared_ptr<Token> tok,
                                           const ExprVal &left,
                                           const ExprVal &right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
    if (!std::holds_alternative<double>(left)) {
        throw RuntimeException(tok, "Left operand must be a number");
    }
}

void AstInterpreter::resolve_identifier(const IdentifierExpr &identifier_expr,
                                        int depth) {
    identifier_scope_depth[&identifier_expr] = depth;
}

std::shared_ptr<Environment> AstInterpreter::move_up_env(int depth) {
    auto env = this->env;
    for (int i = 0; i < depth; ++i) {
        env = env->parent_scope_env;
    }

    return env;
}
