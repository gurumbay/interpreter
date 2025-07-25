#ifndef STRING_OBJECT_H
#define STRING_OBJECT_H
#include "objects/Object.h"
#include "objects/IteratorObject.h"
#include <string>

class StringObject : public Object {
public:
    std::string value;
    StringObject(const std::string& v);
    std::string type_name() const override;
    std::shared_ptr<IteratorObject> iter() const;
}; 

#endif // STRING_OBJECT_H
