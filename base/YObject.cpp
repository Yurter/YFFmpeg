#include "YObject.h"

YObject::YObject() :
    _name("YObject")
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

std::ostream &operator<<(std::ostream& os, const YObject& obj)
{
    os << obj.toString();
    return os;
}
