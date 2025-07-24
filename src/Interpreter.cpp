#include "Interpreter.h"
#include <stdexcept>
#include <cmath>

Interpreter::Interpreter() {}

void Interpreter::run(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        visit(stmt.get());
    }
}

// --- Statement visitors ---
void Interpreter::visit(const Stmt* stmt) {
    if (auto s = dynamic_cast<const ExpressionStmt*>(stmt)) visitExpressionStmt(s);
    else if (auto s = dynamic_cast<const AssignStmt*>(stmt)) visitAssignStmt(s);
    else if (auto s = dynamic_cast<const IfStmt*>(stmt)) visitIfStmt(s);
    else if (auto s = dynamic_cast<const WhileStmt*>(stmt)) visitWhileStmt(s);
    else if (auto s = dynamic_cast<const ForStmt*>(stmt)) visitForStmt(s);
    else if (auto s = dynamic_cast<const BlockStmt*>(stmt)) visitBlockStmt(s);
    else throw std::runtime_error("Unknown statement type");
}

void Interpreter::visitExpressionStmt(const ExpressionStmt* stmt) {
    eval(stmt->expr.get());
}

void Interpreter::visitAssignStmt(const AssignStmt* stmt) {
    Value val = eval(stmt->value.get());
    m_env[stmt->name] = val;
}

void Interpreter::visitIfStmt(const IfStmt* stmt) {
    Value cond = eval(stmt->condition.get());
    bool condVal = false;
    if (std::holds_alternative<double>(cond)) condVal = std::get<double>(cond) != 0.0;
    else if (std::holds_alternative<std::string>(cond)) condVal = !std::get<std::string>(cond).empty();
    else throw std::runtime_error("Invalid condition in if");
    if (condVal) {
        for (const auto& s : stmt->thenBranch) visit(s.get());
    } else {
        for (const auto& s : stmt->elseBranch) visit(s.get());
    }
}

void Interpreter::visitWhileStmt(const WhileStmt* stmt) {
    while (true) {
        Value cond = eval(stmt->condition.get());
        bool condVal = false;
        if (std::holds_alternative<double>(cond)) condVal = std::get<double>(cond) != 0.0;
        else if (std::holds_alternative<std::string>(cond)) condVal = !std::get<std::string>(cond).empty();
        else throw std::runtime_error("Invalid condition in while");
        if (!condVal) break;
        for (const auto& s : stmt->body) visit(s.get());
    }
}

void Interpreter::visitForStmt(const ForStmt* stmt) {
    // Support range(stop), range(start, stop), range(start, stop, step)
    if (auto call = dynamic_cast<const CallExpr*>(stmt->iterable.get())) {
        if (auto var = dynamic_cast<const VariableExpr*>(call->callee.get())) {
            if (var->name == "range") {
                size_t argc = call->arguments.size();
                double start = 0, stop = 0, step = 1;
                if (argc == 1) {
                    Value vstop = eval(call->arguments[0].get());
                    if (!std::holds_alternative<double>(vstop)) throw std::runtime_error("range(stop) expects a number");
                    stop = std::get<double>(vstop);
                } else if (argc == 2) {
                    Value vstart = eval(call->arguments[0].get());
                    Value vstop = eval(call->arguments[1].get());
                    if (!std::holds_alternative<double>(vstart) || !std::holds_alternative<double>(vstop))
                        throw std::runtime_error("range(start, stop) expects numbers");
                    start = std::get<double>(vstart);
                    stop = std::get<double>(vstop);
                } else if (argc == 3) {
                    Value vstart = eval(call->arguments[0].get());
                    Value vstop = eval(call->arguments[1].get());
                    Value vstep = eval(call->arguments[2].get());
                    if (!std::holds_alternative<double>(vstart) || !std::holds_alternative<double>(vstop) || !std::holds_alternative<double>(vstep))
                        throw std::runtime_error("range(start, stop, step) expects numbers");
                    start = std::get<double>(vstart);
                    stop = std::get<double>(vstop);
                    step = std::get<double>(vstep);
                    if (step == 0) throw std::runtime_error("range() step argument must not be zero");
                } else {
                    throw std::runtime_error("range() expects 1 to 3 arguments");
                }
                if (step > 0) {
                    for (double i = start; i < stop; i += step) {
                        m_env[stmt->var] = i;
                        for (const auto& s : stmt->body) visit(s.get());
                    }
                } else {
                    for (double i = start; i > stop; i += step) {
                        m_env[stmt->var] = i;
                        for (const auto& s : stmt->body) visit(s.get());
                    }
                }
                return;
            }
        }
    }
    throw std::runtime_error("Only for var in range(...) is supported");
}

