#pragma once

#include "base/YStream.h"
#include "YVideoParameters.h"

class YVideoStream : public YStream
{

public:

    YVideoStream();
    YVideoStream(AVStream* stream);
    virtual ~YVideoStream() override = default;

    bool    init() override;

public:

    // Media
    YVideoParameters    parameters;

};

