#include <stdexcept>
#include "core/Environment.h"

Environment::Environment() : parent(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> parent) : parent(parent) {}

void Environment::set(const std::string& name, ObjectPtr value) {
    values[name] = value;
}

ObjectPtr Environment::get(const std::string& name) {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }
    
    if (parent != nullptr) {
        return parent->get(name);
    }
    
    throw std::runtime_error("Undefined variable: " + name);
}

void Environment::update(const std::string& name, ObjectPtr value) {
    auto it = values.find(name);
    if (it != values.end()) {
        it->second = value;
        return;
    }
    
    if (parent != nullptr) {
        parent->update(name, value);
        return;
    }
    
    throw std::runtime_error("Undefined variable: " + name);
}

bool Environment::has(const std::string& name) {
    auto it = values.find(name);
    if (it != values.end()) {
        return true;
    }
    
    if (parent != nullptr) {
        return parent->has(name);
    }
    
    return false;
}
