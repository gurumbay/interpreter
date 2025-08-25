#include <cmath>
#include <iostream>
#include <stdexcept>
#include "core/Interpreter.h"
#include "objects/NumberObject.h"
#include "objects/StringObject.h"
#include "objects/ListObject.h"
#include "objects/RangeObject.h"
#include "objects/IteratorObject.h"
#include "objects/FunctionObject.h"

const char* BreakException::what() const noexcept {
    return "Break";
}

const char* ContinueException::what() const noexcept {
    return "Continue";
}

ReturnException::ReturnException(ObjectPtr v) : value(v) {}

const char* ReturnException::what() const noexcept {
    return "Return";
}

Interpreter::Interpreter() {
    m_global_env = std::make_shared<Environment>();
    m_current_env = m_global_env;
    setupBuiltinFunctions();
}

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
    else if (auto s = dynamic_cast<const FunctionDefStmt*>(stmt)) visitFunctionDefStmt(s);
    else if (auto s = dynamic_cast<const ReturnStmt*>(stmt)) visitReturnStmt(s);
    else if (auto s = dynamic_cast<const BreakStmt*>(stmt)) throw BreakException();
    else if (auto s = dynamic_cast<const ContinueStmt*>(stmt)) throw ContinueException();
    else throw std::runtime_error("Unknown statement type");
}

void Interpreter::visitExpressionStmt(const ExpressionStmt* stmt) {
    eval(stmt->expr.get());
}

void Interpreter::visitAssignStmt(const AssignStmt* stmt) {
    ObjectPtr val = eval(stmt->value.get());
    m_current_env->set(stmt->name, val);
}

void Interpreter::visitIfStmt(const IfStmt* stmt) {
    ObjectPtr cond = eval(stmt->condition.get());
    if (isTruthy(cond)) {
        for (const auto& s : stmt->thenBranch) visit(s.get());
    } else {
        for (const auto& s : stmt->elseBranch) visit(s.get());
    }
}

