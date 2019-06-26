#pragma once

#include "base/YStream.h"
#include "YVideoParameters.h"

class YVideoStream : public YStream
{

public:

    YVideoStream(YVideoParameters param = YVideoParameters());
    YVideoStream(AVStream* stream, YVideoParameters param);
    virtual ~YVideoStream() override = default;

    YCode               init() override;

};

