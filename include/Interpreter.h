#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "AST.h"
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <iostream>

class Interpreter {
public:
    Interpreter();
    void run(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    using Value = std::variant<double, std::string>;
    std::unordered_map<std::string, Value> m_env;

    // Statement visitors
    void visit(const Stmt* stmt);
    void visitExpressionStmt(const ExpressionStmt* stmt);
    void visitAssignStmt(const AssignStmt* stmt);
    void visitIfStmt(const IfStmt* stmt);
    void visitWhileStmt(const WhileStmt* stmt);
    void visitForStmt(const ForStmt* stmt);
    void visitBlockStmt(const BlockStmt* stmt);

    // Expression visitors
    Value eval(const Expr* expr);
    Value evalNumberExpr(const NumberExpr* expr);
    Value evalStringExpr(const StringExpr* expr);
    Value evalVariableExpr(const VariableExpr* expr);
    Value evalBinaryExpr(const BinaryExpr* expr);
    Value evalUnaryExpr(const UnaryExpr* expr);
    Value evalAssignExpr(const AssignExpr* expr);
    Value evalCallExpr(const CallExpr* expr);
};

#endif // INTERPRETER_H