void Interpreter::visitWhileStmt(const WhileStmt* stmt) {
    while (isTruthy(eval(stmt->condition.get()))) {
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
        m_current_env->set(stmt->var, iterator->next());
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

void Interpreter::visitFunctionDefStmt(const FunctionDefStmt* stmt) {
    // Convert unique_ptr to raw pointers for function body
    std::vector<const Stmt*> body_ptrs;
    for (const auto& stmt_ptr : stmt->body) {
        body_ptrs.push_back(stmt_ptr.get());
    }
    
    auto function = std::make_shared<FunctionObject>(stmt->parameters, body_ptrs, m_current_env);
    m_current_env->set(stmt->name, function);
}

void Interpreter::visitReturnStmt(const ReturnStmt* stmt) {
    ObjectPtr value = nullptr;
    if (stmt->value) {
        value = eval(stmt->value.get());
    } else {
        value = std::make_shared<NumberObject>(0.0); // Default return value
    }
    throw ReturnException(value);
}

ObjectPtr Interpreter::eval(const Expr* expr) {
    if (auto e = dynamic_cast<const NumberExpr*>(expr)) return std::make_shared<NumberObject>(e->value);
    else if (auto e = dynamic_cast<const StringExpr*>(expr)) return std::make_shared<StringObject>(e->value);
    else if (auto e = dynamic_cast<const VariableExpr*>(expr)) {
        return m_current_env->get(e->name);
    } else if (auto e = dynamic_cast<const BinaryExpr*>(expr)) {
        ObjectPtr left = eval(e->left.get());
        ObjectPtr right = eval(e->right.get());
        const std::string& op = e->op;

        if (auto lnum = std::dynamic_pointer_cast<NumberObject>(left)) {
            if (auto rnum = std::dynamic_pointer_cast<NumberObject>(right)) {
                return evaluateNumberOperation(lnum->value, rnum->value, op);
            }
            if (auto rstr = std::dynamic_pointer_cast<StringObject>(right)) {
                return evaluateStringNumberOperation(rstr->value, lnum->value, op);
            }
        }
        if (auto lstr = std::dynamic_pointer_cast<StringObject>(left)) {
            if (auto rstr = std::dynamic_pointer_cast<StringObject>(right)) {
                return evaluateStringOperation(lstr->value, rstr->value, op);
            } else if (auto rnum = std::dynamic_pointer_cast<NumberObject>(right)) {
                return evaluateStringNumberOperation(lstr->value, rnum->value, op);
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
            return std::make_shared<NumberObject>(isTruthy(operand) ? 0.0 : 1.0);
        }
        throw std::runtime_error("Unknown unary operator: " + e->op);
    } else if (auto e = dynamic_cast<const AssignExpr*>(expr)) {
        ObjectPtr val = eval(e->value.get());
        m_current_env->update(e->name, val);
        return val;
    } else if (auto e = dynamic_cast<const CallExpr*>(expr)) {
        ObjectPtr callee = eval(e->callee.get());
        
        std::vector<ObjectPtr> arguments;
        for (const auto& arg : e->arguments) {
            arguments.push_back(eval(arg.get()));
        }
        
        return callFunction(callee, arguments);
    } else if (auto e = dynamic_cast<const MemberAccessExpr*>(expr)) {
        ObjectPtr object = eval(e->object.get());
        
        // For now, we'll support basic member access on strings and lists
        if (auto str = std::dynamic_pointer_cast<StringObject>(object)) {
            if (e->member == "length") {
                return std::make_shared<NumberObject>(static_cast<double>(str->value.length()));
            }
        } else if (auto list = std::dynamic_pointer_cast<ListObject>(object)) {
            if (e->member == "length") {
                return std::make_shared<NumberObject>(static_cast<double>(list->items.size()));
            }
        }
        
        throw std::runtime_error("Member '" + e->member + "' not found on object");
    } else if (auto e = dynamic_cast<const ListExpr*>(expr)) {
        std::vector<ObjectPtr> items;
        for (const auto& elem : e->elements)
            items.push_back(eval(elem.get()));
        return std::make_shared<ListObject>(items);
    } else if (auto e = dynamic_cast<const IndexExpr*>(expr)) {
        ObjectPtr collection = eval(e->collection.get());
        ObjectPtr index = eval(e->index.get());
        auto get_index = [&](int size) -> int {
            if (auto num = std::dynamic_pointer_cast<NumberObject>(index)) {
                int idx = static_cast<int>(num->value);
                if (idx < 0) idx += size;
                if (idx < 0 || idx >= size)
                    throw std::runtime_error("Index out of range");
                return idx;
            }
            throw std::runtime_error("Index must be a number");
        };
    
        if (auto list = std::dynamic_pointer_cast<ListObject>(collection)) {
            int idx = get_index(static_cast<int>(list->items.size()));
            return list->items[idx];
        } else if (auto str = std::dynamic_pointer_cast<StringObject>(collection)) {
            int idx = get_index(static_cast<int>(str->value.size()));
            return std::make_shared<StringObject>(std::string(1, str->value[idx]));
        }
        throw std::runtime_error("Object is not subscriptable");
    }
    throw std::runtime_error("Unknown expression type");
}

void Interpreter::setupBuiltinFunctions() {
    auto print_func = [](const std::vector<ObjectPtr>& args) -> ObjectPtr {
        bool first = true;
        for (const auto& arg : args) {
            if (!first) std::cout << " ";
            first = false;
            if (auto num = std::dynamic_pointer_cast<NumberObject>(arg)) {
                std::cout << num->value;
            } else if (auto str = std::dynamic_pointer_cast<StringObject>(arg)) {
                std::cout << str->value;
            } else {
                std::cout << "<object>";
            }
        }
        std::cout << std::endl;
        return std::make_shared<NumberObject>(0.0);
    };
    
    auto range_func = [](const std::vector<ObjectPtr>& args) -> ObjectPtr {
        size_t argc = args.size();
        double start = 0, stop = 0, step = 1;
        
        if (argc == 1) {
            if (auto n = std::dynamic_pointer_cast<NumberObject>(args[0])) stop = n->value;
            else throw std::runtime_error("range(stop) expects a number");
        } else if (argc == 2) {
            if (auto n1 = std::dynamic_pointer_cast<NumberObject>(args[0])) start = n1->value;
            else throw std::runtime_error("range(start, stop) expects numbers");
            if (auto n2 = std::dynamic_pointer_cast<NumberObject>(args[1])) stop = n2->value;
            else throw std::runtime_error("range(start, stop) expects numbers");
        } else if (argc == 3) {
            if (auto n1 = std::dynamic_pointer_cast<NumberObject>(args[0])) start = n1->value;
            else throw std::runtime_error("range(start, stop, step) expects numbers");
            if (auto n2 = std::dynamic_pointer_cast<NumberObject>(args[1])) stop = n2->value;
            else throw std::runtime_error("range(start, stop, step) expects numbers");
            if (auto n3 = std::dynamic_pointer_cast<NumberObject>(args[2])) step = n3->value;
            else throw std::runtime_error("range(start, stop, step) expects numbers");
            if (step == 0) throw std::runtime_error("range() step argument must not be zero");
        } else {
            throw std::runtime_error("range() expects 1 to 3 arguments");
        }
        return std::make_shared<RangeObject>(start, stop, step);
    };
    
    auto len_func = [](const std::vector<ObjectPtr>& args) -> ObjectPtr {
        if (args.size() != 1) {
            throw std::runtime_error("len() expects exactly 1 argument");
        }
        
        if (auto str = std::dynamic_pointer_cast<StringObject>(args[0])) {
            return std::make_shared<NumberObject>(static_cast<double>(str->value.length()));
        } else if (auto list = std::dynamic_pointer_cast<ListObject>(args[0])) {
            return std::make_shared<NumberObject>(static_cast<double>(list->items.size()));
        } else {
            throw std::runtime_error("len() expects a string or list");
        }
    };
    
    m_global_env->set("print", std::make_shared<FunctionObject>("print", print_func));
    m_global_env->set("range", std::make_shared<FunctionObject>("range", range_func));
    m_global_env->set("len", std::make_shared<FunctionObject>("len", len_func));
}

ObjectPtr Interpreter::callFunction(ObjectPtr callee, const std::vector<ObjectPtr>& arguments) {
    if (auto func = std::dynamic_pointer_cast<FunctionObject>(callee)) {
        if (func->get_type() == FunctionObject::FunctionType::BUILTIN) {
            return func->get_builtin()(arguments);
        } else {
            // User-defined function
            const auto& parameters = func->get_parameters();
            if (arguments.size() != parameters.size()) {
                throw std::runtime_error("Function expects " + std::to_string(parameters.size()) + 
                                       " arguments, got " + std::to_string(arguments.size()));
            }
            
            // Create new environment for function execution
            auto function_env = std::make_shared<Environment>(func->get_closure());
            
            // Bind parameters
            for (size_t i = 0; i < parameters.size(); ++i) {
                function_env->set(parameters[i], arguments[i]);
            }
            
            // Save current environment and switch to function environment
            auto previous_env = m_current_env;
            m_current_env = function_env;
            
                         try {
                 // Execute function body
                 for (const auto& stmt : func->get_body()) {
                     visit(stmt);
                 }
                // If no return statement, return default value
                m_current_env = previous_env;
                return std::make_shared<NumberObject>(0.0);
            } catch (const ReturnException& e) {
                m_current_env = previous_env;
                return e.value;
            }
        }
    }
    
    throw std::runtime_error("Can only call functions");
}

ObjectPtr Interpreter::evaluateNumberOperation(double left, double right, const std::string& op) {
    if (op == "+") return std::make_shared<NumberObject>(left + right);
    if (op == "-") return std::make_shared<NumberObject>(left - right);
    if (op == "*") return std::make_shared<NumberObject>(left * right);
    if (op == "/") return std::make_shared<NumberObject>(left / right);
    if (op == "%") return std::make_shared<NumberObject>(std::fmod(left, right));
    if (op == "==") return std::make_shared<NumberObject>(left == right ? 1.0 : 0.0);
    if (op == "!=") return std::make_shared<NumberObject>(left != right ? 1.0 : 0.0);
    if (op == "<") return std::make_shared<NumberObject>(left < right ? 1.0 : 0.0);
    if (op == ">") return std::make_shared<NumberObject>(left > right ? 1.0 : 0.0);
    if (op == "<=") return std::make_shared<NumberObject>(left <= right ? 1.0 : 0.0);
    if (op == ">=") return std::make_shared<NumberObject>(left >= right ? 1.0 : 0.0);
    if (op == "**") return std::make_shared<NumberObject>(std::pow(left, right));
    if (op == "and") return std::make_shared<NumberObject>((left && right) ? 1.0 : 0.0);
    if (op == "or") return std::make_shared<NumberObject>((left || right) ? 1.0 : 0.0);
    
    throw std::runtime_error("Unsupported binary operator for numbers: " + op);
}

ObjectPtr Interpreter::evaluateStringOperation(const std::string& left, const std::string& right, const std::string& op) {
    if (op == "+") return std::make_shared<StringObject>(left + right);
    
    throw std::runtime_error("Unsupported binary operator for strings: " + op);
}

ObjectPtr Interpreter::evaluateStringNumberOperation(const std::string& str, double num, const std::string& op) {
    if (op == "*") {
        // String repetition: "abc" * 3 = "abcabcabc"
        int count = static_cast<int>(num);
        if (count < 0) throw std::runtime_error("String repetition count must be non-negative");
        
        std::string result;
        result.reserve(str.length() * count);
        for (int i = 0; i < count; ++i) {
            result += str;
        }
        return std::make_shared<StringObject>(result);
    }
    
    throw std::runtime_error("Unsupported binary operator for string and number: " + op);
}

bool Interpreter::isTruthy(ObjectPtr value) {
    if (auto num = std::dynamic_pointer_cast<NumberObject>(value)) {
        return num->value != 0.0;
    }
    if (auto str = std::dynamic_pointer_cast<StringObject>(value)) {
        return !str->value.empty();
    }
    // All other objects are considered truthy
    return true;
}
