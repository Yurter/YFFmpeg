#pragma once

#include "base/YParameters.h"

class YAudioParameters : public YParameters
{

public:

//    YAudioParameters(YParameters parameters = YParameters());
    YAudioParameters();
    virtual ~YAudioParameters() override = default;

    void                setSampleRate(int64_t sample_rate);
    void                setSampleFormat(AVSampleFormat sample_format);
    void                setChannelLayout(uint64_t channels_layout);
    void                setChanels(int64_t channels);

    int64_t             sampleRate()    const;
    AVSampleFormat      sampleFormat()  const;
    uint64_t            channelLayout() const;
    int64_t             channels()      const;

    void                toCodecpar(AVCodecParameters* codecpar) override;
//    void                softCopy(YParameters* other_parametrs)  override;
    YAudioParameters&   operator=(const YAudioParameters& rhs);

private:

    int64_t             _sample_rate;
    AVSampleFormat      _sample_format;
    uint64_t            _channel_layout;
    int64_t             _channels;

};
