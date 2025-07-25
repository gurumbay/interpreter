#ifndef NUMBER_OBJECT_H
#define NUMBER_OBJECT_H
#include "objects/Object.h"

class NumberObject : public Object {
public:
    double value;
    NumberObject(double v);
    std::string type_name() const override;
};

#endif // NUMBER_OBJECT_H
