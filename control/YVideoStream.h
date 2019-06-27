#pragma once

#include "base/YStream.h"
#include "YVideoParameters.h"

class YVideoStream : public YStream
{

public:

    YVideoStream(YVideoParameters* param = new YVideoParameters());
    YVideoStream(AVStream* stream, YVideoParameters* param);
    virtual ~YVideoStream() override = default;

    YCode               init() override;

};

