#include "YAudioParameters.h"

YAudioParameters::YAudioParameters()
{
    //
}

void YAudioParameters::setSampleRate(int64_t sample_rate)
{
    _sample_rate = sample_rate;
}

void YAudioParameters::setSampleFormat(AVSampleFormat sample_format)
{
    _sample_format = sample_format;
}

void YAudioParameters::setChanelsLayout(uint64_t chanels_layout)
{
    _chanels_layout = chanels_layout;
}

void YAudioParameters::setChanels(int64_t chanels)
{
    _chanels = chanels;
}

int64_t YAudioParameters::sampleRate() const
{
    return _sample_rate;
}

AVSampleFormat YAudioParameters::sampleFormat() const
{
    return _sample_format;
}

int64_t YAudioParameters::chanelsLayout() const
{
    return _chanels_layout;
}

int64_t YAudioParameters::chanels() const
{
    return _chanels;
}
