#include "clox/middleware/identifier_resolver.hpp"
#include "clox/common/constants.hpp"
#include "clox/common/error_manager.hpp"
#include "clox/common/token.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/utils/helper.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

IdentifierResolver::IdentifierResolver(
    std::shared_ptr<AstInterpreter> interpreter)
    : interpreter(interpreter) {
    scopes.emplace_back();
    for (const auto &identifier : interpreter->global_env->identifier_table) {
        scopes.back()[identifier.first] = true;
    }
};

void IdentifierResolver::resolve_program(
    const std::vector<std::shared_ptr<Stmt>> &stmts) {
    resolve_stmts(stmts);
}

void IdentifierResolver::resolve_stmts(
    const std::vector<std::shared_ptr<Stmt>> &stmts) {
    for (auto &stmt : stmts) {
        try {
            stmt->accept(*this);
        } catch (StaticException &err) {
            ErrorManager::handle_static_err(err);
            continue;
        }
    }
}

void IdentifierResolver::visit_expr_stmt(const ExprStmt &expr_stmt) {
    expr_stmt.expr->accept(*this);
}

void IdentifierResolver::visit_assign_stmt(const AssignStmt &assign_stmt) {
    assign_stmt.value->accept(*this);
    resolve_identifier(*assign_stmt.var);
}

void IdentifierResolver::visit_var_decl(const VarDecl &var_decl_stmt) {
    /*
       Need a pair of func declare and define to handle a special case: var is
       accessed in its initializer. var a = a;
        => raise error
    */
    declare_identifier(*var_decl_stmt.var_name);
    if (var_decl_stmt.initializer != nullptr) {
        var_decl_stmt.initializer->accept(*this);
    }
    define_identifier(*var_decl_stmt.var_name);
}

void IdentifierResolver::visit_block_stmt(
    const BlockStmt &block_stmt, std::shared_ptr<Environment> block_env) {
    addScope();
    for (auto stmt : block_stmt.stmts) {
        stmt->accept(*this);
    }
    closeScope();
}

void IdentifierResolver::visit_if_stmt(const IfStmt &if_stmt) {
    for (auto condition : if_stmt.conditions) {
        condition->accept(*this);
    }

    for (auto block : if_stmt.if_blocks) {
        block->accept(*this);
    }

    if (if_stmt.else_block != nullptr) {
        if_stmt.else_block->accept(*this);
    }
}

void IdentifierResolver::visit_while_stmt(const WhileStmt &while_stmt) {
    while_stmt.condition->accept(*this);
    ResolveLoopType enclosing_loop_type = current_loop_type;
    current_loop_type = ResolveLoopType::LOOP;
    while_stmt.body->accept(*this);
    current_loop_type = enclosing_loop_type;
}

void IdentifierResolver::visit_break_stmt(const BreakStmt &break_stmt) {
    if (current_loop_type == ResolveLoopType::NONE) {
        throw StaticException(
            break_stmt.break_kw,
            "'Break' statement can only be used inside a loop.");
    }
}

void IdentifierResolver::visit_continue_stmt(
    const ContinueStmt &continue_stmt) {
    if (current_loop_type == ResolveLoopType::NONE) {
        throw StaticException(
            continue_stmt.continue_kw,
            "'Continue' statement can only be used inside a loop.");
    }
}

void IdentifierResolver::visit_function_decl(FunctionDecl &func_decl_stmt) {
    resolve_function(func_decl_stmt, ResolveFuncType::FUNCTION);
}

void IdentifierResolver::visit_class_decl(const ClassDecl &class_decl_stmt) {
    declare_identifier(*class_decl_stmt.name);
    define_identifier(*class_decl_stmt.name);

    if (class_decl_stmt.superclass != nullptr) {
        if (class_decl_stmt.superclass->token->lexeme ==
            class_decl_stmt.name->lexeme) {
            throw StaticException(class_decl_stmt.superclass->token,
                                  "Class cannot extend itself.");
        }
        class_decl_stmt.superclass->accept(*this);
    }

    auto enclosing_class_type = current_class_type;
    current_class_type = ResolveClassType::CLASS;

    addScope(); // class scope
    scopes.back()["this"] = true;
    if (class_decl_stmt.superclass != nullptr) {
        current_class_type = ResolveClassType::SUBCLASS;
        scopes.back()["super"] = true;
    }
    for (auto method : class_decl_stmt.methods) {
        bool is_initializer =
            method->name->lexeme == class_decl_stmt.name->lexeme;
        if (is_initializer) {
            resolve_function(*method, ResolveFuncType::INITIALIZER);
        } else {
            resolve_function(*method, ResolveFuncType::METHOD);
        }
    }
    closeScope();

    current_class_type = enclosing_class_type;
}

