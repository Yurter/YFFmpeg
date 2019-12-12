#include "Object.hpp"

namespace fpp {

    Object::Object() :
        _name("Object"),
        _inited(false)
    {
        EMPTY_CONSTRUCTOR
    }

    void Object::setName(std::string name) {
        _name = name;
    }

    std::string Object::name() const {
//        /* fpp::Object */
//        const std::string name_with_namespace = typeid(*this).name();
//        /* Object */
//        const std::string name_without_namespace = name_with_namespace.substr(name_with_namespace.find_last_of(':') + 1);
//        return name_without_namespace;
        return _name;
    }

    bool Object::is(std::string name) const {
        return _name == name;
    }

    Code Object::init() {
        return Code::NOT_IMPLEMENTED;
    }

    void Object::setInited(bool inited) {
        _inited = inited;
    }

    bool Object::inited() const {
        return _inited;
    }

    std::string Object::toString() const {
        return "[" + _name + ":" + std::to_string(int64_t(this)) + "]";
//        return "{" + std::string(typeid(*this).name()) + ":" + std::to_string(int64_t(this)) + "}";
    }

    Object::operator std::string() const {
//        return Object::toString();
        return toString();
    }

} // namespace fpp
