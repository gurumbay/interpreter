#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
struct Expr;
struct Stmt;

// --- Expression nodes ---
struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    double value;
    NumberExpr(double v) : value(v) {}
};

struct StringExpr : Expr {
    std::string value;
    StringExpr(const std::string& v) : value(v) {}
};

struct VariableExpr : Expr {
    std::string name;
    VariableExpr(const std::string& n) : name(n) {}
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    std::string op;
    BinaryExpr(std::unique_ptr<Expr> l, std::string o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
};

struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> operand;
    UnaryExpr(std::string o, std::unique_ptr<Expr> e)
        : op(std::move(o)), operand(std::move(e)) {}
};

struct AssignExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignExpr(const std::string& n, std::unique_ptr<Expr> v)
        : name(n), value(std::move(v)) {}
};

struct ListExpr : Expr {
    std::vector<std::unique_ptr<Expr>> elements;
    ListExpr(std::vector<std::unique_ptr<Expr>> elems)
        : elements(std::move(elems)) {}
};

struct IndexExpr : Expr {
    std::unique_ptr<Expr> collection;
    std::unique_ptr<Expr> index;
    IndexExpr(std::unique_ptr<Expr> coll, std::unique_ptr<Expr> idx)
        : collection(std::move(coll)), index(std::move(idx)) {}
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
    CallExpr(std::unique_ptr<Expr> c, std::vector<std::unique_ptr<Expr>> args)
        : callee(std::move(c)), arguments(std::move(args)) {}
};

struct MemberAccessExpr : Expr {
    std::unique_ptr<Expr> object;
    std::string member;
    MemberAccessExpr(std::unique_ptr<Expr> obj, const std::string& mem)
        : object(std::move(obj)), member(mem) {}
};

// --- Statement nodes ---
struct Stmt {
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ExpressionStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(const std::string& n, std::unique_ptr<Expr> v)
        : name(n), value(std::move(v)) {}
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> thenBranch;
    std::vector<std::unique_ptr<Stmt>> elseBranch;
    IfStmt(std::unique_ptr<Expr> cond,
           std::vector<std::unique_ptr<Stmt>> thenB,
           std::vector<std::unique_ptr<Stmt>> elseB)
        : condition(std::move(cond)),
          thenBranch(std::move(thenB)),
          elseBranch(std::move(elseB)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Stmt>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ForStmt : Stmt {
    std::string var;
    std::unique_ptr<Expr> iterable;
    std::vector<std::unique_ptr<Stmt>> body;
    ForStmt(const std::string& v, std::unique_ptr<Expr> iter, std::vector<std::unique_ptr<Stmt>> b)
        : var(v), iterable(std::move(iter)), body(std::move(b)) {}
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

struct BreakStmt : Stmt {
    BreakStmt() = default;
};

struct ContinueStmt : Stmt {
    ContinueStmt() = default;
};

struct FunctionDefStmt : Stmt {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Stmt>> body;
    FunctionDefStmt(const std::string& n, std::vector<std::string> params, std::vector<std::unique_ptr<Stmt>> b)
        : name(n), parameters(std::move(params)), body(std::move(b)) {}
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;
    ReturnStmt(std::unique_ptr<Expr> v) : value(std::move(v)) {}
};

#endif // AST_H
