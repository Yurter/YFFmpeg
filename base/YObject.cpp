#include "YObject.h"

YObject::YObject() :
    _name("YObject"),
    _inited(false)
{
    //
}

void YObject::setName(std::string name)
{
    _name = name;
}

std::string YObject::name() const
{
    return _name;
}

bool YObject::is(std::string name)
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

std::ostream &operator<<(std::ostream& os, const YObject& obj)
{
    os << obj.toString();
    return os;
}
