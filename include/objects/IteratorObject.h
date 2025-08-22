#ifndef ITERATOR_OBJECT_H
#define ITERATOR_OBJECT_H

#include "objects/Object.h"

class IteratorObject : public Object {
public:
    virtual bool has_next() const = 0;
    virtual ObjectPtr next() = 0;
    std::string type_name() const override;
}; 

#endif // ITERATOR_OBJECT_H
