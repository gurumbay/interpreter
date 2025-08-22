#ifndef STRING_OBJECT_H
#define STRING_OBJECT_H

#include <string>
#include "objects/Object.h"
#include "objects/IteratorObject.h"

class StringObject : public Object {
public:
    std::string value;
    StringObject(const std::string& v);
    std::string type_name() const override;
    std::shared_ptr<IteratorObject> iter() const;
};

class StringIterator : public IteratorObject {
    const std::string& str;
    size_t index;
public:
    StringIterator(const std::string& s);
    bool has_next() const override;
    ObjectPtr next() override;
    std::string type_name() const override;
}; 

#endif // STRING_OBJECT_H
