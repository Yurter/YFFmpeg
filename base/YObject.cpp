#include "YObject.h"

YObject::YObject() :
    _name("YObject"),
    _inited(false)
{
    EMPTY_CONSTRUCTOR
}

void YObject::setName(std::string name)
{
    _name = name;
}

std::string YObject::name() const
{
    return _name;
}

bool YObject::is(std::string name) const
{
    return _name == name;
}

YCode YObject::init()
{
    return YCode::OK;
}

void YObject::setInited(bool inited)
{
    _inited = inited;
}

bool YObject::inited() const
{
    return _inited;
}

std::string YObject::toString() const
{
    return std::to_string(int64_t(this));
}

YObject::operator std::string() const
{
    return toString();
}

std::ostream &operator<<(std::ostream& os, const YObject& obj)
{
    os << obj.toString();
    return os;
}
