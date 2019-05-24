#pragma once

#include "YParameters.h"

class YAudioParameters : public YParameters
{

public:

    YAudioParameters();
    ~YAudioParameters();

    void            setSampleRate(int64_t sample_rate);
    void            setSampleFormat(AVSampleFormat sample_format);
    void            setChanelsLayout(int64_t chanels_layout);
    void            setChanels(int64_t chanels);

    int64_t         sampleRate()    const;
    AVSampleFormat  sampleFormat()  const;
    int64_t         chanelsLayout() const;
    int64_t         chanels()       const;

private:

    int64_t         _sample_rate;
    AVSampleFormat  _sample_format;
    int64_t         _chanels_layout;
    int64_t         _chanels;

};
