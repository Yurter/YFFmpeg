#include "YAudioParameters.h"

YAudioParameters::YAudioParameters() :
    YParameters(YMediaType::MEDIA_TYPE_AUDIO),
    _sample_rate(DEFAULT_INT),
    _sample_format(DEFAULT_SAMPLE_FORMAT),
    _channel_layout(DEFAULT_CHANEL_LAYOUT),
    _channels(DEFAULT_INT)
{
    setName("YAudioParameters");
}

//YAudioParameters::YAudioParameters(YParameters parameters) :
//    YParameters(parameters),
//    _sample_rate(DEFAULT_INT),
//    _sample_format(DEFAULT_SAMPLE_FORMAT),
//    _channel_layout(DEFAULT_CHANEL_LAYOUT),
//    _channels(DEFAULT_INT)
//{
//    setName("YAudioParameters");
//}

void YAudioParameters::setSampleRate(int64_t sample_rate)
{
    _sample_rate = sample_rate;
}

void YAudioParameters::setSampleFormat(AVSampleFormat sample_format)
{
    _sample_format = sample_format;
}

void YAudioParameters::setChannelLayout(uint64_t channel_layout)
{
    _channel_layout = channel_layout;
}

void YAudioParameters::setChannels(int64_t channels)
{
    _channels = channels;
}

int64_t YAudioParameters::sampleRate() const
{
    return _sample_rate;
}

AVSampleFormat YAudioParameters::sampleFormat() const
{
    return _sample_format;
}

uint64_t YAudioParameters::channelLayout() const
{
    return _channel_layout;
}

int64_t YAudioParameters::channels() const
{
    return _channels;
}

std::string YAudioParameters::toString() const //TODO
{
    std::string str = "Sample rate: " + std::to_string(sampleRate()) + ", "
            + "sample format: " + av_get_sample_fmt_name(sampleFormat()) + "[" + std::to_string(sampleFormat()) + "], "
            + "channel layout: " + std::to_string(channelLayout()) + ", "
            + "channels: " + std::to_string(channels());
    return str;
}

//void YAudioParameters::toCodecpar(AVCodecParameters* codecpar)
//{
//    codecpar->sample_rate       = int(_sample_rate);
//    codecpar->channel_layout    = _channel_layout;
//    YParameters::toCodecpar(codecpar);
//}

//void YAudioParameters::softCopy(YAudioParameters& other_parametrs)
//{
//    if (not_inited_int(_sample_rate))           { _sample_rate      = other_parametrs.sampleRate();     }
//    if (not_inited_smp_fmt(_sample_format))     { _sample_format    = other_parametrs.sampleFormat();   }
//    if (not_inited_ch_layout(_channel_layout)) { _channel_layout  = other_parametrs.channelLayout(); }
//    if (not_inited_int(_channels))              { _channels         = other_parametrs.channels();       }
//    YParameters::softCopy(other_parametrs);
//}

YAudioParameters &YAudioParameters::operator=(const YAudioParameters& rhs)
{
    _sample_rate    = rhs.sampleRate();
    _sample_format  = rhs.sampleFormat();
    _channel_layout = rhs.channelLayout();
    _channels       = rhs.channels();
    YParameters::operator=(rhs);
    return *this;
}
