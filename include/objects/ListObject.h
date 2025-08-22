#ifndef LIST_OBJECT_H
#define LIST_OBJECT_H

#include <vector>
#include "objects/IteratorObject.h"
#include "objects/Object.h"

class ListObject : public Object {
public:
    std::vector<ObjectPtr> items;
    ListObject();
    ListObject(const std::vector<ObjectPtr>& items);
    std::string type_name() const override;
    std::shared_ptr<IteratorObject> iter() const;
};

class ListIterator : public IteratorObject {
    const std::vector<ObjectPtr>& items;
    size_t index;
public:
    ListIterator(const std::vector<ObjectPtr>& items);
    bool has_next() const override;
    ObjectPtr next() override;
    std::string type_name() const override;
};

#endif // LIST_OBJECT_H
