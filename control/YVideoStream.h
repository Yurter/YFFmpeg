#pragma once

#include "base/YStream.h"
#include "YVideoParameters.h"

class YVideoStream : public YStream
{

public:

    YVideoStream(YVideoParameters param = YVideoParameters());
    YVideoStream(YAbstractMedia* media_context, AVStream* stream, YVideoParameters param);
    virtual ~YVideoStream() override = default;

    YCode               init() override;

public:

    // Media
    YVideoParameters    parameters;

};

