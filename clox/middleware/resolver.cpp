#include "clox/middleware/resolver.hpp"
#include "clox/error_manager/error_manager.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include <memory>
#include <string>
#include <unordered_map>

Resolver::Resolver(std::shared_ptr<AstInterpreter> interpreter)
    : interpreter(interpreter) {
    scopes.emplace_back();
    for (const auto &identifier : interpreter->global_env->identifier_table) {
        scopes.back()[identifier.first] = true;
    }
};

void Resolver::visit_expr_stmt(const ExprStmt &expr_stmt) {
    expr_stmt.expr->accept(*this);
}

void Resolver::visit_assign_stmt(const AssignStmt &assign_stmt) {
    assign_stmt.value->accept(*this);
    resolve_identifier(*assign_stmt.var);
}

void Resolver::visit_print_stmt(const PrintStmt &print_stmt) {
    print_stmt.expr->accept(*this);
}

void Resolver::visit_var_decl(const VarDecl &var_decl_stmt) {
    declare_identifier(var_decl_stmt.var_name);
    if (var_decl_stmt.initializer != nullptr) {
        var_decl_stmt.initializer->accept(*this);
    }
    define_identifier(var_decl_stmt.var_name);
}

void Resolver::visit_block_stmt(const BlockStmt &block_stmt,
                                std::shared_ptr<Environment> block_env) {
    beginScope();
    for (auto stmt : block_stmt.stmts) {
        stmt->accept(*this);
    }
    endScope();
}

void Resolver::visit_if_stmt(const IfStmt &if_stmt) {
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

void Resolver::visit_while_stmt(const WhileStmt &while_stmt) {
    while_stmt.condition->accept(*this);
    while_stmt.body->accept(*this);
}

void Resolver::visit_function_decl(const FunctionDecl &func_decl_stmt) {
    declare_identifier(func_decl_stmt.name);
    define_identifier(func_decl_stmt.name);

    beginScope();
    for (auto param : func_decl_stmt.params) {
        define_identifier(param->name);
    }
    // TODO(trung.bc): techdebt
    std::shared_ptr<BlockStmt> func_body =
        std::dynamic_pointer_cast<BlockStmt>(func_decl_stmt.body);
    for (auto stmt : func_body->stmts) {
        stmt->accept(*this);
    }
    endScope();
}

void Resolver::visit_return_stmt(const ReturnStmt &return_stmt) {
    return_stmt.expr->accept(*this);
}

ExprVal Resolver::visit_identifier(const IdentifierExpr &identifier_expr) {
    if (scopes.back().count(identifier_expr.name->lexeme) != 0 and
        scopes.back()[identifier_expr.name->lexeme] == false) {
        ErrorManager::handle_err(
            identifier_expr.name->line,
            "Can't read local variable in its own initializer.");
    }

    resolve_identifier(identifier_expr);
    return NIL;
}

ExprVal Resolver::visit_literal(const LiteralExpr &literal_expr) { return NIL; }

ExprVal Resolver::visit_grouping(const GroupExpr &group_expr) {
    group_expr.expr->accept(*this);
    return NIL;
}

ExprVal Resolver::visit_func_call(const FuncCallExpr &func_call_expr) {
    func_call_expr.callee->accept(*this);
    for (auto arg : func_call_expr.args) {
        arg->accept(*this);
    }
    return NIL;
}

ExprVal Resolver::visit_unary(const UnaryExpr &unary_expr) {
    unary_expr.operand->accept(*this);
    return NIL;
}

ExprVal Resolver::visit_binary(const BinaryExpr &binary_expr) {
    binary_expr.left_operand->accept(*this);
    binary_expr.right_operand->accept(*this);
    return NIL;
}

void Resolver::beginScope() {
    scopes.push_back(std::unordered_map<std::string, bool>{});
}

void Resolver::endScope() { scopes.pop_back(); }

void Resolver::declare_identifier(std::shared_ptr<Token> identifier_name) {
    if (scopes.back().count(identifier_name->lexeme) != 0) {
        ErrorManager::handle_err(identifier_name->line,
                                 "Variable with name " +
                                     identifier_name->lexeme +
                                     " already declared in this scope.");
    }
    scopes.back()[identifier_name->lexeme] = false;
}
void Resolver::define_identifier(std::shared_ptr<Token> identifier_name) {
    scopes.back()[identifier_name->lexeme] = true;
}

void Resolver::resolve_identifier(const IdentifierExpr &identifier_expr) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(identifier_expr.name->lexeme) != 0) {
            interpreter->resolve_identifier(identifier_expr,
                                            scopes.size() - 1 - i);
            return;
        }
    }
}