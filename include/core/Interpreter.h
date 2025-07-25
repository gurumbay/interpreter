#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "core/AST.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "objects/Object.h"

class BreakException : public std::exception {
    const char* what() const noexcept override { return "Break"; }
};
class ContinueException : public std::exception {
    const char* what() const noexcept override { return "Continue"; }
};

class Interpreter {
public:
    Interpreter();
    void run(const std::vector<std::unique_ptr<Stmt>>& statements);
private:
    std::unordered_map<std::string, ObjectPtr> m_env;
    void visit(const Stmt* stmt);
    void visitExpressionStmt(const ExpressionStmt* stmt);
    void visitAssignStmt(const AssignStmt* stmt);
    void visitIfStmt(const IfStmt* stmt);
    void visitWhileStmt(const WhileStmt* stmt);
    void visitForStmt(const ForStmt* stmt);
    void visitBlockStmt(const BlockStmt* stmt);
    ObjectPtr eval(const Expr* expr);
};

#endif // INTERPRETER_H
