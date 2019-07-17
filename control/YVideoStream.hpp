#pragma once

#include "base/YStream.hpp"
#include "YVideoParameters.hpp"

class YVideoStream : public YStream
{

public:

    YVideoStream(YVideoParameters* param = new YVideoParameters());
    YVideoStream(AVStream* stream, YVideoParameters* param);
    virtual ~YVideoStream() override = default;

    YCode               init() override;

    bool                operator>(const YStream& other) const override;

};

typedef std::list<YVideoStream*> VideoStreamList;

