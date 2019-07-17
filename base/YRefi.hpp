#pragma once

#include "YDataProcessor.hpp"
#include "YFrame.hpp"
#include "YStream.hpp"

//TODO class name: YRefi? YRefi?
class YRefi : public YDataProcessor<YFrame,YFrame>
{

public:

    YRefi(StreamPair io_streams);
    virtual ~YRefi() override;

    virtual YCode       init() = 0;
    std::string         toString() const override;

protected:

    // Media
    StreamPair        _io_streams;

};

