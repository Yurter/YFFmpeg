#pragma once

#include "YFrame.h"
#include "YDataProcessor.h"

class YAbstractFilter : public YDataProcessor<YFrame,YFrame>
{
public:
    YAbstractFilter();
};
