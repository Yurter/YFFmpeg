#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"
#include "YStream.h"

class YAbstractRefi : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractRefi(streams_pair io_streams);
    virtual ~YAbstractRefi() override;

    virtual YCode       init() = 0;
    std::string         toString() const override;

protected:

    // Media
    streams_pair        _io_streams;

};

