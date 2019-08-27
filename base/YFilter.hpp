#pragma once

#include "YDataProcessor.hpp"
#include "YFrame.hpp"

class YFilter : public YDataProcessor<YFrame,YFrame>
{

public:

    YFilter();

};
