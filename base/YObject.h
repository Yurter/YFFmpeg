#pragma once

#include <string>

class YObject
{

public:

    YObject();
    ~YObject() = default;

    void            setName(std::string name);
    std::string     name() const;

private:

    std::string     _name;

};
