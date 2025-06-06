#pragma once
#include "clox/ast_interpreter/environment.hpp"
#include "clox/common/token.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include <memory>
#include <unordered_map>

class AstInterpreter : public IExprVisitor, public IStmtVisitor {
  private:
    ExprVal evaluate_expr(Expr &expr);

    void visit_expr_stmt(const ExprStmt &) override;

    void visit_assign_stmt(const AssignStmt &) override;

    void visit_var_decl(const VarDecl &) override;

    void
    visit_block_stmt(const BlockStmt &,
                     std::shared_ptr<Environment> block_env = nullptr) override;

    void visit_if_stmt(const IfStmt &) override;

    void visit_while_stmt(const WhileStmt &) override;

    void visit_break_stmt(const BreakStmt &) override;

    void visit_continue_stmt(const ContinueStmt &) override;

    void visit_function_decl(FunctionDecl &) override;

    void visit_class_decl(const ClassDecl &) override;

    void visit_return_stmt(const ReturnStmt &) override;

    void visit_set_class_field(const SetClassFieldStmt &) override;

    ExprVal visit_identifier(const IdentifierExpr &) override;

    ExprVal visit_this(const ThisExpr &this_expr) override;

    ExprVal visit_super(const SuperExpr &super_expr) override;

    std::shared_ptr<LoxClass> cast_expr_val_to_lox_class(ExprVal &);

    ExprVal visit_literal(const LiteralExpr &) override;

    ExprVal visit_grouping(const GroupExpr &) override;

    ExprVal visit_func_call(const FuncCallExpr &) override;

    ExprVal visit_get_class_field(const GetClassFieldExpr &) override;

    ExprVal visit_unary(const UnaryExpr &) override;

    ExprVal visit_binary(const BinaryExpr &) override;

    bool cast_literal_to_bool(const ExprVal &val);

    bool is_equal(const ExprVal &left, const ExprVal &right);

    void check_number_operand(std::shared_ptr<Token> tok, const ExprVal &right);

    void check_number_operands(std::shared_ptr<Token> tok, const ExprVal &left,
                               const ExprVal &right);
    void check_int_operands(std::shared_ptr<Token> tok, const ExprVal &left,
                            const ExprVal &right);

    // Use with Resolver class to resolve in which scope an identifier (var or
    // func) is defined
    void update_identifier_scope_depth_map(const IdentifierExpr &, int depth);
    uint get_identifier_depth(const IdentifierExpr &);
    std::shared_ptr<Environment> move_up_env(int depth);

    std::shared_ptr<Environment> env;
    std::unordered_map<const IdentifierExpr *, int> identifier_scope_depth_map;
    const std::shared_ptr<Environment> global_env;

  public:
    const bool is_interactive_mode;

    AstInterpreter(const bool is_interactive_mode);

    ExprVal interpret_single_expr(Expr &expression);

    void interpret_program(const std::vector<std::shared_ptr<Stmt>> &stmts);

    friend class LoxFunction;
    friend class IdentifierResolver;
};