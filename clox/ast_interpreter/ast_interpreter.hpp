#pragma once
#include "clox/ast_interpreter/environment.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include "clox/scanner/token.hpp"
#include <memory>
#include <unordered_map>

class AstInterpreter : public IExprVisitor, public IStmtVisitor {
  private:
    friend class LoxFunction;
    ExprVal evaluate_expr(std::shared_ptr<Expr> expr);

    void visit_expr_stmt(const ExprStmt &e) override;

    void visit_assign_stmt(const AssignStmt &a) override;

    void visit_print_stmt(const PrintStmt &p) override;

    void visit_var_decl(const VarDecl &v) override;

    void
    visit_block_stmt(const BlockStmt &b,
                     std::shared_ptr<Environment> block_env = nullptr) override;

    void visit_if_stmt(const IfStmt &b) override;

    void visit_while_stmt(const WhileStmt &w) override;

    void visit_function_decl(const FunctionDecl &f) override;

    void visit_class_decl(const ClassDecl &) override;

    void visit_return_stmt(const ReturnStmt &r) override;

    void visit_set_prop(const SetPropStmt &) override;

    ExprVal visit_identifier(const IdentifierExpr &v) override;

    ExprVal visit_literal(const LiteralExpr &l) override;

    ExprVal visit_grouping(const GroupExpr &g) override;

    ExprVal visit_func_call(const FuncCallExpr &f) override;

    ExprVal visit_get_prop(const GetPropExpr &) override;

    ExprVal visit_unary(const UnaryExpr &u) override;

    ExprVal visit_binary(const BinaryExpr &b) override;

    bool cast_literal_to_bool(ExprVal val);

    bool is_equal(ExprVal left, ExprVal right);

    void check_number_operand(std::shared_ptr<Token> tok, ExprVal right);

    void check_number_operands(std::shared_ptr<Token> tok, ExprVal left,
                               ExprVal right);
    void check_int_operands(std::shared_ptr<Token> tok, ExprVal left,
                            ExprVal right);

    std::shared_ptr<Environment> move_up_env(int depth);

  public:
    const std::shared_ptr<Environment> global_env;
    std::shared_ptr<Environment> env;
    const bool is_interactive_mode;
    std::unordered_map<const IdentifierExpr *, int> identifier_scope_depth;

    AstInterpreter(const bool is_interactive_mode);

    ExprVal interpret_single_expr(std::shared_ptr<Expr> expression);

    void interpret_program(const std::vector<std::shared_ptr<Stmt>> &stmts);

    // Use with Resolver class to resolve in which scope an identifier (var or
    // func) is defined
    void resolve_identifier(const IdentifierExpr &identifier_expr, int depth);
};