#pragma once

#include <string>

class YObject
{

public:

    YObject();
    virtual ~YObject() = default;

    void                setName(std::string name);
    virtual std::string name() const final;

private:

    std::string     _name;

};
