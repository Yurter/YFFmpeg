#pragma once

#include "base/YStream.h"
#include "YAudioParameters.h"

class YAudioStream : public YStream
{

public:

    YAudioStream();
    YAudioStream(YAbstractMedia* media_context, AVStream* stream);
    virtual ~YAudioStream() override = default;

    YCode               init() override;

public:

    // Media
    YAudioParameters    parameters;

};
