#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"

//TODO class name
class YAbstractFrameProcessor : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractFrameProcessor();
    virtual ~YAbstractFrameProcessor() override;

    virtual bool init(AVCodecContext *input_codec_context, AVCodecContext *output_codec_context) = 0;

protected:

    //

private:

    //


    // YDataProcessor interface
protected:
};

