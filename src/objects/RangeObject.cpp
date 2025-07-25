#include "objects/RangeObject.h"
#include "objects/NumberObject.h"

RangeObject::RangeObject(double start, double stop, double step)
    : start(start), stop(stop), step(step) {}

std::string RangeObject::type_name() const {
    return "range";
}

std::shared_ptr<IteratorObject> RangeObject::iter() const {
    return std::make_shared<RangeIterator>(start, stop, step);
}


RangeIterator::RangeIterator(double start, double stop, double step)
    : current(start), stop(stop), step(step), forward(step > 0) {}

std::string RangeIterator::type_name() const {
    return "range_iterator";
}

bool RangeIterator::has_next() const {
    return forward ? (current < stop) : (current > stop);
}

ObjectPtr RangeIterator::next() {
    if (!has_next()) return nullptr;
    double val = current;
    current += step;
    return std::make_shared<NumberObject>(val);
}