void Interpreter::visitBlockStmt(const BlockStmt* stmt) {
    for (const auto& s : stmt->statements) visit(s.get());
}

// --- Expression visitors ---
Interpreter::Value Interpreter::eval(const Expr* expr) {
    if (auto e = dynamic_cast<const NumberExpr*>(expr)) return evalNumberExpr(e);
    else if (auto e = dynamic_cast<const StringExpr*>(expr)) return evalStringExpr(e);
    else if (auto e = dynamic_cast<const VariableExpr*>(expr)) return evalVariableExpr(e);
    else if (auto e = dynamic_cast<const BinaryExpr*>(expr)) return evalBinaryExpr(e);
    else if (auto e = dynamic_cast<const UnaryExpr*>(expr)) return evalUnaryExpr(e);
    else if (auto e = dynamic_cast<const AssignExpr*>(expr)) return evalAssignExpr(e);
    else if (auto e = dynamic_cast<const CallExpr*>(expr)) return evalCallExpr(e);
    else throw std::runtime_error("Unknown expression type");
}

Interpreter::Value Interpreter::evalNumberExpr(const NumberExpr* expr) {
    return expr->value;
}

Interpreter::Value Interpreter::evalStringExpr(const StringExpr* expr) {
    return expr->value;
}

Interpreter::Value Interpreter::evalVariableExpr(const VariableExpr* expr) {
    auto it = m_env.find(expr->name);
    if (it == m_env.end()) throw std::runtime_error("Undefined variable: " + expr->name);
    return it->second;
}

Interpreter::Value Interpreter::evalBinaryExpr(const BinaryExpr* expr) {
    Value left = eval(expr->left.get());
    Value right = eval(expr->right.get());
    const std::string& op = expr->op;
    // Number operations
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
        double l = std::get<double>(left);
        double r = std::get<double>(right);
        if (op == "+") return l + r;
        if (op == "-") return l - r;
        if (op == "*") return l * r;
        if (op == "/") return l / r;
        if (op == "%") return std::fmod(l, r);
        if (op == "==") return l == r ? 1.0 : 0.0;
        if (op == "!=") return l != r ? 1.0 : 0.0;
        if (op == "<") return l < r ? 1.0 : 0.0;
        if (op == ">") return l > r ? 1.0 : 0.0;
        if (op == "<=") return l <= r ? 1.0 : 0.0;
        if (op == ">=") return l >= r ? 1.0 : 0.0;
        if (op == "and") return l && r ? 1.0 : 0.0;
        if (op == "or") return l || r ? 1.0 : 0.0;
        throw std::runtime_error("Unsupported binary operator for numbers: " + op);
    }
    // String concatenation
    if (op == "+" && std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
        return std::get<std::string>(left) + std::get<std::string>(right);
    }
    throw std::runtime_error("Type error in binary expression");
}

Interpreter::Value Interpreter::evalUnaryExpr(const UnaryExpr* expr) {
    Value operand = eval(expr->operand.get());
    if (expr->op == "-") {
        if (!std::holds_alternative<double>(operand)) throw std::runtime_error("Unary '-' expects a number");
        return -std::get<double>(operand);
    }
    if (expr->op == "not") {
        if (std::holds_alternative<double>(operand)) return std::get<double>(operand) == 0.0 ? 1.0 : 0.0;
        if (std::holds_alternative<std::string>(operand)) return std::get<std::string>(operand).empty() ? 1.0 : 0.0;
        throw std::runtime_error("Unary 'not' expects a number or string");
    }
    throw std::runtime_error("Unknown unary operator: " + expr->op);
}

Interpreter::Value Interpreter::evalAssignExpr(const AssignExpr* expr) {
    Value val = eval(expr->value.get());
    m_env[expr->name] = val;
    return val;
}

Interpreter::Value Interpreter::evalCallExpr(const CallExpr* expr) {
    // Only support print as a built-in for now
    if (auto var = dynamic_cast<const VariableExpr*>(expr->callee.get())) {
        if (var->name == "print") {
            bool first = true;
            for (const auto& arg : expr->arguments) {
                Value val = eval(arg.get());
                if (!first) std::cout << " ";
                first = false;
                if (std::holds_alternative<double>(val)) {
                    std::cout << std::get<double>(val);
                } else if (std::holds_alternative<std::string>(val)) {
                    std::cout << std::get<std::string>(val);
                } else {
                    throw std::runtime_error("Cannot print this value");
                }
            }
            std::cout << std::endl;
            return 0.0;
        }
    }
    throw std::runtime_error("Only built-in function supported is print()");
}
