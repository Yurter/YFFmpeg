#include "AudioParameters.hpp"

namespace fpp {

    AudioParameters::AudioParameters() :
        Parameters(MediaType::MEDIA_TYPE_AUDIO),
        _sample_rate(DEFAULT_INT),
        _sample_format(DEFAULT_SAMPLE_FORMAT),
        _channel_layout(DEFAULT_CHANEL_LAYOUT),
        _channels(DEFAULT_INT)
    {
        setName("AudioParameters");
    }

    //AudioParameters::AudioParameters(Parameters parameters) :
    //    Parameters(parameters),
    //    _sample_rate(DEFAULT_INT),
    //    _sample_format(DEFAULT_SAMPLE_FORMAT),
    //    _channel_layout(DEFAULT_CHANEL_LAYOUT),
    //    _channels(DEFAULT_INT)
    //{
    //    setName("AudioParameters");
    //}

    void AudioParameters::setSampleRate(int64_t sample_rate) {
        _sample_rate = sample_rate;
    }

    void AudioParameters::setSampleFormat(AVSampleFormat sample_format) {
        _sample_format = sample_format;
    }

    void AudioParameters::setChannelLayout(uint64_t channel_layout) {
        _channel_layout = channel_layout;
    }

    void AudioParameters::setChannels(int64_t channels) {
        _channels = channels;
    }

    int64_t AudioParameters::sampleRate() const {
        return _sample_rate;
    }

    AVSampleFormat AudioParameters::sampleFormat() const {
        return _sample_format;
    }

    uint64_t AudioParameters::channelLayout() const {
        return _channel_layout;
    }

    int64_t AudioParameters::channels() const {
        return _channels;
    }

    std::string AudioParameters::toString() const //TODO
    {
        std::string str = "Sample rate: " + std::to_string(sampleRate()) + ", "
                + "sample format: " + av_get_sample_fmt_name(sampleFormat()) + "[" + std::to_string(sampleFormat()) + "], "
                + "channel layout: " + std::to_string(channelLayout()) + ", "
                + "channels: " + std::to_string(channels());
        return str;
    }

    //void AudioParameters::toCodecpar(AVCodecParameters* codecpar)
    //{
    //    codecpar->sample_rate       = int(_sample_rate);
    //    codecpar->channel_layout    = _channel_layout;
    //    Parameters::toCodecpar(codecpar);
    //}

    //void AudioParameters::softCopy(AudioParameters& other_parametrs)
    //{
    //    if (not_inited_int(_sample_rate))           { _sample_rate      = other_parametrs.sampleRate();     }
    //    if (not_inited_smp_fmt(_sample_format))     { _sample_format    = other_parametrs.sampleFormat();   }
    //    if (not_inited_ch_layout(_channel_layout)) { _channel_layout  = other_parametrs.channelLayout(); }
    //    if (not_inited_int(_channels))              { _channels         = other_parametrs.channels();       }
    //    Parameters::softCopy(other_parametrs);
    //}

    AudioParameters &AudioParameters::operator=(const AudioParameters& rhs) {
        _sample_rate    = rhs.sampleRate();
        _sample_format  = rhs.sampleFormat();
        _channel_layout = rhs.channelLayout();
        _channels       = rhs.channels();
        Parameters::operator=(rhs);
        return *this;
    }

} // namespace fpp
