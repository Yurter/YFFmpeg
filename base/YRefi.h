#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"
#include "YStream.h"

//TODO class name: YRefi? YRefi?
class YRefi : public YDataProcessor<YFrame,YFrame>
{

public:

    YRefi(streams_pair io_streams);
    virtual ~YRefi() override;

    virtual YCode       init() = 0;
    std::string         toString() const override;

protected:

    // Media
    streams_pair        _io_streams;

};

