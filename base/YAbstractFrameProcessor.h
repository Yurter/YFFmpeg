#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"

class YAbstractFrameProcessor : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFrameProcessor();
    virtual ~YAbstractFrameProcessor();

protected:

    //

private:

    //


    // YDataProcessor interface
protected:
};

