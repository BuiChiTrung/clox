#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include <memory>
#include <vector>

enum class ResolveFuncType {
    NONE,
    FUNCTION,
    METHOD,
    // Initializer is a special method (not allow return_kw, etc). It
    // should be a seperate type.
    INITIALIZER,
};

enum class ResolveClassType {
    NONE,
    CLASS,
    SUBCLASS,
};

enum class ResolveLoopType {
    NONE,
    LOOP,
};

class IdentifierResolver : public IExprVisitor, public IStmtVisitor {
  private:
    std::shared_ptr<AstInterpreter> interpreter = nullptr;
    std::vector<std::unordered_map<std::string, bool>> scopes = {};
    ResolveFuncType current_func_type = ResolveFuncType::NONE;
    ResolveClassType current_class_type = ResolveClassType::NONE;
    ResolveLoopType current_loop_type = ResolveLoopType::NONE;

    void resolve_stmts(const std::vector<std::shared_ptr<Stmt>> &stmts);

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

    ExprVal visit_this(const ThisExpr &) override;

    ExprVal visit_super(const SuperExpr &) override;

    ExprVal visit_literal(const LiteralExpr &) override;

    ExprVal visit_grouping(const GroupExpr &) override;

    ExprVal visit_func_call(const FuncCallExpr &) override;

    ExprVal visit_get_class_field(const GetClassFieldExpr &) override;

    ExprVal visit_unary(const UnaryExpr &) override;

    ExprVal visit_binary(const BinaryExpr &) override;

    void addScope();
    void closeScope();

    void declare_identifier(Token &identifier_name);
    void define_identifier(const Token &var_name);

    void resolve_identifier(const IdentifierExpr &);
    void resolve_function(const FunctionDecl &, ResolveFuncType);

  public:
    IdentifierResolver(std::shared_ptr<AstInterpreter> interpreter);

    void resolve_program(const std::vector<std::shared_ptr<Stmt>> &stmts);
};