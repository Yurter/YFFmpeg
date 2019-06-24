#pragma once

#include "YDataProcessor.h"
#include "YFrame.h"
#include "YStream.h"

//TODO class name
class YAbstractRefi : public YDataProcessor<YFrame,YFrame>
{

public:

    YAbstractRefi(streams_pair audio_streams);
    virtual ~YAbstractRefi() override;

    virtual YCode init() = 0;

protected:

    //

private:

    //


    // YDataProcessor interface
protected:

    // YDataProcessor interface
protected:
};

