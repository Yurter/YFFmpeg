#include "AudioParameters.hpp"
#include "core/utils.hpp"

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

    std::string AudioParameters::toString() const {
        std::string str = Parameters::toString() + "; ";
        str += "smplr: " + std::to_string(sampleRate()) + ", ";
        str += std::string(av_get_sample_fmt_name(sampleFormat())) + ", ";
        str += "chlt: " + std::to_string(channelLayout()) + ", ";
        str += "ch: " + std::to_string(channels());
        return str;
    }

//    Code AudioParameters::completeFrom(const Parameters* other_parametrs) { //TODO заменить все присваивания на сеттеры
//        auto other_audio_parameters = static_cast<const AudioParameters*>(other_parametrs);
//        if (not_inited_int(_sample_rate))           { _sample_rate      = other_audio_parameters->sampleRate();     }
//        if (not_inited_smp_fmt(_sample_format))     { _sample_format    = other_audio_parameters->sampleFormat();   }
//        if (not_inited_ch_layout(_channel_layout))  { _channel_layout   = other_audio_parameters->channelLayout();  }
//        if (not_inited_int(_channels))              { _channels         = other_audio_parameters->channels();       }
//        try_to(Parameters::completeFrom(other_parametrs));
//        return Code::OK;
//    }
    Code AudioParameters::completeFrom(const Parameters* other_parametrs) { //TODO заменить все присваивания на сеттеры
        auto other_audio_parameters = static_cast<const AudioParameters*>(other_parametrs);
        if (not_inited_int(_sample_rate))           { setSampleRate(other_audio_parameters->sampleRate());          }
        if (not_inited_smp_fmt(_sample_format))     { setSampleFormat(other_audio_parameters->sampleFormat());      }
        if (not_inited_ch_layout(_channel_layout))  { setChannelLayout(other_audio_parameters->channelLayout());    }
        if (not_inited_int(_channels))              { setChannels(other_audio_parameters->channels());              }
        try_to(Parameters::completeFrom(other_parametrs));
        return Code::OK;
    }

} // namespace fpp
