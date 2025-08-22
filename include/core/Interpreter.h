#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <string>
#include <unordered_map>
#include "core/AST.h"
#include "core/Environment.h"
#include "objects/FunctionObject.h"
#include "objects/Object.h"

class BreakException : public std::exception {
    const char* what() const noexcept override;
};
class ContinueException : public std::exception {
    const char* what() const noexcept override;
};

class ReturnException : public std::exception {
public:
    ObjectPtr value;
    ReturnException(ObjectPtr v);
    const char* what() const noexcept override;
};

class Interpreter {
public:
    Interpreter();
    void run(const std::vector<std::unique_ptr<Stmt>>& statements);
private:
    std::shared_ptr<Environment> m_global_env;
    std::shared_ptr<Environment> m_current_env;
    
    void visit(const Stmt* stmt);
    void visitExpressionStmt(const ExpressionStmt* stmt);
    void visitAssignStmt(const AssignStmt* stmt);
    void visitIfStmt(const IfStmt* stmt);
    void visitWhileStmt(const WhileStmt* stmt);
    void visitForStmt(const ForStmt* stmt);
    void visitBlockStmt(const BlockStmt* stmt);
    void visitFunctionDefStmt(const FunctionDefStmt* stmt);
    void visitReturnStmt(const ReturnStmt* stmt);
    ObjectPtr eval(const Expr* expr);
    
    // Built-in functions
    void setupBuiltinFunctions();
    ObjectPtr callFunction(ObjectPtr callee, const std::vector<ObjectPtr>& arguments);
};

#endif // INTERPRETER_H
