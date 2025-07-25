#include "objects/StringObject.h"
#include "objects/IteratorObject.h"
#include <memory>

StringObject::StringObject(const std::string& v) : value(v) {}

std::string StringObject::type_name() const {
    return "string";
}

class StringIterator : public IteratorObject {
    const std::string& str;
    size_t index;
public:
    StringIterator(const std::string& s) : str(s), index(0) {}
    bool has_next() const override { return index < str.size(); }
    ObjectPtr next() override {
        if (!has_next()) return nullptr;
        char c = str[index++];
        return std::make_shared<StringObject>(std::string(1, c));
    }
    std::string type_name() const override { return "string_iterator"; }
};

std::shared_ptr<IteratorObject> StringObject::iter() const {
    return std::make_shared<StringIterator>(value);
} 