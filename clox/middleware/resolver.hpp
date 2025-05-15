#include "clox/ast_interpreter/ast_interpreter.hpp"
#include "clox/parser/expr.hpp"
#include "clox/parser/stmt.hpp"
#include <memory>
#include <vector>

enum class ResolveFuncType {
    NONE,
    FUNCTION,
    // METHOD,
    // CLASS,
};

class IdentifierResolver : public IExprVisitor, public IStmtVisitor {
  private:
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

    void visit_return_stmt(const ReturnStmt &r) override;

    ExprVal visit_identifier(const IdentifierExpr &v) override;

    ExprVal visit_literal(const LiteralExpr &l) override;

    ExprVal visit_grouping(const GroupExpr &g) override;

    ExprVal visit_func_call(const FuncCallExpr &f) override;

    ExprVal visit_unary(const UnaryExpr &u) override;

    ExprVal visit_binary(const BinaryExpr &b) override;

    std::shared_ptr<AstInterpreter> interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes;
    ResolveFuncType current_func_type;

    void beginScope();
    void endScope();

    void declare_identifier(std::shared_ptr<Token> var_name);
    void define_identifier(std::shared_ptr<Token> var_name);

    void resolve_identifier(const IdentifierExpr &var_expr);

  public:
    IdentifierResolver(std::shared_ptr<AstInterpreter> interpreter);

    void resolve_program(std::vector<std::shared_ptr<Stmt>> &stmts);
};