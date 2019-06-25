#pragma once

#include "base/YStream.h"
#include "YAudioParameters.h"

class YAudioStream : public YStream
{

public:

    YAudioStream(YAudioParameters param = YAudioParameters());
    YAudioStream(YAbstractMedia* media_context, AVStream* stream, YAudioParameters param);
    virtual ~YAudioStream() override = default;

    YCode               init() override;

public:

    // Media
    YAudioParameters    parameters;

};
