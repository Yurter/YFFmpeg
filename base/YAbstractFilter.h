#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"

class YAbstractFilter : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFilter();

};
