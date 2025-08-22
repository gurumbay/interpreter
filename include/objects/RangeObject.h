#ifndef RANGE_OBJECT_H
#define RANGE_OBJECT_H

#include <memory>
#include "objects/IteratorObject.h"
#include "objects/Object.h"

class RangeObject : public Object {
public:
    double start, stop, step;
    RangeObject(double start, double stop, double step);
    std::string type_name() const override;
    std::shared_ptr<IteratorObject> iter() const;
};

class RangeIterator : public IteratorObject {
    double current, stop, step;
    bool forward;
public:
    RangeIterator(double start, double stop, double step);
    bool has_next() const override;
    ObjectPtr next() override;
    std::string type_name() const override;
};

#endif // RANGE_OBJECT_H
