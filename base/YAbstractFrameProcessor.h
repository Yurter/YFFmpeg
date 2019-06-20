#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"
#include "YStream.h"

//TODO class name
class YAbstractFrameProcessor : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFrameProcessor(streams_pair audio_streams);
    virtual ~YAbstractFrameProcessor() override;

    virtual YCode init() = 0;

protected:

    //

private:

    //


    // YDataProcessor interface
protected:
};

