#pragma once

#include <string>
#include <iostream>

class YObject
{

public:

    YObject();
    virtual ~YObject() = default;

    void                setName(std::string name);
    std::string         name()  const;
    bool                is(std::string name);

    virtual std::string toString() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const YObject& obj);

private:

    std::string         _name;

};
