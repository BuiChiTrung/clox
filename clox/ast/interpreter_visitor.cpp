#include "interpreter_visitor.hpp"
#include "clox/ast/expr.hpp"
#include "clox/error_manager.hpp"
#include "clox/token.hpp"
#include <format>
#include <iostream>
#include <variant>

LiteralVariant
InterpreterVisitor::interpret_single_expr(std::shared_ptr<Expr> expression) {
    try {
        LiteralVariant result = evaluate_expr(expression);
        return result;
    }
    catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
        return std::monostate();
    }
}

void InterpreterVisitor::interpret_program(
    std::vector<std::shared_ptr<Stmt>> stmts) {
    try {
        for (const auto &stmt : stmts) {
            // exec stmt
            stmt->accept(*this);
        }
        return;
    }
    catch (RuntimeException &err) {
        ErrorManager::handle_runtime_err(err);
        return;
    }
}

LiteralVariant InterpreterVisitor::evaluate_expr(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
}

void InterpreterVisitor::visit_expr_stmt(const ExprStmt &e) {
    evaluate_expr(e.expr);
    return;
}

void InterpreterVisitor::visit_assign_stmt(const AssignStmt &a) {
    LiteralVariant new_value = evaluate_expr(a.value);
    env->assign_new_value_to_variable(a.var->name, new_value);
    return;
}

void InterpreterVisitor::visit_print_stmt(const PrintStmt &p) {
    LiteralVariant val = evaluate_expr(p.expr);
    std::cout << literal_to_string(val) << std::endl;
    return;
}

void InterpreterVisitor::visit_var_stmt(const VarStmt &v) {
    LiteralVariant var_value = std::monostate();
    if (v.initializer != nullptr) {
        var_value = evaluate_expr(v.initializer);
    }
    env->add_new_variable(v.var_name, var_value);
    return;
}

void InterpreterVisitor::visit_if_stmt(const IfStmt &i) {
    auto expr_val = evaluate_expr(i.condition);
    if (cast_literal_to_bool(expr_val)) {
        i.if_block->accept(*this);
    }
    else if (i.else_block != nullptr) {
        i.else_block->accept(*this);
    }
    return;
}

void InterpreterVisitor::visit_while_stmt(const WhileStmt &w) {
    while (cast_literal_to_bool(evaluate_expr(w.condition))) {
        w.body->accept(*this);
    }
    return;
}

void InterpreterVisitor::visit_block_stmt(const BlockStmt &b) {
    auto parent_scope_env = this->env;
    this->env = std::make_shared<Environment>(parent_scope_env);
    for (auto stmt : b.stmts) {
        stmt->accept(*this);
    }
    this->env = parent_scope_env;
    return;
}

LiteralVariant InterpreterVisitor::visit_variable(const VariableExpr &v) {
    return env->get_variable(v.name);
}

LiteralVariant InterpreterVisitor::visit_literal(const LiteralExpr &l) {
    return l.value;
}

LiteralVariant InterpreterVisitor::visit_grouping(const GroupExpr &g) {
    return evaluate_expr(g.expression);
}

LiteralVariant InterpreterVisitor::visit_func_call(const FuncCallExpr &f) {
    LiteralVariant callee = evaluate_expr(f.callee);

    if (!std::holds_alternative<Callable>(callee)) {
        throw RuntimeException(f.close_parenthesis,
                               "Can only call functions and classes.");
    }

    Callable func = std::get<Callable>(callee);
    if (func.arg_num != f.args.size()) {
        throw RuntimeException(
            f.close_parenthesis,
            std::format(
                "Expected {} args to be passed to the function, but got {}",
                func.arg_num, f.args.size()));
    }

    std::vector<LiteralVariant> arg_vals{};
    for (auto arg : f.args) {
        arg_vals.push_back(evaluate_expr(arg));
    }

    return func.invoke();
}

LiteralVariant InterpreterVisitor::visit_unary(const UnaryExpr &u) {
    LiteralVariant right = evaluate_expr(u.right);

    switch (u.op->type) {
    case TokenType::BANG:
        return !cast_literal_to_bool(right);
    case TokenType::MINUS:
        checkNumberOperand(u.op, right);
        return -std::get<double>(right);
    default:
        ErrorManager::handle_err(u.op->line, "Invalid unary expression");
        return std::monostate();
    }
}

LiteralVariant InterpreterVisitor::visit_binary(const BinaryExpr &b) {
    LiteralVariant left = evaluate_expr(b.left);
    LiteralVariant right = evaluate_expr(b.right);

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
        throw RuntimeException(b.op,
                               "Operands must be two numbers or two strings");
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
        return *left_double_ptr / *right_double_ptr;
    case TokenType::GREATER:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr > *right_double_ptr;
    case TokenType::LESS:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr < *right_double_ptr;
    case TokenType::GREATER_EQUAL:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr >= *right_double_ptr;
    case TokenType::LESS_EQUAL:
        checkNumberOperands(b.op, left, right);
        return *left_double_ptr <= *right_double_ptr;
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
        return std::monostate();
    }
}

bool InterpreterVisitor::cast_literal_to_bool(LiteralVariant val) {
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

bool InterpreterVisitor::is_equal(LiteralVariant left, LiteralVariant right) {
    if (std::holds_alternative<bool>(left) ||
        std::holds_alternative<bool>(right)) {
        return cast_literal_to_bool(left) == cast_literal_to_bool(right);
    }

    // Compare variant: Type check then value check.
    return left == right;
}

void InterpreterVisitor::checkNumberOperand(std::shared_ptr<Token> tok,
                                            LiteralVariant right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
    return;
}

void InterpreterVisitor::checkIntOperands(std::shared_ptr<Token> tok,
                                          LiteralVariant left,
                                          LiteralVariant right) {
    checkNumberOperands(tok, left, right);

    double left_double = std::get<double>(left);
    if (static_cast<int>(left_double) != left_double) {
        throw RuntimeException(tok, "Left operand must be an int");
    }
    double right_double = std::get<double>(right);
    if (static_cast<int>(right_double) != right_double) {
        throw RuntimeException(tok, "Right operand must be an int");
    }

    return;
}

void InterpreterVisitor::checkNumberOperands(std::shared_ptr<Token> tok,
                                             LiteralVariant left,
                                             LiteralVariant right) {
    if (!std::holds_alternative<double>(right)) {
        throw RuntimeException(tok, "Right operand must be a number");
    }
    if (!std::holds_alternative<double>(left)) {
        throw RuntimeException(tok, "Left operand must be a number");
    }
    return;
}