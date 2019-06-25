#pragma once

#include "base/YParameters.h"

class YAudioParameters : public YParameters
{

public:

    YAudioParameters(YParameters parameters = YParameters());
    virtual ~YAudioParameters() override;

    void            setSampleRate(int64_t sample_rate);
    void            setSampleFormat(AVSampleFormat sample_format);
    void            setChanelsLayout(uint64_t chanels_layout);
    void            setChanels(int64_t chanels);

    int64_t         sampleRate()    const;
    AVSampleFormat  sampleFormat()  const;
    uint64_t        chanelsLayout() const;
    int64_t         channels()       const;

    void            softCopy(YAudioParameters &other_parametrs);
    YAudioParameters&    operator=(const YAudioParameters &rhs);

private:

    int64_t         _sample_rate;
    AVSampleFormat  _sample_format;
    uint64_t        _chanels_layout;
    int64_t         _chanels;

};
