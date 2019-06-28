#pragma once

#include <string>

class YObject
{

public:

    YObject();
    virtual ~YObject() = default;

    void                setName(std::string name);
    std::string         name()  const;
    bool                is(std::string name);

    virtual std::string toString() const = 0;

    //TODO operator<< + toString()

private:

    std::string         _name;

};
