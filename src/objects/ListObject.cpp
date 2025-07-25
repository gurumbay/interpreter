#include "objects/ListObject.h"
#include "objects/IteratorObject.h"
#include <memory>

ListObject::ListObject() {}
ListObject::ListObject(const std::vector<ObjectPtr>& items) : items(items) {}

std::string ListObject::type_name() const {
    return "list";
}

std::shared_ptr<IteratorObject> ListObject::iter() const {
    return std::make_shared<ListIterator>(items);
}


ListIterator::ListIterator(const std::vector<ObjectPtr>& items) : items(items), index(0) {}

std::string ListIterator::type_name() const {
    return "list_iterator";
}

bool ListIterator::has_next() const {
    return index < items.size();
}

ObjectPtr ListIterator::next() {
    if (!has_next()) return nullptr;
    return items[index++];
}
