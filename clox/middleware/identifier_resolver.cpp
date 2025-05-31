#include "clox/middleware/identifier_resolver.hpp"
#include "clox/common/error_manager.hpp"
#include "clox/common/token.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include <memory>
#include <string>
#include <unordered_map>

IdentifierResolver::IdentifierResolver(
    std::shared_ptr<AstInterpreter> interpreter)
    : interpreter(interpreter), current_func_type(ResolveFuncType::NONE) {
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
        stmt->accept(*this);
    }
}

void IdentifierResolver::visit_expr_stmt(const ExprStmt &expr_stmt) {
    expr_stmt.expr->accept(*this);
}

void IdentifierResolver::visit_assign_stmt(const AssignStmt &assign_stmt) {
    assign_stmt.value->accept(*this);
    resolve_identifier(*assign_stmt.var);
}

void IdentifierResolver::visit_print_stmt(const PrintStmt &print_stmt) {
    print_stmt.expr->accept(*this);
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
    while_stmt.body->accept(*this);
}

void IdentifierResolver::visit_function_decl(FunctionDecl &func_decl_stmt) {
    resolve_function(func_decl_stmt, ResolveFuncType::FUNCTION);
}

void IdentifierResolver::visit_class_decl(const ClassDecl &class_decl_stmt) {
    declare_identifier(*class_decl_stmt.name);
    define_identifier(*class_decl_stmt.name);

    auto enclosing_class_type = current_class_type;
    current_class_type = ResolveClassType::CLASS;

    addScope(); // class scope
    scopes.back()["this"] = true;
    for (auto method : class_decl_stmt.methods) {
        bool is_constructor =
            method->name->lexeme == class_decl_stmt.name->lexeme;
        if (is_constructor) {
            resolve_function(*method, ResolveFuncType::CONSTRUCTOR);
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
        define_identifier(*param->name);
    }
    std::shared_ptr<BlockStmt> func_body =
        std::dynamic_pointer_cast<BlockStmt>(func_decl_stmt.body);
    resolve_stmts(func_body->stmts);
    closeScope();

    current_func_type = enclosing_func_type;
}

void IdentifierResolver::visit_return_stmt(const ReturnStmt &return_stmt) {
    if (current_func_type == ResolveFuncType::NONE) {
        ErrorManager::handle_err(*return_stmt.return_kw,
                                 "Cannot return from outside a function.");
    }
    if (current_func_type == ResolveFuncType::CONSTRUCTOR) {
        ErrorManager::handle_err(*return_stmt.return_kw,
                                 "Cannot return inside the class constructor.");
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
    if (scopes.back().count(identifier_expr.name->lexeme) != 0 and
        scopes.back()[identifier_expr.name->lexeme] == false) {
        ErrorManager::handle_err(
            identifier_expr.name->line,
            "Can't read local variable in its own initializer.");
    }

    resolve_identifier(identifier_expr);
    return NIL;
}

ExprVal IdentifierResolver::visit_this(const ThisExpr &this_expr) {
    if (current_class_type != ResolveClassType::CLASS) {
        ErrorManager::handle_err(*this_expr.name,
                                 "Can't return from outside a class method.");
    }
    resolve_identifier(this_expr);
    return NIL;
}

void IdentifierResolver::resolve_identifier(
    const IdentifierExpr &identifier_expr) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(identifier_expr.name->lexeme) != 0) {
            interpreter->resolve_identifier(identifier_expr,
                                            scopes.size() - 1 - i);
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
Adds identifier to the innermost scope so that it shadows any outer one and so
that we know the variable exists. We mark it as “not ready yet” by binding its
name to false in the scope map.
*/
void IdentifierResolver::declare_identifier(const Token &identifier_name) {
    if (scopes.back().count(identifier_name.lexeme) != 0) {
        ErrorManager::handle_err(identifier_name.line,
                                 "Variable with name " +
                                     identifier_name.lexeme +
                                     " already declared in this scope.");
    }
    scopes.back()[identifier_name.lexeme] = false;
}

// Mark identifier as resolved
void IdentifierResolver::define_identifier(const Token &identifier_name) {
    scopes.back()[identifier_name.lexeme] = true;
}
