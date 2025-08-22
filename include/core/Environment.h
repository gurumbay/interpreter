#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>
#include "objects/Object.h"

class Environment {
private:
    std::unordered_map<std::string, ObjectPtr> values;
    std::shared_ptr<Environment> parent;

public:
    Environment();
    Environment(std::shared_ptr<Environment> parent);

    void set(const std::string& name, ObjectPtr value);
    ObjectPtr get(const std::string& name);
    void update(const std::string& name, ObjectPtr value);
    bool has(const std::string& name);
};

#endif // ENVIRONMENT_H
