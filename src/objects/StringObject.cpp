#include <memory>
#include "objects/StringObject.h"
#include "objects/IteratorObject.h"

StringObject::StringObject(const std::string& v) : value(v) {}

std::string StringObject::type_name() const {
    return "string";
}

std::shared_ptr<IteratorObject> StringObject::iter() const {
    return std::make_shared<StringIterator>(value);
}


StringIterator::StringIterator(const std::string& s) : str(s), index(0) {}

bool StringIterator::has_next() const { 
    return index < str.size(); 
}

ObjectPtr StringIterator::next() {
    if (!has_next()) return nullptr;
    char c = str[index++];
    return std::make_shared<StringObject>(std::string(1, c));
}

std::string StringIterator::type_name() const { 
    return "string_iterator"; 
}