void IdentifierResolver::resolve_function(const FunctionDecl &func_decl_stmt,
                                          ResolveFuncType func_type) {
    ResolveFuncType enclosing_func_type = current_func_type;
    current_func_type = func_type;

    declare_identifier(*func_decl_stmt.name);
    define_identifier(*func_decl_stmt.name);

    addScope();
    for (auto param : func_decl_stmt.params) {
        define_identifier(*param->token);
    }
    std::shared_ptr<BlockStmt> func_body =
        std::dynamic_pointer_cast<BlockStmt>(func_decl_stmt.body);
    resolve_stmts(func_body->stmts);
    closeScope();

    current_func_type = enclosing_func_type;
}

void IdentifierResolver::visit_return_stmt(const ReturnStmt &return_stmt) {
    if (current_func_type == ResolveFuncType::NONE) {
        throw StaticException(return_stmt.return_kw,
                              "Cannot return from outside a function.");
    }
    if (current_func_type == ResolveFuncType::INITIALIZER) {
        throw StaticException(return_stmt.return_kw,
                              "Cannot return inside the class initializer.");
    }
    return_stmt.expr->accept(*this);
}

void IdentifierResolver::visit_set_class_field(
    const SetClassFieldStmt &set_class_field_stmt) {
    set_class_field_stmt.lox_instance->accept(*this);
    set_class_field_stmt.value->accept(*this);
}

ExprVal
IdentifierResolver::visit_identifier(const IdentifierExpr &identifier_expr) {
    if (scopes.back().count(identifier_expr.token->lexeme) != 0 and
        scopes.back()[identifier_expr.token->lexeme] == false) {
        throw StaticException(
            identifier_expr.token,
            "Can't read local variable in its own initializer.");
    }

    resolve_identifier(identifier_expr);
    return NIL;
}

ExprVal IdentifierResolver::visit_this(const ThisExpr &this_expr) {
    if (current_class_type == ResolveClassType::NONE) {
        throw StaticException(this_expr.token,
                              "Can't use 'this' from outside a class method.");
    }
    resolve_identifier(this_expr);
    return NIL;
}

ExprVal IdentifierResolver::visit_super(const SuperExpr &super_expr) {
    if (current_class_type == ResolveClassType::NONE) {
        throw StaticException(super_expr.token,
                              "Can't use 'super' outside a subclass method.");
    } else if (current_class_type != ResolveClassType::SUBCLASS) {
        throw StaticException(
            super_expr.token,
            "Can't use 'super' inside a class with no super class.");
    }

    resolve_identifier(super_expr);
    return NIL;
}

void IdentifierResolver::resolve_identifier(
    const IdentifierExpr &identifier_expr) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(identifier_expr.token->lexeme) != 0) {
            interpreter->update_identifier_scope_depth_map(
                identifier_expr, scopes.size() - 1 - i);
            return;
        }
    }
}

ExprVal IdentifierResolver::visit_literal(const LiteralExpr &literal_expr) {
    return NIL;
}

ExprVal IdentifierResolver::visit_grouping(const GroupExpr &group_expr) {
    group_expr.expr->accept(*this);
    return NIL;
}

ExprVal
IdentifierResolver::visit_func_call(const FuncCallExpr &func_call_expr) {
    func_call_expr.callee->accept(*this);
    for (auto arg : func_call_expr.args) {
        arg->accept(*this);
    }
    return NIL;
}

ExprVal IdentifierResolver::visit_get_class_field(
    const GetClassFieldExpr &get_class_field_expr) {
    get_class_field_expr.lox_instance->accept(*this);
    return NIL;
}

ExprVal IdentifierResolver::visit_unary(const UnaryExpr &unary_expr) {
    unary_expr.operand->accept(*this);
    return NIL;
}

ExprVal IdentifierResolver::visit_binary(const BinaryExpr &binary_expr) {
    binary_expr.left_operand->accept(*this);
    binary_expr.right_operand->accept(*this);
    return NIL;
}

void IdentifierResolver::addScope() {
    scopes.push_back(std::unordered_map<std::string, bool>{});
}

void IdentifierResolver::closeScope() { scopes.pop_back(); }

/*
Adds identifier to the innermost scope so that it shadows any outer one and
so that we know the variable exists. We mark it as “not ready yet” by
binding its name to false in the scope map.
*/
void IdentifierResolver::declare_identifier(Token &identifier_name) {
    if (scopes.back().count(identifier_name.lexeme) != 0) {
        std::cout << "Huhu" << std::endl;
        throw StaticException(
            std::shared_ptr<Token>(&identifier_name,
                                   smart_pointer_no_op_deleter),
            "Variable with name " + identifier_name.lexeme +
                " already declared in this scope.");
    }
    scopes.back()[identifier_name.lexeme] = false;
}

// Mark identifier as resolved
void IdentifierResolver::define_identifier(const Token &identifier_name) {
    scopes.back()[identifier_name.lexeme] = true;
}
