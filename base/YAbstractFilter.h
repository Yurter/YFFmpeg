#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"

//TODO class name: YAbstractFilter? YFilter?
class YAbstractFilter : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFilter();


    // YObject interface
public:
    std::string toString() const;
};
