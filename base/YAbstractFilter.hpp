#pragma once

#include "YDataProcessor.hpp"
#include "YFrame.hpp"

//TODO class name: YAbstractFilter? YFilter?
class YAbstractFilter : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFilter();


    // YObject interface
public:
    std::string toString() const;
};
