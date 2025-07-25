#include <iostream>
#include <stdexcept>
#include <cmath>
#include "core/Interpreter.h"
#include "objects/NumberObject.h"
#include "objects/StringObject.h"
#include "objects/ListObject.h"
#include "objects/RangeObject.h"
#include "objects/IteratorObject.h"

Interpreter::Interpreter() {}

void Interpreter::run(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        visit(stmt.get());
    }
}

void Interpreter::visit(const Stmt* stmt) {
    if (auto s = dynamic_cast<const ExpressionStmt*>(stmt)) visitExpressionStmt(s);
    else if (auto s = dynamic_cast<const AssignStmt*>(stmt)) visitAssignStmt(s);
    else if (auto s = dynamic_cast<const IfStmt*>(stmt)) visitIfStmt(s);
    else if (auto s = dynamic_cast<const WhileStmt*>(stmt)) visitWhileStmt(s);
    else if (auto s = dynamic_cast<const ForStmt*>(stmt)) visitForStmt(s);
    else if (auto s = dynamic_cast<const BlockStmt*>(stmt)) visitBlockStmt(s);
    else if (auto s = dynamic_cast<const BreakStmt*>(stmt)) throw BreakException();
    else if (auto s = dynamic_cast<const ContinueStmt*>(stmt)) throw ContinueException();
    else throw std::runtime_error("Unknown statement type");
}

void Interpreter::visitExpressionStmt(const ExpressionStmt* stmt) {
    eval(stmt->expr.get());
}

void Interpreter::visitAssignStmt(const AssignStmt* stmt) {
    ObjectPtr val = eval(stmt->value.get());
    m_env[stmt->name] = val;
}

void Interpreter::visitIfStmt(const IfStmt* stmt) {
    ObjectPtr cond = eval(stmt->condition.get());
    bool condVal = false;
    if (auto num = std::dynamic_pointer_cast<NumberObject>(cond)) condVal = num->value != 0.0;
    else if (auto str = std::dynamic_pointer_cast<StringObject>(cond)) condVal = !str->value.empty();
    else throw std::runtime_error("Invalid condition in if");
    if (condVal) {
        for (const auto& s : stmt->thenBranch) visit(s.get());
    } else {
        for (const auto& s : stmt->elseBranch) visit(s.get());
    }
}

void Interpreter::visitWhileStmt(const WhileStmt* stmt) {
    while (true) {
        ObjectPtr cond = eval(stmt->condition.get());
        bool condVal = false;
        if (auto num = std::dynamic_pointer_cast<NumberObject>(cond)) condVal = num->value != 0.0;
        else if (auto str = std::dynamic_pointer_cast<StringObject>(cond)) condVal = !str->value.empty();
        else throw std::runtime_error("Invalid condition in while");
        if (!condVal) break;
        try {
            for (const auto& s : stmt->body) visit(s.get());
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            continue;
        }
    }
}

void Interpreter::visitForStmt(const ForStmt* stmt) {
    ObjectPtr iterable = eval(stmt->iterable.get());
    std::shared_ptr<IteratorObject> iterator;
    // Try to get an iterator from the object
    if (auto range = std::dynamic_pointer_cast<RangeObject>(iterable)) {
        iterator = range->iter();
    } else if (auto str = std::dynamic_pointer_cast<StringObject>(iterable)) {
        iterator = str->iter();
    } else if (auto list = std::dynamic_pointer_cast<ListObject>(iterable)) {
        iterator = list->iter();
    } else {
        throw std::runtime_error("Object is not iterable");
    }
    while (iterator->has_next()) {
        m_env[stmt->var] = iterator->next();
        try {
            for (const auto& s : stmt->body) visit(s.get());
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            continue;
        }
    }
}

void Interpreter::visitBlockStmt(const BlockStmt* stmt) {
    for (const auto& s : stmt->statements) visit(s.get());
}

