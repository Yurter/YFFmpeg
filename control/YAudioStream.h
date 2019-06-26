#pragma once

#include "base/YStream.h"
#include "YAudioParameters.h"

class YAudioStream : public YStream
{

public:

    YAudioStream(YAudioParameters param = YAudioParameters());
    YAudioStream(AVStream* stream, YAudioParameters param);
    virtual ~YAudioStream() override = default;

    YCode               init() override;

};
