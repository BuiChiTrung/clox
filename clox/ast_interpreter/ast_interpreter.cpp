#include "ast_interpreter.hpp"
#include "clox/ast_interpreter/callable.hpp"
#include "clox/ast_interpreter/environment.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"

#include <iostream>
#include <variant>

AstInterpreter::AstInterpreter() : global_env(std::make_shared<Environment>()) {
    env = global_env;
    global_env->add_new_variable("clock", std::make_shared<ClockNativeFunc>());
}

ExprVal
AstInterpreter::interpret_single_expr(std::shared_ptr<Expr> expression) {
    try {
        ExprVal result = evaluate_expr(expression);
        return result;
    } catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
        return NIL;
    }
}

void AstInterpreter::interpret_program(
    std::vector<std::shared_ptr<Stmt>> stmts) {
    try {
        for (const auto &stmt : stmts) {
            // exec stmt
            stmt->accept(*this);
        }
    } catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
    }
}

ExprVal AstInterpreter::evaluate_expr(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
}

void AstInterpreter::visit_expr_stmt(const ExprStmt &e) {
    evaluate_expr(e.expr);
}

void AstInterpreter::visit_assign_stmt(const AssignStmt &a) {
    ExprVal new_value = evaluate_expr(a.value);
    env->assign_new_value_to_variable(a.var->name, new_value);
}

void AstInterpreter::visit_print_stmt(const PrintStmt &p) {
    ExprVal val = evaluate_expr(p.expr);
    std::cout << exprval_to_string(val) << std::endl;
}

void AstInterpreter::visit_var_decl(const VarDecl &v) {
    ExprVal var_value = NIL;
    if (v.initializer != nullptr) {
        var_value = evaluate_expr(v.initializer);
    }
    env->add_new_variable(v.var_name->lexeme, var_value);
}

void AstInterpreter::visit_if_stmt(const IfStmt &i) {
    auto expr_val = evaluate_expr(i.condition);
    if (cast_literal_to_bool(expr_val)) {
        i.if_block->accept(*this);
    } else if (i.else_block != nullptr) {
        i.else_block->accept(*this);
    }
}

void AstInterpreter::visit_while_stmt(const WhileStmt &w) {
    while (cast_literal_to_bool(evaluate_expr(w.condition))) {
        w.body->accept(*this);
    }
}

void AstInterpreter::visit_function_decl(const FunctionDecl &w) {
    std::shared_ptr<LoxFunction> func(new LoxFunction(w, env));
    env->add_new_variable(w.name->lexeme, func);
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
        return_val = evaluate_expr(r.expr);
    }

    throw ReturnVal(return_val);
}

ExprVal AstInterpreter::visit_variable(const VariableExpr &v) {
    return env->get_variable(v.name);
}

ExprVal AstInterpreter::visit_literal(const LiteralExpr &l) { return l.value; }

ExprVal AstInterpreter::visit_grouping(const GroupExpr &g) {
    return evaluate_expr(g.expression);
}

ExprVal AstInterpreter::visit_func_call(const FuncCallExpr &f) {
    ExprVal callee = evaluate_expr(f.callee);

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
        arg_vals.push_back(evaluate_expr(arg));
    }

    return func->invoke(this, arg_vals);
}

ExprVal AstInterpreter::visit_unary(const UnaryExpr &u) {
    ExprVal right = evaluate_expr(u.right);

    switch (u.op->type) {
    case TokenType::BANG:
        return !cast_literal_to_bool(right);
    case TokenType::MINUS:
        checkNumberOperand(u.op, right);
        return -std::get<double>(right);
    default:
        ErrorManager::handle_err(u.op->line, "Invalid unary expression");
        return NIL;
    }
}

ExprVal AstInterpreter::visit_binary(const BinaryExpr &b) {
    ExprVal left = evaluate_expr(b.left);
    ExprVal right = evaluate_expr(b.right);

    auto left_double_ptr = std::get_if<double>(&left);
    auto right_double_ptr = std::get_if<double>(&right);
    auto left_string_ptr = std::get_if<std::string>(&left);
    auto right_string_ptr = std::get_if<std::string>(&right);

    switch (b.op->type) {
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
        throw RuntimeException(b.op, "Operands must be number or string");
    case TokenType::MINUS:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr - *right_double_ptr;
    case TokenType::STAR:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr * *right_double_ptr;
    case TokenType::MOD:
        checkIntOperands(b.op, left, right);
        return double(int(*left_double_ptr) % int(*right_double_ptr));
    case TokenType::SLASH:
        checkNumberOperands(b.op, left, right);
        if (*right_double_ptr == 0) {
            throw RuntimeException(b.op, "Devide by 0");
        }
        return *left_double_ptr / *right_double_ptr;
    case TokenType::GREATER:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr > *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr > *right_string_ptr;
        }
        throw RuntimeException(b.op, "Expected compare 2 numbers or 2 strings");
    case TokenType::LESS:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr < *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr < *right_string_ptr;
        }
        throw RuntimeException(b.op, "Expected compare 2 numbers or 2 strings");
    case TokenType::GREATER_EQUAL:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr >= *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr >= *right_string_ptr;
        }
        throw RuntimeException(b.op, "Expected compare 2 numbers or 2 strings");
    case TokenType::LESS_EQUAL:
        if (left_double_ptr && right_double_ptr) {
            return *left_double_ptr <= *right_double_ptr;
        }
        if (left_string_ptr && right_string_ptr) {
            return *left_string_ptr <= *right_string_ptr;
        }
        throw RuntimeException(b.op, "Expected compare 2 numbers or 2 strings");
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
        ErrorManager::handle_err(b.op->line, "Invalid binary expression");
        return NIL;
    }
}

bool AstInterpreter::cast_literal_to_bool(ExprVal val) {
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

bool AstInterpreter::is_equal(ExprVal left, ExprVal right) {
    if (std::holds_alternative<bool>(left) ||
        std::holds_alternative<bool>(right)) {
        return cast_literal_to_bool(left) == cast_literal_to_bool(right);
    }

    // Compare variant: Type check then value check.
    return left == right;
}

void AstInterpreter::checkNumberOperand(std::shared_ptr<Token> tok,
                                        ExprVal right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
}

void AstInterpreter::checkIntOperands(std::shared_ptr<Token> tok, ExprVal left,
                                      ExprVal right) {
    checkNumberOperands(tok, left, right);

    double left_double = std::get<double>(left);
    if (static_cast<int>(left_double) != left_double) {
        throw RuntimeException(tok, "Left operand must be an int");
    }
    double right_double = std::get<double>(right);
    if (static_cast<int>(right_double) != right_double) {
        throw RuntimeException(tok, "Right operand must be an int");
    }
}

void AstInterpreter::checkNumberOperands(std::shared_ptr<Token> tok,
                                         ExprVal left, ExprVal right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
    if (!std::holds_alternative<double>(left)) {
        throw RuntimeException(tok, "Left operand must be a number");
    }
}