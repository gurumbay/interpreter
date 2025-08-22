#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>

class Object {
public:
    virtual ~Object() = default;
    virtual std::string type_name() const = 0;
};
using ObjectPtr = std::shared_ptr<Object>;

# endif // OBJECT_H
