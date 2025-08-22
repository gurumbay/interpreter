#ifndef FUNCTION_OBJECT_H
#define FUNCTION_OBJECT_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "core/AST.h"
#include "core/Environment.h"
#include "objects/Object.h"

using BuiltinFunction = std::function<ObjectPtr(const std::vector<ObjectPtr>&)>;

class FunctionObject : public Object {
public:
    enum class FunctionType {
        BUILTIN,
        USER_DEFINED
    };

private:
    FunctionType type;
    
    // For built-in functions
    BuiltinFunction builtin_func;
    std::string builtin_name;
    
    // For user-defined functions
    std::vector<std::string> parameters;
    std::vector<const Stmt*> body;  // Raw pointers since AST outlives functions
    std::shared_ptr<Environment> closure;

public:
    // Constructor for built-in functions
    FunctionObject(const std::string& name, BuiltinFunction func)
        : type(FunctionType::BUILTIN), builtin_func(func), builtin_name(name) {}
    
    // Constructor for user-defined functions
    FunctionObject(const std::vector<std::string>& params,
                   std::vector<const Stmt*> func_body,
                   std::shared_ptr<Environment> env)
        : type(FunctionType::USER_DEFINED), parameters(params), 
          body(func_body), closure(env) {}
    
    std::string type_name() const override;
    
    FunctionType get_type() const { return type; }
    
    // For built-in functions
    BuiltinFunction get_builtin() const { return builtin_func; }
    std::string get_builtin_name() const { return builtin_name; }
    
    // For user-defined functions
    const std::vector<std::string>& get_parameters() const { return parameters; }
    const std::vector<const Stmt*>& get_body() const { return body; }
    std::shared_ptr<Environment> get_closure() const { return closure; }
};

#endif // FUNCTION_OBJECT_H
