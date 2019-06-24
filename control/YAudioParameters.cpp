#include "YAudioParameters.h"

YAudioParameters::YAudioParameters() :
    _sample_rate(DEFAULT_INT),
    _sample_format(DEFAULT_SAMPLE_FORMAT),
    _chanels_layout(DEFAULT_CHANEL_LAYOUT),
    _chanels(DEFAULT_INT)
{
    setName("YAudioParameters");
}

YAudioParameters::~YAudioParameters()
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

uint64_t YAudioParameters::chanelsLayout() const
{
    return _chanels_layout;
}

int64_t YAudioParameters::channels() const
{
    return _chanels;
}

void YAudioParameters::softCopy(YAudioParameters &other_parametrs)
{
    if (not_inited_int(_sample_rate))           { _sample_rate      = other_parametrs.sampleRate();     }
    if (not_inited_smp_fmt(_sample_format))     { _sample_format    = other_parametrs.sampleFormat();   }
    if (not_inited_ch_layout(_chanels_layout))  { _chanels_layout   = other_parametrs.chanelsLayout();  }
    if (not_inited_int(_chanels))               { _chanels          = other_parametrs.channels();        }
    YParameters::softCopy(other_parametrs);
}

YAudioParameters &YAudioParameters::operator=(const YAudioParameters &rhs)
{
    _sample_rate    = rhs.sampleRate();
    _sample_format  = rhs.sampleFormat();
    _chanels_layout = rhs.chanelsLayout();
    _chanels        = rhs.channels();
    YParameters::operator=(rhs);
    return *this;
}
