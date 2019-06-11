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
