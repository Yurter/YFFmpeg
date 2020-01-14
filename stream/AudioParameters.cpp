#include "AudioParameters.hpp"
#include "core/utils.hpp"

namespace fpp {

    AudioParameters::AudioParameters() :
        Parameters(MediaType::MEDIA_TYPE_AUDIO)
      , _sample_rate(DEFAULT_INT)
      , _sample_format(DEFAULT_SAMPLE_FORMAT)
      , _channel_layout(DEFAULT_CHANEL_LAYOUT)
      , _channels(DEFAULT_INT)
      , _frame_size(DEFAULT_INT)
    {
        setName("AudioParameters");
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
            const auto defailt_mp3_sample_format = _codec->sample_fmts[0];//AV_SAMPLE_FMT_FLTP;
            log_warning("Cannot set pixel format: " << av_get_sample_fmt_name(sample_format)
                        << " - " << _codec->name << " doesn't compatible with it, "
                        << "setting default: " << av_get_sample_fmt_name(defailt_mp3_sample_format));
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
        std::string str = Parameters::toString() + "; ";
        str += "smplr: " + std::to_string(sampleRate()) + ", ";
        str += utils::sample_format_to_string(sampleFormat()) + ", ";
        str += "chlt: " + std::to_string(channelLayout()) + ", ";
        str += "ch: " + std::to_string(channels());
        return str;
    }

    Code AudioParameters::completeFrom(const Parameters* other_parametrs) {
        auto other_audio_parameters = static_cast<const AudioParameters*>(other_parametrs);
        if (not_inited_int(_sample_rate))           { setSampleRate(other_audio_parameters->sampleRate());          }
        if (not_inited_smp_fmt(_sample_format))     { setSampleFormat(other_audio_parameters->sampleFormat());      }
        if (not_inited_ch_layout(_channel_layout))  { setChannelLayout(other_audio_parameters->channelLayout());    }
        if (not_inited_int(_channels))              { setChannels(other_audio_parameters->channels());              }
        try_to(Parameters::completeFrom(other_parametrs));
        return Code::OK;
    }

} // namespace fpp
