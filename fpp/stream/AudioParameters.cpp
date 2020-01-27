#include "AudioParameters.hpp"
#include <fpp/core/Utils.hpp>

namespace ffmpeg { extern "C" {
    #include <libavformat/avformat.h>
} } // namespace ffmpeg

#define DEFAULT_SAMPLE_FORMAT   ffmpeg::AV_SAMPLE_FMT_NONE
#define DEFAULT_CHANEL_LAYOUT   0
#define DEFAULT_SAMPLE_RATE     44'100
#define not_inited_smp_fmt(x)   ((x) == DEFAULT_SAMPLE_FORMAT)
#define not_inited_ch_layout(x) ((x) == DEFAULT_CHANEL_LAYOUT)

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

    void AudioParameters::setSampleFormat(ffmpeg::AVSampleFormat sample_format) {
        if (sample_format == ffmpeg::AVSampleFormat::AV_SAMPLE_FMT_NONE) {
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

    ffmpeg::AVSampleFormat AudioParameters::sampleFormat() const {
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
            + utils::to_string(sampleFormat()) + ", "
            + "channel_layout: " + std::to_string(channelLayout()) + ", "
            + "channels: " + std::to_string(channels());
    }

    void AudioParameters::completeFrom(const SharedParameters other_params) {
        Parameters::completeFrom(other_params);
        auto other_audio_parames = utils::makeAudioParams(other_params);
        if (not_inited_int(sampleRate()))           { setSampleRate(other_audio_parames->sampleRate());         }
        if (not_inited_smp_fmt(sampleFormat()))     { setSampleFormat(other_audio_parames->sampleFormat());     }
        if (not_inited_ch_layout(channelLayout()))  { setChannelLayout(other_audio_parames->channelLayout());   }
        if (not_inited_int(channels()))             { setChannels(other_audio_parames->channels());             }
    }

    void AudioParameters::parseStream(const ffmpeg::AVStream* avstream) {
        Parameters::parseStream(avstream);
        setSampleRate(avstream->codecpar->sample_rate);
        setSampleFormat(ffmpeg::AVSampleFormat(avstream->codecpar->format));
        setChannelLayout(avstream->codecpar->channel_layout);
        setChannels(avstream->codecpar->channels);
        setFrameSize(avstream->codecpar->frame_size);
    }

    bool AudioParameters::betterThen(const SharedParameters& other) {
        auto other_aparams = utils::makeAudioParams(other);
        auto this_sound_quality = sampleRate() * channels();
        auto other_sound_quality = other_aparams->sampleRate() * other_aparams->channels();
        return this_sound_quality > other_sound_quality;
    }

} // namespace fpp