ObjectPtr Interpreter::eval(const Expr* expr) {
    if (auto e = dynamic_cast<const NumberExpr*>(expr)) return std::make_shared<NumberObject>(e->value);
    else if (auto e = dynamic_cast<const StringExpr*>(expr)) return std::make_shared<StringObject>(e->value);
    else if (auto e = dynamic_cast<const VariableExpr*>(expr)) {
        auto it = m_env.find(e->name);
        if (it == m_env.end()) throw std::runtime_error("Undefined variable: " + e->name);
        return it->second;
    } else if (auto e = dynamic_cast<const BinaryExpr*>(expr)) {
        ObjectPtr left = eval(e->left.get());
        ObjectPtr right = eval(e->right.get());
        const std::string& op = e->op;
        if (auto lnum = std::dynamic_pointer_cast<NumberObject>(left)) {
            if (auto rnum = std::dynamic_pointer_cast<NumberObject>(right)) {
                double l = lnum->value, r = rnum->value;
                if (op == "+") return std::make_shared<NumberObject>(l + r);
                if (op == "-") return std::make_shared<NumberObject>(l - r);
                if (op == "*") return std::make_shared<NumberObject>(l * r);
                if (op == "/") return std::make_shared<NumberObject>(l / r);
                if (op == "%") return std::make_shared<NumberObject>(std::fmod(l, r));
                if (op == "==") return std::make_shared<NumberObject>(l == r ? 1.0 : 0.0);
                if (op == "!=") return std::make_shared<NumberObject>(l != r ? 1.0 : 0.0);
                if (op == "<") return std::make_shared<NumberObject>(l < r ? 1.0 : 0.0);
                if (op == ">") return std::make_shared<NumberObject>(l > r ? 1.0 : 0.0);
                if (op == "<=") return std::make_shared<NumberObject>(l <= r ? 1.0 : 0.0);
                if (op == ">=") return std::make_shared<NumberObject>(l >= r ? 1.0 : 0.0);
                if (op == "and") return std::make_shared<NumberObject>((l && r) ? 1.0 : 0.0);
                if (op == "or") return std::make_shared<NumberObject>((l || r) ? 1.0 : 0.0);
                throw std::runtime_error("Unsupported binary operator for numbers: " + op);
            }
        }
        if (op == "+") {
            if (auto lstr = std::dynamic_pointer_cast<StringObject>(left)) {
                if (auto rstr = std::dynamic_pointer_cast<StringObject>(right)) {
                    return std::make_shared<StringObject>(lstr->value + rstr->value);
                }
            }
        }
        throw std::runtime_error("Type error in binary expression");
    } else if (auto e = dynamic_cast<const UnaryExpr*>(expr)) {
        ObjectPtr operand = eval(e->operand.get());
        if (e->op == "-") {
            if (auto num = std::dynamic_pointer_cast<NumberObject>(operand)) {
                return std::make_shared<NumberObject>(-num->value);
            }
            throw std::runtime_error("Unary '-' expects a number");
        }
        if (e->op == "not") {
            if (auto num = std::dynamic_pointer_cast<NumberObject>(operand)) return std::make_shared<NumberObject>(num->value == 0.0 ? 1.0 : 0.0);
            if (auto str = std::dynamic_pointer_cast<StringObject>(operand)) return std::make_shared<NumberObject>(str->value.empty() ? 1.0 : 0.0);
            throw std::runtime_error("Unary 'not' expects a number or string");
        }
        throw std::runtime_error("Unknown unary operator: " + e->op);
    } else if (auto e = dynamic_cast<const AssignExpr*>(expr)) {
        ObjectPtr val = eval(e->value.get());
        m_env[e->name] = val;
        return val;
    } else if (auto e = dynamic_cast<const CallExpr*>(expr)) {
        // Only support print and range as built-ins for now
        if (auto var = dynamic_cast<const VariableExpr*>(e->callee.get())) {
            if (var->name == "print") {
                bool first = true;
                for (const auto& arg : e->arguments) {
                    ObjectPtr val = eval(arg.get());
                    if (!first) std::cout << " ";
                    first = false;
                    if (auto num = std::dynamic_pointer_cast<NumberObject>(val)) {
                        std::cout << num->value;
                    } else if (auto str = std::dynamic_pointer_cast<StringObject>(val)) {
                        std::cout << str->value;
                    } else {
                        std::cout << "<object>";
                    }
                }
                std::cout << std::endl;
                return std::make_shared<NumberObject>(0.0);
            } else if (var->name == "range") {
                size_t argc = e->arguments.size();
                double start = 0, stop = 0, step = 1;
                if (argc == 1) {
                    ObjectPtr vstop = eval(e->arguments[0].get());
                    if (auto n = std::dynamic_pointer_cast<NumberObject>(vstop)) stop = n->value;
                    else throw std::runtime_error("range(stop) expects a number");
                } else if (argc == 2) {
                    ObjectPtr vstart = eval(e->arguments[0].get());
                    ObjectPtr vstop = eval(e->arguments[1].get());
                    if (auto n1 = std::dynamic_pointer_cast<NumberObject>(vstart)) start = n1->value;
                    else throw std::runtime_error("range(start, stop) expects numbers");
                    if (auto n2 = std::dynamic_pointer_cast<NumberObject>(vstop)) stop = n2->value;
                    else throw std::runtime_error("range(start, stop) expects numbers");
                } else if (argc == 3) {
                    ObjectPtr vstart = eval(e->arguments[0].get());
                    ObjectPtr vstop = eval(e->arguments[1].get());
                    ObjectPtr vstep = eval(e->arguments[2].get());
                    if (auto n1 = std::dynamic_pointer_cast<NumberObject>(vstart)) start = n1->value;
                    else throw std::runtime_error("range(start, stop, step) expects numbers");
                    if (auto n2 = std::dynamic_pointer_cast<NumberObject>(vstop)) stop = n2->value;
                    else throw std::runtime_error("range(start, stop, step) expects numbers");
                    if (auto n3 = std::dynamic_pointer_cast<NumberObject>(vstep)) step = n3->value;
                    else throw std::runtime_error("range(start, stop, step) expects numbers");
                    if (step == 0) throw std::runtime_error("range() step argument must not be zero");
                } else {
                    throw std::runtime_error("range() expects 1 to 3 arguments");
                }
                return std::make_shared<RangeObject>(start, stop, step);
            }
        }
        throw std::runtime_error("Only built-in functions supported are print() and range()");
    }
    throw std::runtime_error("Unknown expression type");
}
