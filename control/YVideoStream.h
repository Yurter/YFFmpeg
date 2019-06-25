#pragma once

#include "base/YStream.h"
#include "YVideoParameters.h"

class YVideoStream : public YStream
{

public:

    YVideoStream();
    YVideoStream(YAbstractMedia* media_context, AVStream* stream);
    virtual ~YVideoStream() override = default;

    YCode               init() override;

public:

    // Media
    YVideoParameters    parameters;

};

