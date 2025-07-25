#include "objects/NumberObject.h"

NumberObject::NumberObject(double v) : value(v) {}

std::string NumberObject::type_name() const {
    return "number";
}
