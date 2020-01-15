#include "AudioParameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    AudioParameters::AudioParameters()
        : _sample_rate { 0 }
        , _sample_format { DEFAULT_SAMPLE_FORMAT }
        , _channel_layout { DEFAULT_CHANEL_LAYOUT }
        , _channels { 0 }
        , _frame_size { 0 } {
        setName("AudioParameters");
        setType(MediaType::Audio);
    }

    void AudioParameters::setSampleRate(int64_t sample_rate) {
        if (sample_rate < 1) {
            log_warning("Sample rate cannot be zero or less: " << sample_rate << ", ignored");
            return;
        }
        _sample_rate = sample_rate;
    }

    void AudioParameters::setSampleFormat(AVSampleFormat sample_format) {
        if (sample_format == AVSampleFormat::AV_SAMPLE_FMT_NONE) {
            log_warning("Sample format cannot be AV_SAMPLE_FMT_NONE, ignored");
            return;
        }
        if_not(utils::compatible_with_sample_format(_codec, sample_format)) {
            const auto defailt_mp3_sample_format = _codec->sample_fmts[0];;
            log_warning("Cannot set pixel format: " << sample_format
                        << " - " << _codec->name << " doesn't compatible with it, "
                        << "setting default: " << defailt_mp3_sample_format);
            _sample_format = defailt_mp3_sample_format;
            return;
        }
        _sample_format = sample_format;
    }

    void AudioParameters::setChannelLayout(uint64_t channel_layout) {
        _channel_layout = channel_layout;
    }

    void AudioParameters::setChannels(int64_t channels) {
        if (channels < 1) {
            log_warning("Channels cannot be less than one: " << channels << ", ignored");
            return;
        }
        _channels = channels;
    }

    void AudioParameters::setFrameSize(int64_t value) {
        if (value < 1) {
            log_warning("Frame size cannot be less than one: " << value << ", ignored");
            return;
        }
        _frame_size = value;
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

    int64_t AudioParameters::frameSize() const {
        return _frame_size;
    }

    std::string AudioParameters::toString() const {
        return Parameters::toString() + "; "
            + "sample_rate: " + std::to_string(sampleRate()) + ", "
            + utils::sample_format_to_string(sampleFormat()) + ", "
            + "channel_layout: " + std::to_string(channelLayout()) + ", "
            + "channels: " + std::to_string(channels());
    }

    Code AudioParameters::completeFrom(const ParametersPointer other_params) {
        auto other_audio_parames = std::static_pointer_cast<const AudioParameters>(other_params);
        if (not_inited_int(_sample_rate))           { setSampleRate(other_audio_parames->sampleRate());          }
        if (not_inited_smp_fmt(_sample_format))     { setSampleFormat(other_audio_parames->sampleFormat());      }
        if (not_inited_ch_layout(_channel_layout))  { setChannelLayout(other_audio_parames->channelLayout());    }
        if (not_inited_int(_channels))              { setChannels(other_audio_parames->channels());              }
        try_to(Parameters::completeFrom(other_params));
        return Code::OK;
    }

    void AudioParameters::parseStream(const AVStream* avstream) {
        setSampleRate(avstream->codecpar->sample_rate);
        setSampleFormat(avstream->codec->sample_fmt);
        setChannelLayout(avstream->codecpar->channel_layout);
        setChannels(avstream->codecpar->channels);
        setFrameSize(avstream->codecpar->frame_size);
        Parameters::parseStream(avstream);
    }

} // namespace fpp
