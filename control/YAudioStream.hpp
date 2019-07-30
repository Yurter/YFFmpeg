#pragma once

#include "base/YStream.hpp"
#include "YAudioParameters.hpp"

class YAudioStream : public YStream
{

public:

    YAudioStream(YAudioParameters* param = new YAudioParameters());
    YAudioStream(AVStream* stream, YAudioParameters* param);
    virtual ~YAudioStream() override = default;

    YCode               init() override;

    bool                operator>(const YStream& other) const override;

};

typedef std::list<YAudioStream*> AudioStreamList;